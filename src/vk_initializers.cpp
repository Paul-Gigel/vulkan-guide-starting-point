#include <vk_initializers.h>
namespace vkinit {
	VkCommandPoolCreateInfo commandPoolCreateInfo(
		uint32_t queFamilyIndex,
		VkCommandPoolCreateFlags flags) 
	{
		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.queueFamilyIndex = queFamilyIndex;
		commandPoolInfo.flags = flags;
		return commandPoolInfo;
		};
	VkCommandBufferAllocateInfo commandBufferAllocInfo(
		VkCommandPool pool,
		uint32_t count,
		VkCommandBufferLevel level) 
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.commandPool = pool;
		allocInfo.commandBufferCount = count;
		allocInfo.level = level;
		return allocInfo;
	};
}