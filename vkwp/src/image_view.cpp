#include "vkwp/image_view.hpp"

vkwp::ImageView::ImageView() = default;

vkwp::ImageView::ImageView(const std::string & label)
    : Resource{ label }
{}
