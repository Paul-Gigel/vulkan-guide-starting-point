#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <Macros.h>

struct AllocatedBuffer {
	VkBuffer _buffer;
	VmaAllocation _allocation;
};