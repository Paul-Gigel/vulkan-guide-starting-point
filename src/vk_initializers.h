#pragma once

#include <vk_types.h>

namespace vkinit {
	VkCommandPoolCreateInfo commandPoolCreateInfo(
		uint32_t queFamilyIndex,
		VkCommandPoolCreateFlags flags = 0
		);
	VkCommandBufferAllocateInfo commandBufferAllocInfo(
		VkCommandPool pool,
		uint32_t count,
		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
		);
}

