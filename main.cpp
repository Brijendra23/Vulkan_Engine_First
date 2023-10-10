#define GLFW_INCLUDE_VULKAN

#include<GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


#include<glm/glm.hpp>
#include<glm/mat4x4.hpp>

#include<iostream>

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "MY_ENGINE", nullptr, nullptr);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::cout << "Extension Property number:" << extensionCount << std::endl;

	glm::mat4 testMAtrix(1.0f);
	glm::vec4 testVector(1.0f);

	auto testResult = testMAtrix * testVector;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}