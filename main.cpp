#define GLFW_INCLUDE_VULKAN

#include<GLFW/glfw3.h>
#include<stdexcept>
#include<vector>
#include<iostream>

#include"Window.h"
#include"VulkanRenderer.h"

Window window;
VulkanRenderer vulkanRenderer;
int main()
{
	
	//creating a window
	window.CreateWindow(800, 600, "Vulkan_engine",GLFW_FALSE);

	//creating an instance
	if (vulkanRenderer.init(window.window) == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}





	while (!glfwWindowShouldClose(window.window))
	{
		glfwPollEvents();
		vulkanRenderer.draw();

	}

	vulkanRenderer.cleanup();
	window.cleanup(window.window);
	
	return 0;
}