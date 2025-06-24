#ifndef TRIANGLEFUNC_H_
#define TRIANGLEFUNC_H_

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"

#include "MacroHead.h"

class TriangleFunc
{
public:
    TriangleFunc();

    ~TriangleFunc();

public:
    void Run();

private:
    /**
     * @brief 初始化窗口系统（如 GLFW）。
     *
     * 创建用于 Vulkan 渲染的窗口句柄。
     */
    void initWindow();

    /**
     * @brief 初始化 ImGui，绑定 Vulkan 和 GLFW，用于后续绘制界面。
     */
    void initImgui();

    /**
     * @brief 初始化 Vulkan 相关对象。
     */
    void initVulkan();

    /**
     * @brief 主循环。
     *
     * 处理窗口事件并保持程序运行，直到用户关闭窗口。
     */
    void mainLoop();

    /**
     * @brief 清理 Vulkan 使用过程中分配的所有资源。
     *
     * 本函数应在程序退出前调用，确保所有 Vulkan 对象被正确销毁以避免资源泄露。
     * 销毁顺序遵循依赖关系，从最底层资源到最高层接口逐步释放。
     */
    void cleanup();

private:
    /**
     * @brief 创建 Vulkan 实例（VkInstance）。
     *
     * 设置应用信息、所需扩展、验证层等，并调用 vkCreateInstance。
     */
    void createInstance();

    /**
     * @brief 设置 Vulkan 验证层调试回调（Debug Messenger）。
     *
     * 启用验证层调试输出，在开发阶段用于捕获 API 使用错误。
     */
    void setupDebugMessenger();

    /**
     * @brief 从系统所有 Vulkan 物理设备中选择一个合适的设备。
     *
     * 选择标准示例：支持必需的队列族和扩展，且是离屏渲染或具有图形能力的设备。
     */
    void pickPhysicalDevice();

    /**
     * @brief 创建逻辑设备并获取所需的图形与呈现队列。
     *
     * 基于当前物理设备初始化 VkDevice，并获取队列句柄。
     *
     */
    void createLogicalDevice();

    /**
     * @brief 创建 Vulkan 与窗口系统关联的表面。
     *
     * 使用 GLFW 创建 VkSurfaceKHR，用于图像呈现。
     *
     */
    void createSurface();

    /**
     * @brief 创建 Vulkan 交换链，完成图像格式、尺寸和呈现模式的选择。
     *
     * - 查询设备支持的交换链细节。
     * - 选择合适的表面格式、呈现模式和交换链图像尺寸。
     * - 创建交换链对象（VkSwapchainKHR）。
     * - 获取交换链包含的图像句柄。
     *
     * @throws std::runtime_error 创建交换链失败时抛出。
     */
    void createSwapChain();

    /**
     * @brief 为交换链中的每个图像创建对应的图像视图（VkImageView）。
     *
     * 图像视图描述了图像的使用方式（如 2D 颜色附件），
     * 是后续创建帧缓冲、渲染目标的基础。
     *
     * @throws std::runtime_error 创建任意一个视图失败时抛出异常。
     */
    void createImageViews();

    /**
     * @brief 创建 Vulkan 图形管线（Graphics Pipeline）。
     *
     * 图形管线将顶点着色器、片段着色器及各类固定功能阶段组合在一起，
     * 最终用于渲染流程中绘制几何图元。
     *
     * 本函数执行：
     * - 着色器模块加载
     * - 固定功能阶段配置（顶点输入、视口、装配、光栅化、混色等）
     * - 管线布局创建
     * - 调用 vkCreateGraphicsPipelines 创建图形管线对象
     *
     * @throws std::runtime_error 如果创建失败
     */
    void createGraphicsPipeline();

    /**
     * @brief 创建 Vulkan 渲染通道（Render Pass）。
     *
     * 渲染通道定义了一次渲染中使用的附件、子通道结构，以及它们的依赖关系。
     * 当前只配置了一个颜色附件用于交换链图像的输出。
     *
     * @throws std::runtime_error 如果创建渲染通道失败。
     */
    void createRenderPass();

