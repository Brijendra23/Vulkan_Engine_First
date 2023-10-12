#pragma once
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<iostream>
#include<stdlib.h>
#include <string>
class Window
{ public:
	Window();
	void CreateWindow();
	void CreateWindow(unsigned int width,unsigned int height, const char * name,int resizable);
	void windowHint(int hint, int value);
	void cleanup(GLFWwindow *window);

	GLFWwindow* window;

	~Window();
};



