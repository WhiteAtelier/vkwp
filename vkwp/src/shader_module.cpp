#include "vkwp/shader_module.hpp"

vkwp::ShaderModule::ShaderModule() = default;

vkwp::ShaderModule::ShaderModule(const std::string & label)
    : Resource{ label }
{}
