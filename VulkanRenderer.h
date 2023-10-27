#pragma once
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include<stdexcept>
#include<vector>
#include"Mesh.h"
#include"Utilities.h"
#include<set>
#include<algorithm>
#include<array>

class VulkanRenderer
{
public:
	VulkanRenderer();

	//SCene Objects
	Mesh firstObject;


	int init(GLFWwindow* newWindow);
	void draw();

	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;


	std::vector<swapChainImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	//pools
	VkCommandPool graphicsCommandPool;

	//Utilities
	//-Main
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;



	//_synchronization
	std::vector<VkSemaphore> imageAvailable;
	std::vector<VkSemaphore> renderFinished;
	std::vector<VkFence> drawFences;



	void cleanup();



	~VulkanRenderer();


private:
	
	GLFWwindow* window;

	int currentFrame = 0;


	//vulkan components
	VkInstance instance;

	
	//Pipeline
	VkPipelineLayout pipelineLayout;
	//renderpass
	VkRenderPass renderPass;
	//Graphics Pipeline
	VkPipeline graphicsPipeline;
	
	
	//vulkan functions 
	//Create Functions
	void createInstance();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFrameBuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronization();
	  

	//-Record Function
	void recordCommands();
	

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