    /**
     * @brief 为交换链中的每一个图像视图创建帧缓冲对象（Framebuffer）。
     *
     * 每个帧缓冲与一个图像视图绑定，用于在指定的渲染通道（_renderPass）中进行渲染输出。
     * 通常每帧渲染对应一个图像视图，因此需创建多个帧缓冲，与交换链图像一一对应。
     *
     * @throws std::runtime_error 如果帧缓冲创建失败。
     */
    void createFramebuffers();

    /**
     * @brief 创建命令池（Command Pool），用于分配命令缓冲区。
     *
     * 命令池负责管理命令缓冲的内存分配，所有图形命令都将通过该池分配。
     * 此处绑定到图形队列族，后续渲染所用命令缓冲也将从此命令池分配。
     *
     * 设置 VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 标志，允许重置单个命令缓冲。
     *
     * @throws std::runtime_error 如果命令池创建失败。
     */
    void createCommandPool();

    /**
     * @brief 为每一帧分配主命令缓冲（Primary Command Buffers）。
     *
     * Vulkan 中的命令缓冲用于记录 GPU 要执行的渲染指令。
     * 由于采用了多帧并发（如双缓冲或三缓冲），每一帧都需要一个独立的命令缓冲。
     *
     * - 使用 _commandPool 分配命令缓冲。
     * - 命令缓冲级别为 VK_COMMAND_BUFFER_LEVEL_PRIMARY，表示可直接提交给队列。
     * - 分配数量等于 _MAX_FRAMES_IN_FLIGHT，用于支持多帧并发。
     *
     * @throws std::runtime_error 如果命令缓冲分配失败。
     */
    void createCommandBuffers();

    /**
     * @brief 创建每帧所需的同步对象（信号量和栅栏）。
     *
     * Vulkan 使用同步对象来协调 CPU 与 GPU 之间的执行顺序，避免资源冲突。
     * 本函数为每帧并发（通常为 2 或 3 帧）分配以下同步对象：
     * - 图像可用信号量（_imageAvailableSemaphores）：在图像获取完成时被触发。
     * - 渲染完成信号量（_renderFinishedSemaphores）：在命令缓冲执行完毕时触发，供呈现使用。
     * - CPU-GPU 栅栏（_inFlightFences）：确保每帧开始时上一帧的命令已执行完毕。
     *
     * 所有信号量创建为初始未触发状态，栅栏设置为初始“已触发”，以允许第一帧立即开始渲染。
     *
     * @throws std::runtime_error 如果任一同步对象创建失败。
     */
    void createSyncObjects();

    /**
     * @brief 每帧渲染逻辑（Frame Rendering）。
     *
     * 本函数实现了 Vulkan 中的帧渲染流程，采用多帧并发（Frames in Flight）机制，
     * 使用多个信号量（semaphores）和栅栏（fences）避免 CPU 与 GPU 同步阻塞。
     *
     * 渲染流程概览：
     * 1. 等待当前帧的栅栏，确保上一帧使用的资源已经可用；
     * 2. 从交换链中获取一张可用图像；
     * 3. 重置命令缓冲并记录新的渲染命令；
     * 4. 提交渲染命令到图形队列，使用信号量同步 GPU 渲染流程；
     * 5. 将渲染完成的图像提交给呈现队列进行显示；
     * 6. 更新当前帧索引，实现循环帧处理（避免资源冲突）。
     *
     * 注意：
     * - 多帧并发通常设置为 2 或 3 帧；
     * - 使用信号量确保图像获取与渲染顺序；
     * - 使用栅栏确保命令缓冲在重用前不会被 GPU 使用中。
     */
    void drawFrame();

private:
    /**
     * @brief 获取当前系统支持的 Vulkan 实例扩展列表。
     *
     * 用于查询所有可用的 Vulkan 实例扩展名称，
     * 可用于验证 GLFW 或调试功能所需的扩展是否支持。
     *
     * @return std::vector<std::string> 支持的扩展名称列表。
     */
    std::vector<std::string> checkValidationInstanceExtensions();

private:
    /**
     * @brief 判断给定的物理设备是否满足程序要求。
     *
     * 包括检查是否支持图形队列族、必要的设备扩展（如 swapchain）、表面支持等。
     *
     * @param device 待评估的 Vulkan 物理设备（VkPhysicalDevice）。
     * @return true  该设备满足所有条件，可用于创建逻辑设备。
     * @return false 设备不满足要求，需跳过。
     */
    bool isDeviceSuitable(VkPhysicalDevice device);

