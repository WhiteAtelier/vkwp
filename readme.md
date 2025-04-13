
# Vulkan Wrapper

Vulkan API Wrapper.

## Features

- Destroy in constructor.
- Uncopyable, Movable instance.
- Move assignable instance.
- Valid/Invalid container.
- (WIP) Memory Pool and Memory Node.

独自の wrapper, まあ, みんな作るよねw

### Destroy in destructor

```c++
{
    vkwp::Image image = device.createImage(...);  // Create instance or throw
    assert(image);  // Valid instance.
} // end of scope, image is deleted.
```

### Uncopyable, movable instance.

like `std::unique_ptr<>`.

```c++
vkwp::Image image = device.createImage(...);
vkwp::Image image2 = image;  // Error. Not sharing.
vkwp::Image image3 = std::move(image); // OK. image will be invalid.
```

### Move assignable, Valid/Invalid container.

```c++
vkwp::Image image; // Create invalid (holder) instance.
assert(!image);
...
image = device.createImage(options...); // Move assign. OK.
assert(image);
```

### [WIP] Memory Pool, Memory Node

```c++
// VkDeviceMemory instance is ONE per a pool.
// You can reduce VkDeviceMemory allocation count.
vkwp::MemoryPool pool = device.createMemoryPool(...);
{
    vkwp::Image image1 = device.createImage(...);
    vkwp::Image image2 = device.createImage(...);
    vkwp::MemoryNode node1 = pool.allocateNode(image1.getMemoryRequirement());
    vkwp::MemoryNode node2 = pool.allocateNode(image2.getMemoryRequirement());
    image1.bindMemory(node1);
    image2.bindMemory(node2);
} // end of scope, node1 and node2 are released and be reusable space.
```

## Requirements

- C++20 or later

## Dependencies

- Vulkan
    - [https://vulkan.lunarg.com/](https://vulkan.lunarg.com/)
- gabime/spdlog
    - Optional, With: `VKWP_SPDLOG_LOGGING=ON`
    - [https://github.com/gabime/spdlog](https://github.com/gabime/spdlog)
- glfw/glfw
    - Optional, With: `VKWP_SUPPORT_GLFW_SURFACE=ON`
    - Version: 3
    - For supporting glfw surface.
    - [https://github.com/glfw/glfw](https://github.com/glfw/glfw)
- google/googletest
    - With: `BUILD_TESTS=ON`
    - [https://github.com/google/googletest](https://github.com/google/googletest)
- doxygen
    - With: `BUILD_DOCS=ON`
    - [https://doxygen.nl/](https://doxygen.nl/)

## Build

### CMake Options

| Option                     | Type  | Default | Description                            |
|----------------------------|-------|---------|----------------------------------------|
| VKWP_SPDLOG_LOGGING        | BOOL  | `OFF`   | Use spdlog for logging.                |
| VKWP_SUPPORT_GLFW_SURFACE  | BOOL  | `OFF`   | glfw surface support.                  |
| VKWP_SUPPORT_WIN32_SURFACE | BOOL  | `OFF`   | win32 surface support. (Windows only.) |
| BUILD_TESTS                | BOOL  | `OFF`   | Build test projects.                   |
| BUILD_DOCS                 | BOOL  | `OFF`   | Build documentation.                   |
| (CMAKE_DEBUG_POSTFIX)      | STRING| `d`     |                                        |
