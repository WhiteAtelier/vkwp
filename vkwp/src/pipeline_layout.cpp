#include "vkwp/pipeline_layout.hpp"

vkwp::PipelineLayout::PipelineLayout() = default;

vkwp::PipelineLayout::PipelineLayout(const std::string & label)
    : Resource{ label }
{}
