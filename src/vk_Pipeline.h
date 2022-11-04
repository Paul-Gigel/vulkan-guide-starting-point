#pragma once
#include "vk_types.h"
#include <vector>
struct PipelineLayout :  public PipelineBuilder
{
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
	VkPipelineLayout buildPipelineLayout(VkDevice, VkPipelineLayoutCreateInfo* const) override;
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
	
	VkPipeline buildPipeline(VkDevice, VkRenderPass);
	virtual VkPipelineLayout buildPipelineLayout(VkDevice, VkPipelineLayoutCreateInfo* const) =0; 
};