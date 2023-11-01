#pragma once

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<vector>

#include"Utilities.h";



class Mesh
{
public:
	Mesh();
	Mesh(VkPhysicalDevice newPhysicalDevice,VkDevice newDevice,
		VkQueue transferQueue,VkCommandPool transferCommandPool,
		std::vector<Vertex>* vertices, std::vector<uint32_t>* indices);


	int getVertexCount();
	VkBuffer getVertexBuffer();

	int getIndexCount();
	VkBuffer getIndexBuffer();



	void destroyBuffers();

	~Mesh();

private:
	int vertexCount;
	int indexCount;
	 
	VkBuffer indexBuffer;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;



	VkPhysicalDevice physicalDevice;
	VkDevice device;

	void createVertexbuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, 
								std::vector<Vertex>* vertices);

	void createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool,
		std::vector<uint32_t>* indices);



};

