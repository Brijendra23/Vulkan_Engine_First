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
	createRenderPass();
	createGraphicsPipeline();
	
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

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;

	//get the count of the swap chain images
	uint32_t imageCount2 = 0;
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapChain, &imageCount2, nullptr);

	std::vector<VkImage> images(imageCount2);
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapChain, &imageCount2,images.data());

	for (VkImage image : images)
	{
		//store image handles
		swapChainImage swapchainImage{};
		swapchainImage.image = image;
		//create ImageViews 
		swapchainImage.imageView = createImageView(image, swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		//add to swapchainImage list about the image handles
		swapChainImages.push_back(swapchainImage);

	}

}
VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = image;														//image to createView for
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;                                    //type of the image(1d,2d,3d)
	viewCreateInfo.format = format;                                                     //format of the image data
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;						//allowa to remapping of rgba components to other rgba value;
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;


	//Subresources allow the view to view only a part of an image
	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;							//which aspect of image to view(eg color_bit  for view colour)
	viewCreateInfo.subresourceRange.baseMipLevel = 0;									//start mipmap Level to view from
	viewCreateInfo.subresourceRange.levelCount = 1;										//number of mipmap levels to view
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;									//start array level to view from
	viewCreateInfo.subresourceRange.layerCount = 1;                                     //number of array levels to view

	//create Image View and return it
	VkImageView imageView;
	VkResult result = vkCreateImageView(mainDevice.logicalDevice, &viewCreateInfo, nullptr, &imageView);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create ImageView!");
	}

	return imageView;

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
	vkDestroyPipeline(mainDevice.logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(mainDevice.logicalDevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);
	for (auto image : swapChainImages)
	{
		vkDestroyImageView(mainDevice.logicalDevice, image.imageView, nullptr);
	}

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



void VulkanRenderer::createRenderPass()
{
	//color attachment of render pass
	VkAttachmentDescription colourAttachment{};
	colourAttachment.format = swapchainImageFormat;                        //format to use for attachment
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;                      //number of samples to write for multipsampling
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;                 //Describes what to with attachment before rendering
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;               //Describes what to do with attachment after rendering
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;       //Describes what to do with stencil before rendering
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;     //describes what to do with stencil after rendering


	//framebuffer data will be sorted as an image, but images can be given different data layouts
	//to give optimal use for certain operations
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;         //image data layout before render pass starts
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;     //image data layout after render pass (to change to)


	//attachment refernece uses an attachment index that refers to index in the attachment list passed to renderpasscreateInfo
	VkAttachmentReference colourAttachmentReference{};
	colourAttachmentReference.attachment = 0;
	colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;



	//information about a paritcular subpass the render pass is using
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;        //Pipeline type subpass is to be bound to
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentReference;



	//Need to determine when layout transition occur using subpass dependencies
	std::array<VkSubpassDependency, 2> subpassdependencies;


	//Conversionfrom VK_Image_layout_Undefined to VK_image_LAyout_color_attachment_optimal
	//transition must happen after....
	subpassdependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;                                //subpasses index(VK_SUBPASS_EXTERNAL= special value refering to all the command outside the render pass
	subpassdependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;    //pipeline stage
	subpassdependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;                       //stage access mask
	//transition must happen before...
	subpassdependencies[0].dstSubpass = 0;
	subpassdependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassdependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassdependencies[0].dependencyFlags = 0;
	// Conversionfrom VK_image_LAyout_color_attachment_optimal to VK_image_layout_present_src_khr
	//transition must happen after....
	subpassdependencies[1].srcSubpass = 0;                 
	subpassdependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;  
	subpassdependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//transition must happen before...
	subpassdependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassdependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subpassdependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	subpassdependencies[1].dependencyFlags = 0;


	//renderpass CreateInfo
	VkRenderPassCreateInfo renderpass{};
	renderpass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass.attachmentCount = 1;
	renderpass.pAttachments = &colourAttachment;
	renderpass.subpassCount = 1;
	renderpass.pSubpasses = &subpass;
	renderpass.dependencyCount = static_cast<uint32_t>(subpassdependencies.size());
	renderpass.pDependencies = subpassdependencies.data();

	//creation of renderpass
	VkResult result = vkCreateRenderPass(mainDevice.logicalDevice, &renderpass, nullptr, &renderPass);

	if (result!=VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create RenderPass!");
	}


}










void VulkanRenderer::createGraphicsPipeline()
{
	//read the Spir-v file
	auto vertexShaderCode = readFile("Shaders/vert.spv");
	auto fragmentShaderCode = readFile("Shaders/frag.spv");


	//creating the shader module to link the code with the grpahics pipeline
	VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

	// --SHADRE STAGE--
	//Vertex stage create info for the shader module
	VkPipelineShaderStageCreateInfo stageVertCreateInfo{};
	stageVertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageVertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; //shader stage name
	stageVertCreateInfo.module = vertexShaderModule; //shader module
	stageVertCreateInfo.pName = "main";//entry point into the shader or name of the function in shader code that you want to run

	//fragment stage creation info
	VkPipelineShaderStageCreateInfo stageFragCreateInfo{};
	stageFragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageFragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stageFragCreateInfo.module =fragmentShaderModule;
	stageFragCreateInfo.pName = "main";//name of the function in shader code that you want to run



	//creating the array of the stage createinfo to use in graphics pipeline creation 
	VkPipelineShaderStageCreateInfo shaderStage[] = {
		stageVertCreateInfo,stageFragCreateInfo
	};
	



	//-- VERTEX INPUT  (TODO: putin vertex descriptions when resources created)--
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr; //List of vertex binding description(data spacing/stride information)
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr; //list of vertex atttribute descriptions(data format and where to bind)

	//--Input Assembly-- what is to be done with the input vertices
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;        //Primitve type to assemble vertices as
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;                     //allow overidiing of strip topology to start new primitives

	//--ViewPort and Scissor--
	//viewport
	VkViewport viewport{};
	viewport.x = 0.0f;                                        //x start coordinate
	viewport.y = 0.0f;                                        //y start coordinate
	viewport.width = (float)swapchainExtent.width;            //widht of the viewport
	viewport.height = (float)swapchainExtent.height;          //height of the viewport
	viewport.minDepth = 0.0f;                                 //min framebuffer depth
	viewport.maxDepth = 1.0f;                                 //max framebuffer depth


	// create a scissor info struct
	VkRect2D scissor{};
	scissor.offset = { 0,0 };                    //offset to use region from
	scissor.extent = swapchainExtent;            // extent to describe the area to use


	VkPipelineViewportStateCreateInfo viewportCreateInfo{};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;

	/*
	//--Dynamic State-- 
	//Dynamic state to enable
	std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);   //Dynamic viewport: Can resize iin command buffer with vkcmdSetViewport(commandBuffer,0,1,&viewport)
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR); //Dynamic scissor : Can resize iin command buffer with vkcmdSetScissor(commandBuffer, 0, 1, &Scissor)
	

	//Dynamic state create info
	
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicState.pDynamicStates = dynamicStateEnables.data();
	*/
	
	
	//--RATSERIZER--
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; 
	rasterizerCreateInfo.depthBiasClamp = VK_FALSE;                               //change if fragments beyond near and farplane are clipped
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;                      //whether to discard data and skip rasterizer. never create fragments
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;                      //how to handle filling points btw vertices
	rasterizerCreateInfo.lineWidth = 1.0f;                                        //how thick lines should be when drawn
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;                        //which face of the tri to cullor not render 
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;                     //which is the front face na dhow to determine
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;                              //whether to add depth bias to fragments (good for stopping "shadow acne' in shadow mapping

	
	
	//-- MULTISAMPLING--
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;           //Enable MultiSample shading or not
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;  //NUMBER OF SAMPLES USE PER FRAGMENT


	//--BLENDING--
	//blending decides how new colour being written to a fragment with the old value


	//Blend attachment state (how blending is handled)
	VkPipelineColorBlendAttachmentState colourState{};
	colourState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;          //colour to apply blending to
	colourState.blendEnable = VK_TRUE;                                //enable blending

	// Blending uses equation: (SrcColourBlendFactor*newColor) colourBlendOp (dstColorBlendFactor*oldColor)
	colourState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colourState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colourState.colorBlendOp = VK_BLEND_OP_ADD;

	//summarized: (VK_Blend_factor_src_alpha *newColour)+(Vk_blend_factor_one_minus_src_alpha*oldcolour)
	//               (newcolouralpha*newcolour)+((1-new colour alpha)*oldcolour)

	colourState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colourState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourState.alphaBlendOp = VK_BLEND_OP_ADD;
	//summarized : (1*newalpha) +(0*oldAlpha)=newAlpha

	VkPipelineColorBlendStateCreateInfo colourBlendCreateInfo{};
	colourBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colourBlendCreateInfo.logicOpEnable = VK_FALSE;
	colourBlendCreateInfo.attachmentCount = 1;
	colourBlendCreateInfo.pAttachments = &colourState;


	//--Pipeline Layout(Todo: apply future desccriptor set layouts)--
	VkPipelineLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutCreateInfo.setLayoutCount = 0;
	layoutCreateInfo.pSetLayouts = nullptr;
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = nullptr;

	//creating pipeline layout
	VkResult result = vkCreatePipelineLayout(mainDevice.logicalDevice, &layoutCreateInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}


	//--depth Stencil testing--
	//TODO: set up depth stncil testing
	 


	//--Graphics Pipeline Creation--
	VkGraphicsPipelineCreateInfo graphicspipelineCreateinfo{};
	graphicspipelineCreateinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicspipelineCreateinfo.stageCount = 2;                                  //number of shader stages
	graphicspipelineCreateinfo.pStages = shaderStage;
	graphicspipelineCreateinfo.pVertexInputState = &vertexInputCreateInfo;
	graphicspipelineCreateinfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	graphicspipelineCreateinfo.pViewportState = &viewportCreateInfo;
	graphicspipelineCreateinfo.pDynamicState = nullptr;
	graphicspipelineCreateinfo.pRasterizationState = &rasterizerCreateInfo;
	graphicspipelineCreateinfo.pMultisampleState = &multisampleCreateInfo;
	graphicspipelineCreateinfo.pColorBlendState = &colourBlendCreateInfo;
	graphicspipelineCreateinfo.pDepthStencilState = nullptr;
	graphicspipelineCreateinfo.layout = pipelineLayout;                           //pipeline layout pipeline should use
	graphicspipelineCreateinfo.renderPass = renderPass;                           //renderpass description the pipeline is compatible with
	graphicspipelineCreateinfo.subpass = 0;                                       //subpass to rneder pass to use with pipeline

	//pipeline derivatives::can create multiple pipeline from one another for optimization
	graphicspipelineCreateinfo.basePipelineHandle = VK_NULL_HANDLE;  //Existing pipeline to derive from
	graphicspipelineCreateinfo.basePipelineIndex = -1;              //index of pipeline

	//create grpahics pipeline
	result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &graphicspipelineCreateinfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphicsPipeline!");
	}


	//destroying the shader module 
	vkDestroyShaderModule(mainDevice.logicalDevice, fragmentShaderModule, nullptr);
	vkDestroyShaderModule(mainDevice.logicalDevice, vertexShaderModule, nullptr);

}




VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& code)
{
	//shader modulecreate info
	VkShaderModuleCreateInfo shaderModuleCreateInfo{};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = code.size();
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());//casting the pointer from char type to uint32_t type since its a pointer ccasting we use reinterpret cast
	
	//creating a shader module and returning the handle
	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(mainDevice.logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Shader Module!");
	}

	return shaderModule;
}


VulkanRenderer::~VulkanRenderer()
{
}
