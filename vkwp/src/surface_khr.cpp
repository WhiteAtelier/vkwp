#include "vkwp/surface_khr.hpp"

vkwp::SurfaceKHR::SurfaceKHR() = default;

vkwp::SurfaceKHR::SurfaceKHR(const std::string & label)
    : Resource{ label }
{}
