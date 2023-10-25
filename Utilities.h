#pragma once

#include<fstream>


const int MAX_FRAME_DRAWS = 2;


const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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










