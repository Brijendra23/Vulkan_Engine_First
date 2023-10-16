#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;
	try{ createInstance(); 
	createSurface();
	getPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	
	
	}
	catch (const std::runtime_error& e)
	{
		printf("ERROR is: %s\n", e.what());
		return EXIT_FAILURE;
	}
	
	return 0;
}





void VulkanRenderer::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}
	//creating the application info
	//not necessary info
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan_Engine";//custom name of the application we making
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);//custom version of the application
	appInfo.pEngineName = "BRICKs";//custom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;//the vulkan version


	//extension creation
	uint32_t glfwExtensionCount = 0;//for getiing the number of extensions required by the glfwExtension
	const char** glfwExtensions;   //extension passed as an array of strings, so need pointer (the array)to pointer (the string)


	


	//getting the glfwExtensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	//create list of extension
	std::vector<const char*> instanceExtension = std::vector<const char*>();


	//filling the list with the names of the extension
	for (size_t i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtension.push_back(glfwExtensions[i]);
	}


	//check instance extension support
	if (!checkInstanceExtensionSupport(&instanceExtension))
	{
		throw std::runtime_error("VKInstance does not support required extension!");
	}

	//creating an instance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtension.size());
	createInfo.ppEnabledExtensionNames = instanceExtension.data();

	//setup validation layer that instance wil use
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	//create instance
	VkResult result=vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vulkan instance!");
	}

	
	
	
}

void VulkanRenderer::getPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	
	//checking devices are zero that supports vulkan
	if (deviceCount == 0)
	{
		throw std::runtime_error("No GPU present that supports vulkan!");
	}

	std::vector<VkPhysicalDevice> deviceList(deviceCount);

	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	//finding best device
	for (const auto& device : deviceList)
	{
		if (checkDeviceSuitable(device))
		{
			mainDevice.physicalDevice = device;
			break;
		}
	}

}

void VulkanRenderer::createLogicalDevice()
{
	//getiing queue family index for creation of the device queue createinfo
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	//creating queue createinfo vector to check if graphics and presentation queue family indices are same or different 
	//using set so that duplicate enteries can be avoided
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicFamily,indices.presentationFamily };




	for (int queueFamilyIndex : queueFamilyIndices)
	{
		//creating queue createinfo for refering to device creation
		VkDeviceQueueCreateInfo queuecreateInfo{};
		queuecreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;  //strucuture
		queuecreateInfo.queueFamilyIndex = queueFamilyIndex;                 //index of the queue
		queuecreateInfo.queueCount = 1;										//number of the queue to be created
		float priority = 1.0f; //pointed for priority can be differnt when queue more then one
		queuecreateInfo.pQueuePriorities = &priority;

		queueCreateInfos.push_back(queuecreateInfo);
	}

	//creation of logical devices 
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());			//number of queues to be created 
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();									 // info about the queue that has to be created
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());        //number of enabled logical device extension
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();                             // list of enabled logical devie extension

	//physical device features  the logical device will be using
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;  //physical device features that logical device will use

	//create the logical device for the given physical device
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);

	//checking the creation of device
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed To create Logical Device for the Given Physical Device!");
	}

	//cQueues are created at the same time as the device
	//so We want handle to queues
	//from given logical device of given queue family from given queue index (0 since only 1 queue created), place the refernece in given vkQueue
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);

}

void VulkanRenderer::createSurface()
{ 
	//create surface(creating a surface create infoo struct, runs the create surface function,returns result
	VkResult result=glfwCreateWindowSurface(instance, window, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface!");
	}

}

void VulkanRenderer::createSwapChain()
{
	//get the swapchain details so we can choose the best settings for the swapchain
	SwapChainDetails swapChainDetails = getSwapChainDetails(mainDevice.physicalDevice);

	//Best Format Settings
	VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);

	//Best Presentation mode settings
	VkPresentModeKHR presentationMode = chooseBestPresentationMode(swapChainDetails.presentationMode);
	//Best surface capabilities
	VkExtent2D extent = chooseSwapExtent(swapChainDetails.surfaceCapabilites);
	//how many images in the swapchange? get 1more than minimum to allow triple buffering
	//if 0 then limitless
	uint32_t imageCount = swapChainDetails.surfaceCapabilites.minImageCount + 1;

	//if imageCount is higher than max, clamp it down to max
	if (swapChainDetails.surfaceCapabilites.maxImageCount>0&&
		swapChainDetails.surfaceCapabilites.maxImageCount < imageCount)
	{
		imageCount = swapChainDetails.surfaceCapabilites.maxImageCount;
	}
	

	// swapchain creatinfo
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.presentMode = presentationMode;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.imageArrayLayers = 1;													//number of layers for each image in chain
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;						//what attachment images will be used as
	swapchainCreateInfo.preTransform = swapChainDetails.surfaceCapabilites.currentTransform;	//transform to perform on the swapchain
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;                     //how to handle blendng images with external graphics
	swapchainCreateInfo.clipped = VK_TRUE;														//whether to clipp the parts of image when behind other window
	

	//get wueue family indices
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	//if graphics and presentation family are different the =n swapchain images must be used shared between families
	if (indices.graphicFamily != indices.presentationFamily)
	{
		uint32_t queueFamilyIndices[] = {
			(uint32_t)indices.graphicFamily,(uint32_t)indices.presentationFamily
		};

		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //image share handle
		swapchainCreateInfo.queueFamilyIndexCount = 2;						//number of queues to share image between
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;		//array of queue that image is to be shared with
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}
	//if old swapchain destroyed and this one replces it, then link old one to quickly hand over the responsibilities
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	//create SwapChain
	VkResult result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapchainCreateInfo, nullptr, &swapChain);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create SwapChain!");
	}
}



