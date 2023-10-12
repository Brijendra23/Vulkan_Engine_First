#pragma once
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include<stdexcept>
#include<vector>
#include"Utilities.h"


class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow* newWindow);

	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;

	void cleanup();



	~VulkanRenderer();


private:
	GLFWwindow* window;

	//vulkan components
	VkInstance instance;


	//vulkan functions 
	//Create Functions
	void createInstance();
	void createLogicalDevice();
	void createSurface();

	//get phhysical device function
	void getPhysicalDevice();


	//Vulkan support function
	//--checker function
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);//checking whther the extensions required are valid or not
	bool checkDeviceSuitable(VkPhysicalDevice device);
	bool checkValidationLayerSupport();

	//--getter function
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);


	
};

