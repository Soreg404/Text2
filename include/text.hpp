#pragma once

namespace txt {

	int init(int width, int height);
	void terminate();


	class Font {
		Font();
	public:

		static std::shared_ptr<Font> load(const char *path);

		~Font();

		struct ChInfo {
			GLuint texid = 0;
			int w = 0, h = 0, x = 0, y = 0, a = 0;
			unsigned int uses = 0;
		};

		void sub(unsigned int fSize, char32_t codepoint, unsigned int count);
		void unsub(unsigned int fSize, char32_t codepoint, unsigned int count);

		const ChInfo *get(unsigned int fSize, char32_t codepoint) const;

	private:
		void *fHandle = nullptr;

		struct ChSizes {
			std::map<unsigned int, ChInfo> sizes;
		};

		std::unordered_map<char32_t, ChSizes> chmap;

	};

	typedef std::shared_ptr<Font> PFont;

	// text bound with static size
	class SBound {

	public:

		SBound();
		~SBound();

		void setText(const std::wstring &newText);
		void setFont(PFont f);
		void setFontSize(unsigned int h);

		void setBoundSize(unsigned int w, unsigned int h);

		void apply();

		void GPUload();

		float scale = 1.f;
		glm::vec2 translate{ 0 };

		void draw();

	private:

		GLuint VAO = 0, VBO = 0;

		bool sendReady = false, drawReady = false;
		inline void resetState() { sendReady = false, drawReady = false; }

		std::wstring currText, newText;
		PFont currFont, newFont;

		unsigned int 
			fh = 16, nfh = 0,

			bw = 0, nbw = 0,
			bh = 0, nbh = 0;


		struct CHUSE { int count = 0, head = 0, tmpCounter = 0; };
		std::map<char32_t, CHUSE> chUsage;

		struct CHPOS {
			int xpos{ 0 }, ypos{ 0 };
			int width{ 0 }, height{ 0 };
			unsigned int texUnit{ 0 };
		};
		std::vector<CHPOS> chPosBuffer;

		// additional map vector


		void applyTextFont();

		void applyBoundSize();
		void applyPositions();


	};


}