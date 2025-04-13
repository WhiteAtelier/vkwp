#include "vkwp/render_pass.hpp"

vkwp::RenderPass::RenderPass() = default;

vkwp::RenderPass::RenderPass(const std::string & label)
    : Resource{ label }
{}
