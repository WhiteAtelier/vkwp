#include "vkwp/sampler.hpp"

vkwp::Sampler::Sampler() = default;

vkwp::Sampler::Sampler(const std::string & label)
    : Resource{ label }
{}
