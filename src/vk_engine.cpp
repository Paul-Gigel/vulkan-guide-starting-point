#define VMA_IMPLEMENTATION
#include "vk_engine.h"
#include <SDL.h>
#include <SDL_vulkan.h>
#include <iostream>
#include <fstream>
#include <vk_types.h>
#include <Macros.h>
#include <vk_initializers.h>
#include <vk_Pipeline.h>
#include <vk_mem_alloc.h>
#include "VkBootstrap.h"


using namespace std;
VulkanEngine::VulkanEngine() {
	Pipeline pip1;
	pip1.vertPath = "../shaders/coloredTriangle.vert.spv";
	pip1.fragPath = "../shaders/coloredTriangle.frag.spv";
	_pips.push_back(pip1);
	pip1.vertPath = "../shaders/triangle.vert.spv";
	pip1.fragPath = "../shaders/triangle.frag.spv";
	_pips.push_back(pip1);
	pip1.vertPath = "../shaders/meshTriangle.vert.spv";
	pip1.fragPath = "../shaders/coloredTriangle.frag.spv";
	_pips.push_back(pip1);
}
void VulkanEngine::init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	
	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags
	);
	//init
	initVulkan();
	initSwapchain();
	initCommands();
	initDefaultRenderPass();
	initFrameBuffers();
	initSyncStructures();
	_PipelineLayouts.push_back({});
	initPipelineLayouts(&_PipelineLayouts[0]);
	for (Pipeline& pip : _pips)
	{
		initPipelines(&pip, &_PipelineLayouts[0]);
	}
	load_meshes();
	//everything went fine
	_isInitialized = true;
}
void VulkanEngine::initVulkan() {
	vkb::InstanceBuilder builder;
	auto instRet = builder
		.set_app_name("learning Vulkan")		/*some where here [ERROR: General] loader_get_json: Failed to open JSON file C:\Program Files (x86)\Epic Games\Launcher\Portal\Extras\Overlay\EOSOverlayVkLayer-Win32.json*/
		.request_validation_layers(true)		
		.require_api_version(1, 1, 0)			
		.use_default_debug_messenger()			
		.build();
	vkb::Instance vkbInst = instRet.value();
	_instance = vkbInst.instance;
	_debugMessanger = vkbInst.debug_messenger;
	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);
	vkb::PhysicalDeviceSelector selector(vkbInst);
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(1, 1)
		.set_surface(_surface)
		.select()
		.value();
	vkb::DeviceBuilder deviceBuilder(physicalDevice);
	vkb::Device vkbDevice = deviceBuilder
		.build()
		.value();
	_chosenGPU = physicalDevice.physical_device;
	_device = vkbDevice.device;
	_graphicsQueue = vkbDevice
		.get_queue(vkb::QueueType::graphics)
		.value();
	_graphicsQueueFamily = vkbDevice
		.get_queue_index(vkb::QueueType::graphics)
		.value();

	VmaAllocatorCreateInfo allocInfo{};
	allocInfo.physicalDevice = _chosenGPU;
	allocInfo.device = _device;
	allocInfo.instance = _instance;
	VK_CHECK(vmaCreateAllocator(&allocInfo, &_allocator));
}
void VulkanEngine::initSwapchain() {
	vkb::SwapchainBuilder swapchainBuilder(_chosenGPU, _device, _surface);
	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(_windowExtent.width, _windowExtent.height)
		.build()
		.value();
	//store swapchainstuff
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImageFormat = vkbSwapchain.image_format;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();

	_mainDelQueue.pushFunktion([=]() {
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
	});
}
void VulkanEngine::initCommands() {
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::commandPoolCreateInfo(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

	VkCommandBufferAllocateInfo allocInfo = vkinit::commandBufferAllocInfo(_commandPool, 1);
	VK_CHECK(vkAllocateCommandBuffers(_device, &allocInfo, &_mainCommandBuffer));

	_mainDelQueue.pushFunktion([=]() {
		vkDestroyCommandPool(_device, _commandPool, nullptr);
	});
}
void VulkanEngine::initDefaultRenderPass() {
	VkAttachmentDescription color_attachment = {};						// the renderpass will use this color attachment.
	color_attachment.format = _swapchainImageFormat;					//the attachment will have the format needed by the swapchain
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;					//1 sample, we won't be doing MSAA
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;				// we Clear when this attachment is loaded
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;			// we keep the attachment stored when the renderpass ends
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;	//we don't care about stencil
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;	//we don't care about stencil
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;			//we don't know or care about the starting layout of the attachment
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;		//after the renderpass ends, the image has to be on a layout ready for display
	
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &color_attachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VK_CHECK(vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass));

	_mainDelQueue.pushFunktion([=]() {
		vkDestroyRenderPass(_device, _renderPass, nullptr);
	});
}
void VulkanEngine::initFrameBuffers() {
	VkFramebufferCreateInfo fpInfo{};
	fpInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fpInfo.pNext = nullptr;
	fpInfo.renderPass = _renderPass;
	fpInfo.attachmentCount = 1;
	fpInfo.width = _windowExtent.width;
	fpInfo.height = _windowExtent.height;
	fpInfo.layers = 1;
	const uint32_t swapchainImageCount = _swapchainImages.size();
	_frameBuffers = std::vector<VkFramebuffer>(swapchainImageCount);
	for (size_t i = 0; i < swapchainImageCount; i++)
	{
		fpInfo.pAttachments = &_swapchainImageViews[i];
		VK_CHECK(vkCreateFramebuffer(_device, &fpInfo, nullptr, &_frameBuffers[i]));
		_mainDelQueue.pushFunktion([=]() {
			vkDestroyFramebuffer(_device, _frameBuffers[i], nullptr);
			vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
		});
	}
};
void VulkanEngine::initSyncStructures() {
	VkFenceCreateInfo fenceCreateInfo = vkinit::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));
	_mainDelQueue.pushFunktion([=]() {
		vkDestroyFence(_device, _renderFence, nullptr);
	});

	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphoreCreateInfo(0);
	VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore));
	VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore));
	_mainDelQueue.pushFunktion([=]() {
		vkDestroySemaphore(_device, _renderSemaphore, nullptr);
		vkDestroySemaphore(_device, _presentSemaphore, nullptr);
	});
}
void VulkanEngine::initPipelineLayouts(PipelineLayout* const lay) {
	if ((lay->_pipelineLayout) == VK_NULL_HANDLE)	// default layout, if nothing
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::pipelineLayoutCreateInfo();
		PipelineBuilder pipelineBuilder;
		pipelineBuilder.buildPipelineLayout(_device, &pipelineLayoutInfo, *lay);
		_mainDelQueue.pushFunktion([=]() {
			vkDestroyPipelineLayout(_device, lay->_pipelineLayout, nullptr);
		});
	}
};
void VulkanEngine::initPipelines(Pipeline* pip, PipelineLayout* lay) {
	VkShaderModule fragShader;
	if (!loadShaderModule(pip->fragPath, &fragShader))
	{
		std::cout << "Error when building the triangle fragment shader module" << std::endl;
	}
	else
	{
		std::cout << "Triangle fragment shader successfully loaded" << std::endl;
	}
	VkShaderModule vertShader;
	if (!loadShaderModule(pip->vertPath, &vertShader))
	{
		std::cout << "Error when building the triangle vertex shader module" << std::endl;

	}
	else {
		std::cout << "Triangle vertex shader successfully loaded" << std::endl;
	}

	PipelineBuilder pipelineBuilder;
	//?!?!?!?!?!?!
	pipelineBuilder._vertexInputInfo = vkinit::vertexInputStateCreateInfo();
	pipelineBuilder._inputAssembly = vkinit::inputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder._viewport.x = 0.0f;
	pipelineBuilder._viewport.y = 0.0f;
	pipelineBuilder._viewport.width = (float)_windowExtent.width;
	pipelineBuilder._viewport.height = (float)_windowExtent.height;
	pipelineBuilder._viewport.minDepth = 0.0f;
	pipelineBuilder._viewport.maxDepth = 1.0f;
	pipelineBuilder._scissor.offset = { 0, 0 };
	pipelineBuilder._scissor.extent = _windowExtent;
	pipelineBuilder._rasterizer = vkinit::rasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
	pipelineBuilder._multisampling = vkinit::multisampleStateCreateInfo();
	pipelineBuilder._colorBlendAttachment = vkinit::colorBlendAttachment();
	
	VertexInputDescription vertexDescription = Vertex::getVertexDescription();
	pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
	pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
	pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
	pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();

	pipelineBuilder._shaderStages.clear();

	pipelineBuilder._shaderStages.push_back(
		vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader));
	pipelineBuilder._shaderStages.push_back(
		vkinit::pipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader));

	pip->_Pipeline = pipelineBuilder.buildPipeline(_device, _renderPass, *pip, *lay);
	
	vkDestroyShaderModule(_device, vertShader, nullptr);
	vkDestroyShaderModule(_device, fragShader, nullptr);
	_mainDelQueue.pushFunktion([=]() {
		vkDestroyPipeline(_device, pip->_Pipeline, nullptr);
	});
