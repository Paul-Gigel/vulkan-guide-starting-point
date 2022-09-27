#pragma once
#include "vk_types.h"
#include <vector>
struct Pipeline
{
	const char* vertPath = "../shaders/coloredTriangle.vert.spv";
	const char* fragPath = "../shaders/coloredTriangle.frag.spv";
	VkPipelineLayout _PipelineLayout;
	VkPipeline _Pipeline;
};
class PipelineBuilder {
public:
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;

	VkPipeline buildPipeline(VkDevice, VkRenderPass);
};