#pragma once
#include "vk_types.h"
#include <vector>

/*
	implement some kind of Codesystem
*/
struct PipelineLayout
{
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
};
struct Pipeline
{
	const char* vertPath;
	const char* fragPath;
	VkPipeline _Pipeline = VK_NULL_HANDLE;
};
class PipelineBuilder
{
public:
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	
	VkPipeline buildPipeline(VkDevice, VkRenderPass, Pipeline&, PipelineLayout&);
	VkPipelineLayout buildPipelineLayout(VkDevice, VkPipelineLayoutCreateInfo* const, PipelineLayout&);
};