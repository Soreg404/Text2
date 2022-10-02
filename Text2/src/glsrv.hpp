#pragma once

namespace txt {
	namespace gl {

		int glInit();
		void setWSize(int width, int height);

		void getVO(GLuint *VAO, GLuint *VBO);

		GLuint loadTexture(int width, int height, unsigned char *buffer);
		void delTexture(GLuint *id);

		GLuint getEBO();
		GLuint getTU();

	}
}