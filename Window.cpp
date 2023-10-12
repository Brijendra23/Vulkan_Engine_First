#include "Window.h"

Window::Window()
{

}
void Window::CreateWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600,"Vulkan_Engine", nullptr, nullptr);

	

}


void Window::CreateWindow(unsigned int width, unsigned int height, const char* name, int resizable)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizable);
	window = glfwCreateWindow(width, height,name, nullptr, nullptr);

}

void Window::windowHint(int hint, int value)
{
	glfwWindowHint(hint, value);
}

void Window::cleanup(GLFWwindow* window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}


Window::~Window()
{
	

}


