#include "vkwp/pipeline.hpp"

vkwp::Pipeline::Pipeline() = default;

vkwp::Pipeline::Pipeline(const std::string & label)
    : Resource{ label }
{}
