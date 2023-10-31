#pragma once
#define GLFW_INCLUDE_VULKAN
#include<glfw3.h>
#include<fstream>
#include<glm/glm.hpp>


const int MAX_FRAME_DRAWS = 2;


const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


//Vertex Data Representation
struct Vertex {
	glm::vec3 pos;//Vertex Position (x,y,z)
	glm::vec3 col; //Color value(r,g,b)
};






//validation layers to be used or enabled

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



// Indices(Location ) of the Queue Families (if they exist at all)

struct QueueFamilyIndices {
	int graphicFamily = -1;// location of the graphics queue family
	int presentationFamily = -1;//location of the presentation queue
	//checking if queue families are valid
	bool isValid()
	{
		return graphicFamily >= 0&& presentationFamily>=0;

	}

};

struct SwapChainDetails {
	VkSurfaceCapabilitiesKHR surfaceCapabilites;  //surface properties like(image size/extent)
	std::vector<VkSurfaceFormatKHR> formats;      //surface image foormat egg rbga and size of each color
	std::vector<VkPresentModeKHR> presentationMode; //presentatin mode on how the images are prensented in the surface

};

struct swapChainImage {
	VkImage image;
	VkImageView imageView;
};



//reading the sprv file for the shaders

static std::vector<char> readFile(const std::string& filename)
{
	//open stream from given file
	//std::ios::binary tells stream toread filw as binary
	//std::ios::ate  start reading from en of file and points the end of the file telling about the size of the file
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	//chech if filestream sucessfully or not
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open a shaders file!");
	}

	size_t fileSize = (size_t)file.tellg();// tells the position of the current position of the pointer
	// file buffer created using the size
	std::vector<char> fileBuffer(fileSize);

	//move read postion to start of the file
	file.seekg(0);

	//read the file data from the start to very end
	file.read(fileBuffer.data(), fileSize);

	//close the stream
	file.close();

	return fileBuffer;
}

static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice,uint32_t allowedTypes, VkMemoryPropertyFlags properties)
{
	//Get PhysicalDevice Properties
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((allowedTypes & (1 << i))                                                          //Index of memory type must match the corresponding bit in allowed types
			&& (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)     //Desired property bit flags are part of memory types property flags
		{
			//return the index of the memory
			return i;
		}
	}

}


static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsageFlags,
	VkMemoryPropertyFlags bufferProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	//Information to create a buffer (doesnt include assigning memory)
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSize;                                           //size of the buffer(size of a vertex* number of vertices)
	bufferCreateInfo.usage = bufferUsageFlags;                                   //MULTIPLE TYPes of buffer are possible to be created 
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;                     //similar to swapchain images, can share vertex buffers


	VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vertex Buffer!");
	}


	//Get buffer memory requirements
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);


	//Allocate memory to buffer
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(physicalDevice,memRequirements.memoryTypeBits,             //index of memory type on physical device that has require bit flags
		bufferProperties);																								 //VK_Memory_property_host_visible_bit:Cpu can interact with mem
																														//VK_MEMORY_PROPERTY_host_coherent_bit:allows placement of data straight into buffer after mapping(otherwise would have to specify amnually)
																														//allocate memory to VKDEVICEMEMORY
	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, bufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	//Allocate the memory to given vertex buffer
	vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}


static void copyBuffer(VkDevice device,
	VkQueue transferQueue, VkCommandPool transferCommandPool,
	VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
{
	//create Command buffer to hold for transfer commands
	VkCommandBuffer transferCommandBuffer;

	//Command buffer deatails
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = transferCommandPool;
	allocInfo.commandBufferCount = 1;


	//Allocate command buffer from pool
	vkAllocateCommandBuffers(device, &allocInfo, &transferCommandBuffer);

	//information to begin recording command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;  //WE ARE using the command buffer only once, so set up for one time only

	//Begin recording traansfer commands
	vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);

	//Region to copy data from and to
	VkBufferCopy bufferCopyRegion{};
	bufferCopyRegion.srcOffset = 0;
	bufferCopyRegion.dstOffset = 0;
	bufferCopyRegion.size = bufferSize;

	//Command to copy data from src to dst buffer
	vkCmdCopyBuffer(transferCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyRegion);


	vkEndCommandBuffer(transferCommandBuffer);

	//Queue Subsmission information
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &transferCommandBuffer;

	//submit transfer command to transfer queue and wait until it finishes
	vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(transferQueue);




	//free temporary command buffer back to pool
	vkFreeCommandBuffers(device, transferCommandPool, 1, &transferCommandBuffer);
}












