# Vulkan Wrapper

Vulkan API Wrapper.

## Features

- Destroy in constructor.
- Uncopyable, Movable instance.
- Move assignable instance.
- Valid/Invalid container.
- (WIP) Memory Pool and Memory Node.

独自の wrapper, まあ, みんな作るよね w

### Destroy in destructor

```c++
{
    vkwp::Image image = vkwp::Image::create(...);  // Create instance or throw
    assert(image);  // Valid instance.
} // end of scope, image is deleted.
```

### Uncopyable, movable instance.

like `std::unique_ptr<>`.

```c++
vkwp::Image image = vkwp::Image::create(...);
vkwp::Image image2 = image;  // Error. Not sharing.
vkwp::Image image3 = std::move(image); // OK. image will be invalid.
```

### Move assignable, Valid/Invalid container.

```c++
vkwp::Image image; // Create invalid (holder) instance.
assert(!image);
...
image = vkwp::Image::create(...); // Move assign. OK.
assert(image);
```

## License

MIT License.

Copyright (c) 2026 Shirao Shotaro, WhiteAtelier

See the LICENSE file in the project root for the full license text.

Third parties using this library **must use version 2.5.0 or later**, to which the MIT License applies.

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
  - With: `VKWP_BUILD_TESTS=ON`
  - [https://github.com/google/googletest](https://github.com/google/googletest)
- doxygen
  - With: `VKWP_BUILD_DOCS=ON`
  - [https://doxygen.nl/](https://doxygen.nl/)

### gabime/spdlog License

The MIT License (MIT)

Copyright (c) 2016 - present, Gabi Melman and spdlog contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

-- NOTE: Third party dependency used by this software --
This software depends on the fmt lib (MIT License),
and users must comply to its license: https://raw.githubusercontent.com/fmtlib/fmt/master/LICENSE

### google/googletest License

Copyright 2008, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright

notice, this list of conditions and the following disclaimer.
_ Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
_ Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Build

### CMake Options

| Option                     | Type   | Default          | Description                            |
| -------------------------- | ------ | ---------------- | -------------------------------------- |
| VKWP_SPDLOG_LOGGING        | BOOL   | `OFF`            | Use spdlog for logging.                |
| VKWP_SUPPORT_GLFW_SURFACE  | BOOL   | `OFF`            | glfw surface support.                  |
| VKWP_SUPPORT_WIN32_SURFACE | BOOL   | `OFF`            | win32 surface support. (Windows only.) |
| VKWP_BUILD_TESTS           | BOOL   | `OFF`            | Build test projects.                   |
| VKWP_BUILD_DOCS            | BOOL   | `OFF`            | Build documentation.                   |
| VKWP_INSTALL               | BOOL   | `ON` (when root) | Install libraries.                     |
| (CMAKE_DEBUG_POSTFIX)      | STRING | `d`              |                                        |
