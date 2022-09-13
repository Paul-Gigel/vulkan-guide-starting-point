
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
		Vk_Result err = x;\
		if (err)\
		{\
			std::cout << "detected Vulkan error:	" << err << "\n";\
			abort;\
		}\
	} while (0);
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

}
void VulkanEngine::cleanup()
{	
	if (_isInitialized) {
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