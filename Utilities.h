#pragma once

// Indices(Location ) of the Queue Families (if they exist at all)

struct QueueFamilyIndices {
	int graphicFamily = -1;// location of the graphics queue family


	//checking if queue families are valid
	bool isValid()
	{
		return graphicFamily >= 0;

	}

};
