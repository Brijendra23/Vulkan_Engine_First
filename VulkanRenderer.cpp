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


	//creating queue createinfo for refering to device creation
	VkDeviceQueueCreateInfo queuecreateInfo{};
	queuecreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queuecreateInfo.queueFamilyIndex = indices.graphicFamily;
	queuecreateInfo.queueCount = 1;
	float priority = 1.0f; //pointed for priority can be differnt when queue more then one
	queuecreateInfo.pQueuePriorities = &priority;


	//creation of logical devices 
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;  //number of queues to be created 
	deviceCreateInfo.pQueueCreateInfos = &queuecreateInfo;// info about the queue that has to be created
	deviceCreateInfo.enabledExtensionCount = 0;   //number of enabled logical device extension
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;// list of enabled logical devie extension

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

	return queueFamily.isValid();
	 
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
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}

VulkanRenderer::~VulkanRenderer()
{
}
