
#ifndef VKWP_IMAGE_VIEW_HPP
#define VKWP_IMAGE_VIEW_HPP

#include "fwd.hpp"
#include "resource.hpp"
#include "vulkan.hpp"

namespace vkwp {

///
/// @brief VkImageView wrapper.
///
class ImageView final : public Resource<VkImageView, VkDevice, void>
{
public:
    /// @brief Default Constructor
    [[nodiscard]]
    explicit ImageView();

    /// @brief Constructor with label name.
    [[nodiscard]]
    explicit ImageView(const std::string & label);
};

}  // namespace vkwp

#endif
