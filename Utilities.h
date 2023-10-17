#pragma once

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










