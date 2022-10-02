#include "util.h"
#include "glsrv.hpp"

GLuint shd = 0;
GLint texUnits = 0;

GLuint main_VBO = 0, EBO = 0;

void glerr(const char *file, int line) {
	int e = 0;
	while(e = glGetError()) printf("[%s] %i: GL ERROR: 0x%x\n", file, line, e);
}
#define GL(x) x; glerr(__FILE__, __LINE__);
#define GLERR glerr(__FILE__, __LINE__)

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

void loadShd();

int txt::gl::glInit() {

	LOG("GL init");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);	


	// check opengl ver
	const GLubyte *glVer = glGetString(GL_VERSION);
	LOG("GL version: %s", reinterpret_cast<const char *>(glVer));

	// get texUnits
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texUnits);
	LOG("GL texture units: %i", texUnits);

	// create shader
	loadShd();
	LOG("GL shader id: %u", shd);

	// instancing buffers
	int box[8]{
		1, 1,
		-1, 1,
		-1, -1,
		1, -1
	};
	glGenBuffers(2, &main_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, main_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);

	unsigned char ixs[] = {
		0, 1, 2,
		0, 2, 3
	};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ixs), ixs, GL_STATIC_DRAW);


	return 0;

}

int checkSHDErr(GLuint shd, bool cmp = true) {
	int success = 0;
	if(cmp) glGetShaderiv(shd, GL_COMPILE_STATUS, &success);
	else glGetProgramiv(shd, GL_LINK_STATUS, &success);

	if(!success) {
		char infoLog[1000] = { 0 };
		if(cmp) glGetShaderInfoLog(shd, 1000, NULL, infoLog);
		else glGetProgramInfoLog(shd, 1000, NULL, infoLog);
		LOG("error %s shader:\n%s\n", (cmp ? "compiling" : "linking"), infoLog);
	}
	return success;
}
GLint createShd(const wchar_t *path, GLenum type) {
	auto src = ldFile(path);
	const GLchar *data = src.data();
	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, &data, nullptr);
	glCompileShader(id);
	checkSHDErr(id);
	return id;
}
void loadShd() {
	GLint vert = createShd(L"res/shd/shd.vert", GL_VERTEX_SHADER);
	GLint frag = createShd(L"res/shd/shd.frag", GL_FRAGMENT_SHADER);

	shd = glCreateProgram();
	glAttachShader(shd, vert);
	glAttachShader(shd, frag);
	glLinkProgram(shd);
	glDeleteShader(vert);
	glDeleteShader(frag);

	if(checkSHDErr(shd, false)) {
		// populate sampler
		glUseProgram(shd);
		GLint loc = glGetUniformLocation(shd, "sampler");
		if(loc != -1)
			for(int i = 0; i < texUnits; i++) glUniform1i(loc + i, i);
		else LOG("GL couldn't find sampler location");
	}
}

void txt::gl::setWSize(int width, int height) {
	GLint loc = glGetUniformLocation(shd, "projection");
	if(loc != -1) {
		glm::mat4 proj = glm::ortho(0.f, static_cast<float>(width), 0.f, static_cast<float>(height), .1f, 100.f);
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));
	}
	else LOG("couldn't set window size in shader");


}

void txt::gl::getVO(GLuint *VAO, GLuint *VBO) {
	glGenVertexArrays(1, VAO);

	glBindVertexArray(*VAO);


	glBindBuffer(GL_ARRAY_BUFFER, main_VBO);
	glVertexAttribIPointer(0, 2, GL_INT, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glVertexAttribIPointer(1, 2, GL_INT, 5 * sizeof(int), 0);
	glEnableVertexAttribArray(1);

	glVertexAttribIPointer(2, 2, GL_INT, 5 * sizeof(int), (void *)(2 * sizeof(int)));
	glEnableVertexAttribArray(2);

	glVertexAttribIPointer(3, 1, GL_INT, 5 * sizeof(int), (void *)(4 * sizeof(int)));
	glEnableVertexAttribArray(3);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
}

GLuint txt::gl::loadTexture(int width, int height, unsigned char *buffer) {
	GLuint id;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// generate texture
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		width,
		height,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		buffer
	);
	// set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return id;
}

void txt::gl::delTexture(GLuint *id) {
	glDeleteTextures(1, id);
}

GLuint txt::gl::getEBO() { return EBO; }

GLuint txt::gl::getTU() { return texUnits; }

