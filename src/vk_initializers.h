#pragma once

#include <vk_types.h>
#include <Macros.h>
namespace vkinit {
	VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queFamilyIndex, VkCommandPoolCreateFlags flags = 0);
	VkCommandBufferAllocateInfo commandBufferAllocInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits, VkShaderModule);
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo();
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo(VkPrimitiveTopology);
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(VkPolygonMode polygonMode);
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo();
	VkPipelineColorBlendAttachmentState colorBlendAttachment();
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(VkPipelineLayoutCreateFlags flags =0,
		uint32_t setLayoutCount =0,
		const VkDescriptorSetLayout* pSetLayouts =nullptr,
		uint32_t pushConstantRangeCount =0,
		const VkPushConstantRange* pPushConstantRanges = nullptr);
	VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags);
	VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags);

}