    /**
     * @brief 检查物理设备是否支持所有所需的设备扩展。
     *
     * 常用于判断设备是否支持 VK_KHR_swapchain 等关键扩展。
     *
     * @param device 要检查的 Vulkan 物理设备。
     * @return true  设备支持所有必需扩展。
     * @return false 至少缺少一个扩展。
     */
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    /**
     * @brief 评估 Vulkan 物理设备的适用性并打分。
     *
     * 用于从多个设备中选择最佳 GPU。
     *
     * @param device 要评估的物理设备。
     * @return int 得分（高分优先选择）。
     */
    int rateDeviceSuitability(VkPhysicalDevice device);

    /**
     * @brief 查找指定物理设备支持的图形队列族与呈现队列族。
     *
     * 遍历设备支持的所有队列族，查找是否支持：
     * - 图形指令（VK_QUEUE_GRAPHICS_BIT）
     * - 指定 surface 的图像呈现支持（vkGetPhysicalDeviceSurfaceSupportKHR）
     *
     * @param device 要查询的 Vulkan 物理设备。
     * @return QueueFamilyIndices 包含图形队列和呈现队列的索引（如果找到）。
     * @throws std::runtime_error 如果队列族数量为 0。
     */
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

private:
    /**
     * @brief 查询指定物理设备对当前窗口表面（_surface）的交换链支持信息。
     *
     * 包括表面能力（分辨率范围、图像数量限制）、支持的表面格式和呈现模式。
     *
     * @param device 要查询的 Vulkan 物理设备。
     * @return SwapChainSupportDetails 包含支持信息的结构体。
     */
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    /**
     * @brief 选择交换链表面格式（颜色格式 + 色彩空间）。
     *
     * 优先选择 VK_FORMAT_B8G8R8A8_SRGB + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR，
     * 若未找到，则回退使用第一个可用格式。
     *
     * @param availableFormats 可用的表面格式列表。
     * @return VkSurfaceFormatKHR 选定的表面格式。
     */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    /**
     * @brief 选择交换链的呈现模式（帧同步方式）。
     *
     * 优先选择 VK_PRESENT_MODE_MAILBOX_KHR（低延迟、三重缓冲），
     * 若不可用，则回退到 VK_PRESENT_MODE_FIFO_KHR（V-Sync，所有设备都支持）。
     *
     * @param availablePresentModes 可用的呈现模式列表。
     * @return VkPresentModeKHR 选定的呈现模式。
     */
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    /**
     * @brief 选择交换链图像的分辨率（VkExtent2D）。
     *
     * 若 `currentExtent` 不为特殊值，表示窗口大小已固定，直接返回即可；
     * 否则从窗口系统获取帧缓冲大小，并限制在设备支持的最小/最大范围内。
     *
     * @param capabilities 表面能力信息（VkSurfaceCapabilitiesKHR）。
     * @return VkExtent2D 最终用于交换链的图像尺寸。
     */
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

private:
    /**
     * @brief 读取指定文件的全部内容（通常用于加载 SPIR-V 着色器字节码）。
     *
     * 使用二进制方式打开文件，并读取所有数据到缓冲区中返回。
     *
     * @param filename 文件路径（相对或绝对路径）。
     * @return std::vector<char> 文件的原始字节内容。
     *
     * @throws std::runtime_error 如果文件打开失败。
     */
    static std::vector<char> readFile(const std::string &filename);

