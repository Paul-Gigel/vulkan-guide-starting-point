#pragma once
#include "vk_types.h"
#include <vector>
struct PipelineLayout
{	
	PipelineLayout() {
		static int plCounterr;
		plCounter = &plCounterr;
	}
	/*int& plCounter() const {
		static int plCounter;
		return plCounter;
	}*/
	int *plCounter;
	VkPipelineLayout _PipelineLayout;
	
};
struct Pipeline 
{
	const char* vertPath;
	const char* fragPath;
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