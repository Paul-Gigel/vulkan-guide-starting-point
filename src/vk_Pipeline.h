#pragma once
#include "vk_types.h"
#include <vector>
struct PipelineLayout
{	
	PipelineLayout() {
		static int plCounterr;
		plCounter = &plCounterr;
	}
	int *plCounter;
	VkPipelineLayout _PipelineLayout = VK_NULL_HANDLE;
	
};
struct Pipeline 
{
	const char* vertPath;
	const char* fragPath;
	VkPipeline _Pipeline = VK_NULL_HANDLE;
};
class PipelineLayoutBuilder {
public:
	VkPipelineLayout _pipelineLayout;

	VkPipeline buildPipelineLayout(VkDevice, VkPipelineLayoutCreateInfo* const);
};
class PipelineBuilder : public PipelineLayoutBuilder{
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
};