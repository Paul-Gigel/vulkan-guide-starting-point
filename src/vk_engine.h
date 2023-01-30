#pragma once

#include <vk_types.h>
#include <vk_Pipeline.h>
#include <vector>
#include <queue>
#include <tuple>
#include <functional>
struct DeletionQueue {
	std::deque<std::function<void()>> deletors;
	void pushFunktion(std::function<void()>&& funktion) {
		deletors.push_back(funktion);
	}
	void flush() {
		for (auto it = deletors.rbegin(); it != deletors.rend();it++)
		{
			(*it)();
		}
		deletors.clear();
	}
};
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

	std::vector<PipelineLayout> _PipelineLayouts;
	std::vector<Pipeline> _pips;

	VmaAllocator _allocator;

	DeletionQueue _mainDelQueue;
public:
	bool _isInitialized{ false };
	int _selectPipe =0;
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };
public:
	VulkanEngine();
	//initializes everything in the engine
	void init();

	void initVulkan();

	void initSwapchain();
	 
	void initCommands();

	void initDefaultRenderPass();

	void initFrameBuffers();

	void initSyncStructures();

	void initPipelineLayouts(PipelineLayout* lay);

	void initPipelines(Pipeline*, PipelineLayout*);

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();


	bool loadShaderModule(const char*, VkShaderModule*);
};