    /**
     * @brief 创建着色器模块（VkShaderModule）。
     *
     * 从预编译的 SPIR-V 字节码（.spv 文件）创建 Vulkan 着色器模块，
     * 该模块可用于后续创建图形管线（如顶点着色器、片段着色器）。
     *
     * @param code 包含 SPIR-V 字节码的二进制数据（通常通过文件读取得到）。
     * @return VkShaderModule 创建成功的着色器模块句柄。
     *
     * @throws std::runtime_error 如果着色器模块创建失败。
     */
    VkShaderModule createShaderModule(const std::vector<char> &code);

private:
    /**
     * @brief 录制图形命令到指定的命令缓冲中。
     *
     * 该函数将绘制指令（如绑定渲染管线、设置视口、绘制三角形）写入指定的主命令缓冲区中，
     * 用于后续提交到图形队列执行。每帧图像都需要一份对应的绘制命令。
     *
     * @param commandBuffer 要写入命令的 VkCommandBuffer（通常为主命令缓冲）。
     * @param imageIndex    当前使用的交换链图像索引，用于选择对应的帧缓冲。
     *
     * @throws std::runtime_error 如果命令缓冲录制开始或结束失败。
     */
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

private:
    /**
     * @brief 窗口帧缓冲尺寸变化时的回调函数。
     *
     * 当用户调整窗口大小（如拖动窗口边界）时，GLFW 会调用此函数。
     * Vulkan 需要在帧缓冲大小发生变化后重建交换链（Swapchain），
     * 所以此处设置一个标志 `_framebufferResized`，在渲染循环中检测该标志并重建相关资源。
     *
     * @param window 触发事件的 GLFW 窗口句柄。
     * @param width 新的帧缓冲区宽度（像素）。
     * @param height 新的帧缓冲区高度（像素）。
     */
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    /**
     * @brief 重建交换链及其相关资源。
     *
     * 当窗口尺寸发生变化（如最小化或用户手动调整大小）后，
     * 原有的交换链不再适用。该函数等待窗口恢复有效大小后，
     * 先使设备空闲，然后清理旧的交换链资源，再重新创建交换链、
     * 图像视图和帧缓冲对象等依赖于交换链的资源。
     *
     * 注意：不要在绘制过程中调用此函数，应在 `vkQueuePresentKHR`
     * 返回 `VK_ERROR_OUT_OF_DATE_KHR` 或 `VK_SUBOPTIMAL_KHR` 时调用，
     * 或通过窗口大小回调设置的标志位触发。
     */
    void recreateSwapChain();

    /**
     * @brief 清理与交换链相关的资源。
     *
     * 此函数会销毁帧缓冲对象、图像视图和交换链本身。
     * 在窗口尺寸改变时，必须先销毁这些旧资源，然后重建新的交换链及其依赖资源。
     *
     * 注意：此函数不销毁与交换链无关的资源，如渲染通道或图形管线等，
     * 若这些资源依赖于交换链的格式或尺寸，也应在外部处理并重建。
     */
    void cleanupSwapChain();

private:
    /**
     * @brief 检查当前系统是否支持指定的 Vulkan 验证层（如 VK_LAYER_KHRONOS_validation）。
     *
     * 在启用调试功能前调用此函数确认验证层是否可用。
     *
     * @return true  系统支持验证层。
     * @return false 系统不支持验证层。
     */
    bool checkValidationLayerSupport();

    /**
     * @brief 获取 Vulkan 实例创建所需的扩展列表。
     *
     * 包括 GLFW 要求的扩展，若启用验证层，则附加 VK_EXT_debug_utils 扩展。
     *
     * @return std::vector<const char*> 扩展名称指针数组。
     */
    std::vector<const char *> getRequiredExtensions();