/*#undef pipelineBuilder*/
} 
void VulkanEngine::load_meshes() {
	_triangleMesh._vertices.resize(3);

	_triangleMesh._vertices[0].position = { 1.f, 1.f , 0.0f };
	_triangleMesh._vertices[1].position = {-1.f, 1.f , 0.0f };
	_triangleMesh._vertices[2].position = { 0.f,-1.f , 0.0f };

	_triangleMesh._vertices[0].color = { 0.f, 1.f, 0.0f };
	_triangleMesh._vertices[1].color = { 0.f, 1.f, 0.0f };
	_triangleMesh._vertices[2].color = { 0.f, 1.f, 0.0f };

	upload_meshes(_triangleMesh);
}
void VulkanEngine::upload_meshes(Mesh& mesh) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = mesh._vertices.size() * sizeof(Vertex);
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo,
		&mesh._vertexBuffer._buffer,
		&mesh._vertexBuffer._allocation,
		nullptr));
	
	_mainDelQueue.pushFunktion([=]() {
		vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
		});

	void* data;
	vmaMapMemory(_allocator, mesh._vertexBuffer._allocation, &data);
	memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));
	vmaUnmapMemory(_allocator, mesh._vertexBuffer._allocation);
}
void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		vkDeviceWaitIdle(_device);
		_mainDelQueue.flush();
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debugMessanger, nullptr);
		vkDestroyInstance(_instance, nullptr);   
		SDL_DestroyWindow(_window);
		std::cout << "cleaned up\n";
	}
} 
void VulkanEngine::draw()
{
	VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000/*UINT64_MAX*/));
	VK_CHECK(vkResetFences(_device, 1, &_renderFence));
	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex));
	VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer, 0));

	VkCommandBuffer cmd = _mainCommandBuffer;
	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = nullptr;
	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	VkClearValue clearValue;
	float flash = abs(sin(_frameNumber / 120.f));
	clearValue.color = { {flash, 0.0f, flash, 1.0f} };

	VkRenderPassBeginInfo rpBeginInfo{};
	rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpBeginInfo.pNext = nullptr;
	rpBeginInfo.renderPass = _renderPass;
	rpBeginInfo.renderArea.offset = { 0, 0 };
	rpBeginInfo.renderArea.extent = _windowExtent;
	rpBeginInfo.framebuffer = _frameBuffers[swapchainImageIndex];
	rpBeginInfo.clearValueCount = 1;
	rpBeginInfo.pClearValues = &clearValue;
	vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _pips[_selectPipe]._Pipeline);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &_triangleMesh._vertexBuffer._buffer, &offset);
	vkCmdDraw(cmd, _triangleMesh._vertices.size(), 1, 0, 0);

	vkCmdEndRenderPass(cmd);
	VK_CHECK(vkEndCommandBuffer(cmd));

	VkSubmitInfo submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitStage;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &_presentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &_renderSemaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderSemaphore;
	presentInfo.pImageIndices = &swapchainImageIndex;
	VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	_frameNumber++;
}
void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//close the window when user alt-f4s or clicks the X button			
			if (e.type == SDL_QUIT) bQuit = true;
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_SPACE)
				{
					_selectPipe++;
					_selectPipe = _selectPipe % _pips.size();
				}
			}
		}
		/*_selectPipe++;
		_selectPipe = _selectPipe % _pip.size();*/
		draw();
	}
}


bool VulkanEngine::loadShaderModule(const char* filePath, VkShaderModule* outShaderModule) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	if (!file.is_open())	return false;
	size_t fileSize = (size_t)file.tellg();
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
	file.seekg(0);
	file.read((char*)buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		return false;
	}
	*outShaderModule = shaderModule;
	return true;
}