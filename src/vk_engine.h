#pragma once

#include <vk_types.h>
#include <vk_Pipeline.h>
#include <vector>
class VulkanEngine {
private:
	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debugMessanger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;

	VkSurfaceKHR _surface;
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	VkRenderPass _renderPass;
	std::vector<VkFramebuffer> _frameBuffers;

	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;

	Pipeline _pip;
	/*VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;*/
public:
	bool _isInitialized{ false };
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };
public:
	//initializes everything in the engine
	void init();

	void initVulkan();

	void initSwapchain();

	void initCommands();

	void initDefaultRenderPass();

	void initFrameBuffers();

	void initSyncStructures();

	void initPipelines();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();


	bool loadShaderModule(const char*, VkShaderModule*);
};
