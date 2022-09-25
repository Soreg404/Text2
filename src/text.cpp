#include "util.h"
#include "text.hpp"
#include "glsrv.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library tlib;

#define FONT(x) reinterpret_cast<FT_Face *>(&x)

int txt::init(int width, int height) {

	LOG("text init");

	// init freetype
	FT_Init_FreeType(&tlib);

	gl::glInit();
	gl::setWSize(width, height);

	return 0;
}

void txt::terminate() {
	FT_Done_FreeType(tlib);
	tlib = nullptr;
}


txt::Font::Font() {}

txt::PFont txt::Font::load(const char *path) {
	std::shared_ptr<Font> ft(new Font());

	FT_Face font;
	FT_Error error = FT_New_Face(tlib, path, 0, &font);

	if(error) {
		switch(error) {
			case FT_Err_Unknown_File_Format:
				LOG("[%s]: unknown file format", path);
				break;
			default:
				LOG("[%s]: loading font error", path);
		}
		return nullptr;
	}

	ft->fHandle = font;

	LOG("[%s]: font loaded", path);
	return ft;
}

txt::Font::~Font() {
	FT_Done_Face(*FONT(fHandle));
}


void txt::Font::sub(unsigned int fSize, char32_t codepoint, unsigned int count) {
	FT_Face *font = FONT(fHandle);
	auto &chInfo = chmap[codepoint].sizes[fSize];
	if(!chInfo.uses) {
		FT_Set_Pixel_Sizes(*font, 0, fSize);
		FT_Error error = FT_Load_Char(*font, codepoint, FT_LOAD_RENDER);
		if(error) {
			LOG("couldn't load char (%u)[%c]", (uint32_t)codepoint, codepoint);
			return;
		}

		FT_GlyphSlot gl = (*font)->glyph;

		chInfo.x = gl->bitmap_left;
		chInfo.y = gl->bitmap_top;

		chInfo.w = gl->bitmap.width;
		chInfo.h = gl->bitmap.rows;

		chInfo.a = gl->advance.x;

		chInfo.texid = gl::loadTexture(chInfo.w, chInfo.h, gl->bitmap.buffer);
	}
	chInfo.uses += count;
}

void txt::Font::unsub(unsigned int fSize, char32_t codepoint, unsigned int count) {
	auto foundCh = chmap.find(codepoint);
	if(foundCh == chmap.end()) return;

	auto chInfo = foundCh->second.sizes.find(fSize);
	if(chInfo == foundCh->second.sizes.end()) return;

	if(chInfo->second.uses <= count) {
		gl::delTexture(&chInfo->second.texid);

		foundCh->second.sizes.erase(chInfo);

		if(foundCh->second.sizes.size() == 0)
			chmap.erase(foundCh);

	} else chInfo->second.uses -= count;
}

const txt::Font::ChInfo *txt::Font::get(unsigned int fSize, char32_t codepoint) const {
	auto foundCh = chmap.find(codepoint);
	if(foundCh == chmap.end()) return nullptr;
	auto foundSize = foundCh->second.sizes.find(fSize);
	if(foundSize == foundCh->second.sizes.end()) return nullptr;
	return &foundSize->second;
}


txt::SBound::SBound() {
	gl::getVO(&VAO, &VBO);
	LOG("SBound created (VAO: %i VBO: %i)", VAO, VBO);
}

txt::SBound::~SBound() {}

void txt::SBound::setText(const std::wstring &newText) { this->newText = newText; resetState(); }

void txt::SBound::setFont(txt::PFont newFont) { this->newFont = newFont; resetState(); }
void txt::SBound::setFontSize(unsigned int h) { nfh = h; resetState(); }

void txt::SBound::setBoundSize(unsigned int w, unsigned int h) { nbw = w, nbh = h; resetState(); }

void txt::SBound::apply() {
	
	if(drawReady) return;

	if(!currFont && !newFont) {
		LOG("no font selected");
		return;
	}

	applyTextFont();

	applyBoundSize();

	applyPositions();

	sendReady = true;

}

void txt::SBound::GPUload() {
	if(!sendReady) return;

	int texUnit = 0;
	for(auto &c : chUsage) {
		for(int i = c.second.head; i < c.second.head + c.second.count; i++) chPosBuffer[i].texUnit = texUnit;
		texUnit++;
		if(texUnit == gl::getTU()) texUnit = 0;
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, chPosBuffer.size() * sizeof(CHPOS), chPosBuffer.data(), GL_STATIC_DRAW);
	drawReady = true;
}

void txt::SBound::draw() {
	if(!sendReady || !drawReady) return;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl::getEBO());

	int tBound_i = 0;
	for(auto &c : chUsage) {
		glActiveTexture(GL_TEXTURE0 + tBound_i);
		glBindTexture(GL_TEXTURE_2D, currFont->get(fh, c.first)->texid);
		tBound_i++;
	}

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0, chPosBuffer.size());
}

void txt::SBound::applyTextFont() {

	if(!newFont) newFont = currFont;
	if(nfh == 0) nfh = fh;

	std::map<char32_t, CHUSE> newChUsage;
	for(auto &c : newText) {
		newChUsage[c].count++;
		// TODO: utf-16 to utf-32 ???
		char32_t cp = c;//getCpFromUTF16();
		newFont->sub(nfh, cp, 1);
	}

	int tmpHead = 0;
	for(auto &c : newChUsage) {
		c.second.head = tmpHead;
		tmpHead += c.second.count;
	}

	if(currFont) {
		for(auto &c : chUsage) {
			// TODO: utf-16 to utf-32 ???
			char32_t cp = c.first;//getCpFromUTF16();
			currFont->unsub(fh, cp, c.second.count);
		}
	}

	fh = nfh;
	currFont = std::move(newFont);
	newFont = nullptr;
	currText = std::move(newText);
	newText = L"";

	chUsage = newChUsage;

	LOG("applied text & font");

}

void txt::SBound::applyBoundSize() { bw = nbw, bh = nbh; }

void txt::SBound::applyPositions() {

	chPosBuffer.resize(currText.length());

	int advance = 0, line = 0;

	for(auto &c : currText) {

		// TODO: utf-16 to utf-32 ???
		char32_t cp = c;//getCpFromUTF16();

		auto chBuffInfoFound = chUsage.find(cp);
		if(chBuffInfoFound == chUsage.end()) {
			LOG("char usage info not found & das bad");
			continue;
		}
		CHUSE *chb = &chBuffInfoFound->second;

		const Font::ChInfo *chInfo = currFont->get(fh, cp);
		if(!chInfo) {
			LOG("char texture not found & das bad");
			continue;
		}

		int index = chb->head + chb->tmpCounter;
		chb->tmpCounter++;
		if(chb->tmpCounter == chb->count) chb->tmpCounter = 0;

		CHPOS insertPos;
		insertPos.xpos = advance + chInfo->x;
		insertPos.ypos = line * fh + chInfo->y;

		insertPos.width = chInfo->w;
		insertPos.height = chInfo->h;

		advance += chInfo->a >> 6;

		chPosBuffer[index] = insertPos;

	}

	LOG("applied positions");

}