//best format is subjective, but ours willbe :
//Format: VK_FORMAT_R8G8B8A8_UNORM(VK_FORMAT_B8G8R8A8_UNORM backup value)
//colorSpace: VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	// if the formats size is 1 and the 0 element is vk_format_undefined says that all the format exist are available to use 
	// so instead of passing all the formats names they pass only one value 1 and that saya all the existing formats are available
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_R8G8B8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	//checking from specific formats for the required best
	for (const auto& format : formats)
	{
		if (format.format == (VK_FORMAT_R8G8B8A8_UNORM || VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}
	return formats[0];
	
}


//Best presentation mode


VkPresentModeKHR VulkanRenderer::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes)
{
	for (const auto& presentationMode : presentationModes)
	{
		if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentationMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	//if current extent is at numeric limits, then extent can vary.\Otherwise ,it is the size of the window.
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		//if value can vary, need to set manually 

		int width, height;
		//get the window size
		glfwGetFramebufferSize(window, &width, &height);


		//create new extent 
		VkExtent2D newExtent = {};
		newExtent.width = width;
		newExtent.height = height;

		//surface also dedines max and min, so make sure within boundaries by clamping value
		newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
		newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));


		return newExtent;




	}
	
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	//Getting the number of layers present in the instance
	uint32_t layerCount=0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//checking for the validation layers in the avaliable layers and validation layer
	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;

}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);//to get the number of extensions to create an arry of correct size
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());


	for (const auto &checkExtension:*checkExtensions)
	{
		bool hasExtension = false;
		for (const auto &extension : extensions)
		{
			if (strcmp(checkExtension, extension.extensionName))
			{
				hasExtension = true;
				break;
			}


		}
		if (!hasExtension)
		{
			return false;
		}
	}
	return true;
	
}

//check for extension for device

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	if (extensionCount == 0)
	{
		return false;
	}

	std::vector<VkExtensionProperties> extensionList(extensionCount);

	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensionList.data());

	for (const auto &extension : deviceExtensions)
	{
		bool hasExtension = false;
		for (const auto &deviceExtension : extensionList)
		{
			if (strcmp(extension, deviceExtension.extensionName))
			{
				hasExtension = true;
				break;

			}
		}
		if (!hasExtension)
		{
			return false;
		}
	}
	return true;



}



bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	//information about the device itself(ID,name,type,vendor,etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//information about things the gpu can do(geo shader,tess shader,wid lines,etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/
	QueueFamilyIndices queueFamily = getQueueFamilies(device);
	bool isExtension = checkDeviceExtensionSupport(device);

	bool swapChainValid = false;
	if (isExtension)
	{
		SwapChainDetails swapchainDetails=getSwapChainDetails(device);
		swapChainValid = !swapchainDetails.presentationMode.empty() && !swapchainDetails.formats.empty();
	}


	return queueFamily.isValid()&&swapChainValid&&isExtension;
	 
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	//get all the family queue properties
	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilyList.data());

	//go through each family and find the required graphic queue
	int i = 0;//since the struct queue family properties has no location indices hence we track using i


	for (const auto& queueFamily : queueFamilyList)
	{
		//First check if queue family has at least 1 queue in that family(could have no queues)
		//queue can be of multiple types defined through bitfiield, need bitwise AND with VK__QUEUE_BIT to check the required type
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicFamily = i;//if queue family valid store its index
		}
		
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
		//checking if presentation type(can be both of the type of graphic family and presentation)
		if (queueFamily.queueCount >= 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}

		//check if graphic family found then stop further checking
		if (indices.isValid())
		{
			break;
		}

		i++;
	}
	return indices;
}
void VulkanRenderer::cleanup()
{
	vkDestroySwapchainKHR(mainDevice.logicalDevice, swapChain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}


SwapChainDetails VulkanRenderer::getSwapChainDetails(VkPhysicalDevice device)
{
	SwapChainDetails swapChainDetails;

	//--Capabilities--
	//geting the surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.surfaceCapabilites);

	//formats
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		swapChainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
	}
	;


	//presentationmode

	uint32_t presentationModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModeCount, nullptr);
	if (presentationModeCount != 0)
	{
		swapChainDetails.presentationMode.resize(presentationModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModeCount, swapChainDetails.presentationMode.data());
		
	}




	return swapChainDetails;
}











VulkanRenderer::~VulkanRenderer()
{
}
