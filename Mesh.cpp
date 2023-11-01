#include "Mesh.h"


Mesh::Mesh()
{
}


Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice,
	VkQueue transferQueue, VkCommandPool transferCommandPool, 
	std::vector<Vertex>* vertices, std::vector<uint32_t>* indices)
{
	vertexCount = vertices->size();
	indexCount = indices->size(); 
	physicalDevice = newPhysicalDevice;
	device = newDevice;
	createVertexbuffer(transferQueue,transferCommandPool, vertices);
	createIndexBuffer(transferQueue, transferCommandPool, indices);

}

int Mesh::getVertexCount()
{
	return vertexCount;
}

VkBuffer Mesh::getVertexBuffer()
{
	return vertexBuffer;
}

int Mesh::getIndexCount()
{
	return indexCount;
}

VkBuffer Mesh::getIndexBuffer()
{
	return indexBuffer;
}




void Mesh::destroyBuffers()
{ 
	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);
}
Mesh::~Mesh()
{
}

void Mesh::createVertexbuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, 
								std::vector<Vertex>* vertices)
{
	//Get the size of the buffer needed for vertices
	VkDeviceSize bufferSize = sizeof(Vertex) * vertices->size();

	//Temporary buffer to "Stage" vertex data before transferring to gpu
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	//create stagingBuffer and allocate memory to it
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory);


	//binding the vertex data to  vertex  buffer
	//Map memory to vertex buffer
	void* data;                                                                  //Create a pointer to a point in normal memory
	vkMapMemory(device, stagingBufferMemory, 0,bufferSize, 0, &data);			 //Map the vertex buffer memory to that pointer in the memory
	memcpy(data, vertices->data(), (size_t)bufferSize);							 //Copying the data from vertices to memory
	vkUnmapMemory(device, stagingBufferMemory);                                   //unmapping the vertex memory


	 //Create buffer with TRANSFER_DST_BIT to mark as recipient of transfer data (also VERTEX_BUFFER)
	//Buffer Memory is to be DEVICE_LOCAL_BIT meaning memory is on the GPU and only accessible by it and not CPU(Host)
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);

	//Copy the data from stagging buffer to Vertex buffer
	copyBuffer(device, transferQueue, transferCommandPool, stagingBuffer, vertexBuffer, bufferSize);



	//clean up of unneccessary buffer and memory
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);


}

void Mesh::createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices)
{
	//Get the size of the buffer needed for vertices
		VkDeviceSize bufferSize = sizeof(uint32_t) * indices->size();

	//Temporary buffer to "Stage" vertex data before transferring to gpu
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	//create stagingBuffer and allocate memory to it
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory);



	//binding the vertex data to  vertex  buffer
	//Map memory to vertex buffer
	void* data;                                                                  
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);			 
	memcpy(data, indices->data(), (size_t)bufferSize);							 
	vkUnmapMemory(device, stagingBufferMemory);

	//create buffer for Index data on gpu access only area
	createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

	//copy data from staging buffer to GPU access buffer
	copyBuffer(device, transferQueue, transferCommandPool, stagingBuffer, indexBuffer, bufferSize);

	//destroy +realease staging buffer resources 
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);


}





