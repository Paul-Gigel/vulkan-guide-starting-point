
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <vk_types.h>
#include <vk_initializers.h>

#include "VkBootstrap.h"

using namespace std;
#define	VK_CHECK(x)\
	do\
	{\
		VkResult err = x;\
		if (err)\
		{\
			std::cout << "detected Vulkan error:	" << err << "\n";\
			abort;\
		}\
	} while (0)
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
	//everything went fine
	_isInitialized = true;
}
void VulkanEngine::initVulkan() {
	vkb::InstanceBuilder builder;
	auto instRet = builder
		.set_app_name("learning Vulkan")
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
}
void VulkanEngine::initCommands() {
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::commandPoolCreateInfo(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

	VkCommandBufferAllocateInfo allocInfo = vkinit::commandBufferAllocInfo(_commandPool, 1);
	VK_CHECK(vkAllocateCommandBuffers(_device, &allocInfo, &_mainCommandBuffer));
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
}
void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
		vkDestroyCommandPool(_device, _commandPool, nullptr);
		vkDestroySwapchainKHR(_device, _swapchain, nullptr);
		for (auto swapchainImageView : _swapchainImageViews)
		{
			vkDestroyImageView(_device, swapchainImageView, nullptr);
		}
		vkDestroyDevice(_device, nullptr);
		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkb::destroy_debug_utils_messenger(_instance, _debugMessanger, nullptr);
		vkDestroyInstance(_instance, nullptr);   
		SDL_DestroyWindow(_window);
		std::cout << "cleaned up\n";
	}
}
void VulkanEngine::draw()
{
	//nothing yet
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
		}

		draw();
	}
}