    /**
     * @brief 填充 VkDebugUtilsMessengerCreateInfoEXT 结构体。
     *
     * 用于初始化调试信息回调的配置项，包括消息级别、类型和回调函数指针等。
     * 可用于 vkCreateDebugUtilsMessengerEXT 或作为 vkCreateInstance 的 pNext 参数。
     *
     * @param createInfo  输出的调试回调创建信息结构体，将被填充完整。
     *
     * @note
     *  - 回调函数使用默认的 debugCallback。
     *  - 若用于 vkCreateInstance 的 pNext，需要在实例创建前调用。
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    /**
     * @brief 创建 Vulkan 的调试信息回调（Debug Messenger）。
     *
     * 此函数是 VK_EXT_debug_utils 扩展提供的创建接口，
     * 用于在运行时接收验证层的错误、警告与性能提示信息。
     *
     * @param instance       Vulkan 实例句柄。
     * @param pCreateInfo    调试回调创建信息结构体指针。
     * @param pAllocator     自定义内存分配器（可为 nullptr）。
     * @param pDebugMessenger 返回创建的调试回调句柄。
     *
     * @return VkResult      创建结果（VK_SUCCESS 表示成功）。
     *
     * @note
     *  此函数必须通过 vkGetInstanceProcAddr 获取，Vulkan 并未默认暴露该扩展函数。
     */
    VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);

    /**
     * @brief 销毁 Vulkan 的调试信息回调（Debug Messenger）。
     *
     * 此函数是 VK_EXT_debug_utils 扩展提供的销毁接口，
     * 用于释放通过 vkCreateDebugUtilsMessengerEXT 创建的调试句柄。
     *
     * @param instance        Vulkan 实例句柄。
     * @param debugMessenger  需要销毁的调试回调句柄。
     * @param pAllocator      自定义内存分配器（可为 nullptr）。
     *
     * @note
     *  此函数也需通过 vkGetInstanceProcAddr 动态加载。
     */
    void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

    /**
     * @brief Vulkan 验证层调试回调函数。
     *
     * 在启用 VK_EXT_debug_utils 扩展后注册此函数，
     * 可在运行时输出验证层的警告、错误与提示信息。
     *
     * @param messageSeverity 消息严重等级（如 ERROR/WARNING/INFO）。
     * @param messageType     消息类型（验证、性能、一般性）。
     * @param pCallbackData   包含调试信息的结构体指针。
     * @param pUserData       有关无效且可能导致崩溃的行为的消息。
     *
     * @return VkBool32       返回 VK_FALSE 继续执行，VK_TRUE 会中断 Vulkan 调用（一般返回 VK_FALSE）。
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

private:
    /**
     * @brief 创建用于 ImGui 的 Vulkan 描述符池。
     *
     * 描述符池（Descriptor Pool）是 Vulkan 中用于管理描述符集（Descriptor Sets）内存的对象。
     * ImGui 在 Vulkan 后端需要大量不同类型的描述符（如采样器、图像、缓冲区等）来绑定资源，
     * 因此需要创建一个容量较大的描述符池，保证在渲染过程中不会出现描述符分配不足的问题。
     *
     * 函数实现细节：
     * - 定义一个 VkDescriptorPoolSize 数组，列举了 ImGui 可能用到的各种描述符类型及其数量。
     * - 设置 VkDescriptorPoolCreateInfo 结构体，指定池的创建参数：
     *   - flags 设置为 VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT，允许释放单个描述符集。
     *   - maxSets 是池中最大描述符集数量，设置为所有类型数量之和的乘积。
     *   - poolSizeCount 和 pPoolSizes 指向描述符类型数组。
     * - 调用 vkCreateDescriptorPool 创建描述符池，失败时抛出异常。
     */
    void createImGuiDescriptorPool();

    /**
     * @brief 检查 Vulkan 函数调用的返回结果，输出错误信息并在错误时终止程序。
     *
     * @param err Vulkan 函数返回的 VkResult 结果码。
     *
     * 如果返回值是 VK_SUCCESS，说明操作成功，函数直接返回。
     * 如果返回错误码，则打印错误信息。
     * 如果错误码是负值（表示严重错误），程序会调用 abort() 终止运行。
     */
    static void check_vk_result(VkResult err);

    /**
     * @brief 开始一次短期使用的命令缓冲录制。
     *
     * 该函数为一次性提交（one-time submit）场景准备命令缓冲：
     *  - 从命令池分配一个主命令缓冲（primary command buffer）。
     *  - 开始录制，设置使用标志为 ONE_TIME_SUBMIT，提示 Vulkan 该缓冲只用一次。
     *
     * @return VkCommandBuffer 返回新分配并开始录制的命令缓冲对象。
     */
    VkCommandBuffer beginSingleTimeCommands();

    /**
     * @brief 结束一次短期使用的命令缓冲录制并提交执行。
     *
     * 该函数会完成命令缓冲的录制，提交到图形队列执行，并等待执行完成后释放该命令缓冲。
     *
     * @param commandBuffer 已开始录制的命令缓冲对象。
     */
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    /**
     * @brief 在给定的 Vulkan 命令缓冲中渲染 ImGui 界面。
     *
     * 该函数负责启动 ImGui 新帧，构建示例 UI 界面内容，最终将 ImGui 绘制命令提交到 Vulkan 命令缓冲。
     *
     * @param cmdBuf 用于提交 ImGui 绘制命令的 Vulkan 命令缓冲。
     */
    void renderImGui(VkCommandBuffer cmdBuf);

