#include "Mesh.h"


Mesh::Mesh()
{
}


Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices)
{
	vertexCount = vertices->size();
	physicalDevice = newPhysicalDevice;
	device = newDevice;
	createVertexbuffer(vertices);

}

int Mesh::getVertexCount()
{
	return vertexCount;
}

VkBuffer Mesh::getVertexBuffer()
{
	return vertexBuffer;
}




void Mesh::destroyVertexBuffer()
{ 
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
}
Mesh::~Mesh()
{
}

void Mesh::createVertexbuffer(std::vector<Vertex>* vertices)
{
	//Information to create a buffer (doesnt include assigning memory)
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = sizeof(Vertex) * vertices->size();                    //size of the buffer(size of a vertex* number of vertices)
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;                   //MULTIPLE TYPes of buffer are possible to be created but e=we are going to use the vertex buffer
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;                     //similar to swapchain images, can share vertex buffers


	VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &vertexBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Vertex Buffer!");
	}


	//Get buffer memory requirements
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);


	//Allocate memory to buffer
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,             //index of memory type on physical device that has require bit flags
		                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); //VK_Memory_property_host_visible_bit:Cpu can interact with mem
																										//VK_MEMORY_PROPERTY_host_coherent_bit:allows placement of data straight into buffer after mapping(otherwise would have to specify amnually)
	//allocate memory to VKDEVICEMEMORY
	result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &vertexBufferMemory);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	//Allocate the memory to given vertex buffer
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);


	//binding the vertex data to  vertex  buffer
	//Map memory to vertex buffer
	void* data;                                              //Create a pointer to a point in normal memory
	vkMapMemory(device, vertexBufferMemory, 0, bufferCreateInfo.size, 0, &data);//Map the vertex buffer memory to that pointer in the memory
	memcpy(data, vertices->data(), (size_t)bufferCreateInfo.size);               //Copying the data from vertices to memory
	vkUnmapMemory(device, vertexBufferMemory);                                   //unmapping the vertex memoory


}

uint32_t Mesh::findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties)
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



