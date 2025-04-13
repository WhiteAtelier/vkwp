#include "vkwp/framebuffer.hpp"

vkwp::Framebuffer::Framebuffer() = default;

vkwp::Framebuffer::Framebuffer(const std::string & label)
    : Resource{ label }
{}