private:
    int _width;

    int _height;

    // GLFW窗口句柄，负责窗口管理和输入处理
    GLFWwindow *_window;

    //窗口背景色
    glm::vec3 _backColor = glm::vec3(0.0f);

private:
    // Vulkan实例，代表整个Vulkan连接和状态
    VkInstance _instance = VK_NULL_HANDLE;

private:
    // 选定的物理设备（GPU），用于支持Vulkan操作
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

private:
    // 创建的逻辑设备，用于与物理设备交互
    VkDevice _device = VK_NULL_HANDLE;

    // 逻辑设备中的图形队列句柄，用于提交绘制命令
    VkQueue _graphicsQueue = VK_NULL_HANDLE;

private:
    // 逻辑设备中的呈现队列句柄，用于提交图像呈现请求
    VkQueue _presentQueue = VK_NULL_HANDLE;

private:
    /*vulkan窗口表面完全是一个 可选组件，如果你只需要离屏渲染。
    Vulkan 浏览器 允许您在没有创建不可见窗口等技巧的情况下执行此作 （对于 OpenGL 是必需的）。*/

    // 窗口表面
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

private:
    // 交换链对象句柄，用于管理用于显示的图像集合。
    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;

    // 交换链图像列表，每个图像用于帧的显示输出。
    std::vector<VkImage> _swapChainImages;

    // 交换链图像的像素格式（如 VK_FORMAT_B8G8R8A8_SRGB）。
    VkFormat _swapChainImageFormat;

    // 交换链图像的尺寸（宽高），与窗口帧缓冲一致。
    VkExtent2D _swapChainExtent;

    // 所需启用的逻辑设备扩展列表，目前仅包含 VK_KHR_swapchain。
    const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

private:
    // 交换链图像视图列表，每个交换链图像对应一个 VkImageView，
    // 用于描述图像在渲染管线中的访问方式和格式信息。
    std::vector<VkImageView> _swapChainImageViews;

private:
    // 渲染通道对象，用于定义帧缓冲中附件的使用方式和生命周期（如颜色、深度等）。
    VkRenderPass _renderPass;

    // 图形渲染管线对象，封装了整个图形绘制流程（包含着色器、输入装配、光栅化等阶段）。
    VkPipeline _graphicsPipeline;

    // 管线布局对象，指定了着色器所需的资源绑定接口（如 descriptor set、push constant 等）。
    VkPipelineLayout _pipelineLayout;

private:
    // 交换链对应的帧缓冲区列表，每个交换链图像对应一个帧缓冲区
    std::vector<VkFramebuffer> _swapChainFramebuffers;

private:
    // 命令池，用于管理和分配命令缓冲区
    VkCommandPool _commandPool;

    // 主要的命令缓冲区，用于记录绘制指令
    std::vector<VkCommandBuffer> _commandBuffers;

private:
    bool _framebufferResized = false;

    // CPU-GPU 同步对象，标记当前帧是否执行完成
    std::vector<VkFence> _inFlightFences;

    // 信号量，表示交换链图像是否可用，等待此信号量后开始渲染
    std::vector<VkSemaphore> _imageAvailableSemaphores;

    // 信号量，表示渲染是否完成，等待此信号量后提交呈现请求
    std::vector<VkSemaphore> _renderFinishedSemaphores;

private:
    uint32_t _currentFrame = 0;

    const int _MAX_FRAMES_IN_FLIGHT = 2;

private:
    // 验证层,只有Debug时运行
#ifdef NDEBUG
    const bool _enableValidationLayers = false;
#else
    const bool _enableValidationLayers = true;
#endif

    // 调试消息
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;

    // 默认验证层信息
    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};

private:
    //imgui
    uint32_t _graphicsQueueFamily = 0;

    VkDescriptorPool _imguiDescriptorPool = VK_NULL_HANDLE;
};

#endif    // !TRIANGLEFUNC_H_
