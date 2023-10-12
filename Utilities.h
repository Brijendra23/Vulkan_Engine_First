#pragma once
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
