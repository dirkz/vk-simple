# vk-simple

This is what happens when one follows [the original vulkan tutorial](https://vulkan-tutorial.com/),
including its use of
[render passes](https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes),
but fully embracing [vk::raii](https://github.com/KhronosGroup/Vulkan-Hpp/blob/main/vk_raii_ProgrammingGuide.md),
using [VMA](https://gpuopen.com/vulkan-memory-allocator/) for memory management,
[slang](https://github.com/shader-slang/slang) for compiling shaders,
[SDL3](https://wiki.libsdl.org/SDL3/FrontPage) for handling the UI,
and [CMake](https://cmake.org/) for project setup.

You'll need the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) installed,
or you [build it yourself](https://github.com/dirkz/vulkan-sdk-builder).

Other dependencies are "vendored in", like GLM. In this case, that means
they are integrated as git submodules, with their SSH URLs.

## How to check out (with all submodules)

Either from the start:

```
git clone --recurse-submodules <repo_url>
```

Or after a normal clone:

```
git submodule update --init --recursive
```