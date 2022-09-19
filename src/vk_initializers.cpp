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
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(
		VkShaderStageFlagBits stage,
		VkShaderModule shaderModule
		) 
	{
		VkPipelineShaderStageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.pNext = nullptr;
		info.stage = stage;
		info.module = shaderModule;
		info.pName = "main";
		return info;
	};
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo()
	{
		VkPipelineVertexInputStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		info.pNext = nullptr;
		info.vertexBindingDescriptionCount = 0;

		info.pVertexBindingDescriptions = nullptr;
		info.vertexAttributeDescriptionCount = 0;
		info.pVertexAttributeDescriptions = nullptr;
		return info;
	}
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo(
		VkPrimitiveTopology topology
		)
	{
		VkPipelineInputAssemblyStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.pNext = nullptr;
		info.topology = topology;
		info.primitiveRestartEnable = VK_FALSE;
		return info;
	}
}