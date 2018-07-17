#pragma once
#include "Matrix.h"

#include <chrono>

namespace win {
	static GLFWwindow* pWin;
	mat::Rvec<2> WASD{};
	mat::Rvec<2> arrowKeys{};
	//std::array<float, 6> numbers;
	mat::Rvec<3> Plane;

	mat::Rvec<2> cursor{};
	mat::Rvec<2> initialCursor{};
	bool movement = false;
	void cursor_cb(GLFWwindow* w, double fromLeft, double fromTop) {
		float x = remainder((static_cast<float>(fromLeft) - 400.f) / 400.f, 6.2832f);
		float y = remainder((-static_cast<float>(fromTop) + 400.f) / 400.f, 6.2832f);
		cursor = mat::Rvec<2>{ x, y } -initialCursor;
		if (movement == false) {
			movement = true;
			initialCursor = mat::Rvec<2>{ x, y };
		}
	}
	void click_cb(GLFWwindow* w, int button, int action, int mods) {}
	void key_cb(GLFWwindow* w, int key, int scancode, int action, int mod) {
		if (action != GLFW_REPEAT) {
			auto x = mat::Rvec<2>{ 1.0f, 0.0f };
			auto z = mat::Rvec<2>{ 0.0f, 1.0f };
			float press{};
			if (action == GLFW_PRESS) { press = 1.0f; }
			if (action == GLFW_RELEASE) { 
				press = -1.0f;
				if (key == GLFW_KEY_1) { Plane = mat::Rvec<3>{ 1.f, 0.f, 0.f }; }
				if (key == GLFW_KEY_2) { Plane = mat::Rvec<3>{ 0.f, 1.f, 0.f }; }
				if (key == GLFW_KEY_3) { Plane = mat::Rvec<3>{ 0.f, 0.f, 1.f }; }
				if (key == GLFW_KEY_4) { Plane = mat::Rvec<3>{ -1.f, 0.f, 0.f }; }
				if (key == GLFW_KEY_5) { Plane = mat::Rvec<3>{ 0.f, -1.f, 0.f }; }
				if (key == GLFW_KEY_6) { Plane = mat::Rvec<3>{ 0.f, 0.f, -1.f }; }
			}
			if (key == GLFW_KEY_W) { WASD -= press * z; }
			if (key == GLFW_KEY_A) { WASD -= press * x; }
			if (key == GLFW_KEY_S) { WASD += press * z; }
			if (key == GLFW_KEY_D) { WASD += press * x; }
			if (key == GLFW_KEY_UP) { arrowKeys -= press * z; }
			if (key == GLFW_KEY_LEFT) { arrowKeys -= press * x; }
			if (key == GLFW_KEY_DOWN) { arrowKeys += press * z; }
			if (key == GLFW_KEY_RIGHT) { arrowKeys += press * x; }


			if ((key == GLFW_KEY_ESCAPE) && action == GLFW_RELEASE) {
				glfwSetWindowShouldClose(pWin, 1);
			}
		}
	}
	class Window {
		std::chrono::steady_clock::time_point t0;
		std::chrono::steady_clock::time_point tLast;
	public:
		float tFrame;
		Window() : t0{ std::chrono::steady_clock::now() }, tLast{ std::chrono::steady_clock::now() }, tFrame{} {
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			pWin = glfwCreateWindow(800, 800, "Rubik cube", NULL, NULL);
			glfwMakeContextCurrent(pWin);
			setCallbacks();
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		}
		void swapAndPoll() {
			glfwSwapBuffers(win::pWin);
			glfwPollEvents();
			tFrame = ((std::chrono::steady_clock::now() - tLast).count()) / 10E8f;
			tLast = std::chrono::steady_clock::now();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		};
		void firstPerson() {
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glfwSetInputMode(win::pWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		~Window() {
			glfwTerminate();
		}
	private:
		void setCallbacks() {
			glfwSetCursorPosCallback(pWin, cursor_cb);
			glfwSetMouseButtonCallback(pWin, click_cb);
			glfwSetKeyCallback(pWin, key_cb);
		}
	};
}