#pragma once
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include<stdexcept>
#include<vector>
#include"Utilities.h"
#include<set>
#include<algorithm>
#include<array>

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
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;


	std::vector<swapChainImage> swapChainImages;



	//Utilities
	//-Main

	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;

	void cleanup();



	~VulkanRenderer();


private:
	GLFWwindow* window;

	//vulkan components
	VkInstance instance;

	//Graphics Pipeline
	VkPipeline graphicsPipeline;

	//Pipeline
	VkPipelineLayout pipelineLayout;
	//renderpass
	VkRenderPass renderPass;

	//vulkan functions 
	//Create Functions
	void createInstance();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createRenderPass();
	void createGraphicsPipeline();
	

	//get phhysical device function
	void getPhysicalDevice();

	


	//Vulkan support function
	//--checker function
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);//checking whther the extensions required are valid or not
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool checkDeviceSuitable(VkPhysicalDevice device);
	bool checkValidationLayerSupport();

	//--getter function
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

	SwapChainDetails  getSwapChainDetails(VkPhysicalDevice device);

	//-- choose functions
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

	//--createFunction

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
};

