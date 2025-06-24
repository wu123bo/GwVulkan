CMAKE_MINIMUM_REQUIRED (VERSION 3.10...3.29)

project(VulkanPro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    ../3rd/stb/
    ../3rd/glm/
    ../3rd/glfw/include/
    ../3rd/vulkan/include/
    ../3rd/imgui/
    ../3rd/imgui/backends/
)

set(SRC
    src/main.cpp
    src/MacroHead.h
    src/Helper/Print.h
    src/Helper/Print.cpp
    src/TriangleFunc.h
    src/TriangleFunc.cpp
)

set(IMGUI_SRC
    ../3rd/imgui/imgui.cpp
    ../3rd/imgui/imgui_draw.cpp
    ../3rd/imgui/imgui_tables.cpp
    ../3rd/imgui/imgui_widgets.cpp
    ../3rd/imgui/backends/imgui_impl_glfw.cpp
    ../3rd/imgui/backends/imgui_impl_vulkan.cpp
)

add_library(libImgui STATIC ${IMGUI_SRC})

add_executable(${PROJECT_NAME} ${SRC})

# 如果 Vulkan 库目录特殊，再加下面这句
set(VULKAN_LIB_DIR ${CMAKE_SOURCE_DIR}/3rd/vulkan/lib/)
target_link_directories(${PROJECT_NAME} PRIVATE ${VULKAN_LIB_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw vulkan-1 libImgui)
