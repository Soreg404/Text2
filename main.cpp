#include "util.h"

#include "text.hpp"

GLFWwindow *wnd = nullptr;
int WWIDTH = 800, WHEIGHT = 600;

int main() {

	// context creation

	glfwInit();

	wnd = glfwCreateWindow(WWIDTH, WHEIGHT, "context", nullptr, nullptr);
	glfwMakeContextCurrent(wnd);
	glViewport(0, 0, WWIDTH, WHEIGHT);

	glewInit();

	// text init
	txt::init(WWIDTH, WHEIGHT);

	txt::PFont f1 = txt::Font::load("res/font/arial.ttf");

	txt::SBound t1;
	t1.setText(L"Hello, world!");
	t1.setFont(f1);
	t1.setFontSize(48);
	t1.apply();
	t1.GPUload();

	while(!glfwWindowShouldClose(wnd)) {

		glfwWaitEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		t1.draw();

		glfwSwapBuffers(wnd);

	}

	glfwTerminate();

	return 0;
}