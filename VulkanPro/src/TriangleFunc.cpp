#include "TriangleFunc.h"
#include "Helper/Print.h"

TriangleFunc::TriangleFunc()
    : _width(800)
    , _height(600)
    , _window(nullptr)
{
}

TriangleFunc::~TriangleFunc() {}

void TriangleFunc::Run()
{
    initWindow();
    initVulkan();
    initImgui();
    mainLoop();
    cleanup();
}

void TriangleFunc::initWindow()
{
    // 初始化GLFW 库
    glfwInit();

    // 禁用OpenGl API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // 不可调整窗口大小
    //  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // 创建GLFW窗口
    _window = glfwCreateWindow(_width, _height, "Vulkan", nullptr, nullptr);

    // 绑定当前类实例指针到 GLFW 窗口，方便回调中访问类成员
    glfwSetWindowUserPointer(_window, this);

    // 设置窗口大小改变时的回调函数，负责标记交换链需要重新创建
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void TriangleFunc::initImgui()
{
    // 检查 ImGui 版本，确保链接的库版本正确
    IMGUI_CHECKVERSION();

    // 创建 ImGui 上下文，所有 ImGui 操作都基于这个上下文
    ImGui::CreateContext();

    // 获取 ImGui IO 配置对象，用于配置和查询输入输出状态
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // 启用键盘导航支持
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 启用游戏手柄导航支持
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 设置 ImGui 使用暗色主题样式
    ImGui::StyleColorsDark();

    // 初始化 ImGui 的平台层 GLFW 支持，传入 Vulkan 窗口和是否安装回调
    ImGui_ImplGlfw_InitForVulkan(_window, true);

    // 加载中文字体，设置字号18，支持完整的中文字符范围（GlyphRangesChineseFull）
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\STXINWEI.TTF", 18.0f, nullptr,
                                 io.Fonts->GetGlyphRangesChineseFull());
    IM_ASSERT(io.Fonts != nullptr);

    // 创建 ImGui 需要的 Vulkan 描述符池（Descriptor Pool），用于分配资源
    createImGuiDescriptorPool();

    // 填写 Vulkan 端的初始化信息结构体
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = _instance;                                           // Vulkan 实例句柄
    init_info.PhysicalDevice = _physicalDevice;                               // 物理设备句柄
    init_info.Device = _device;                                               // 逻辑设备句柄
    init_info.QueueFamily = _graphicsQueueFamily;                             // 图形队列族索引
    init_info.Queue = _graphicsQueue;                                         // 图形队列句柄
    init_info.PipelineCache = VK_NULL_HANDLE;                                 // Pipeline 缓存，一般为 null
    init_info.DescriptorPool = _imguiDescriptorPool;                          // ImGui 使用的描述符池
    init_info.RenderPass = _renderPass;                                       // 渲染通道句柄
    init_info.Subpass = 0;                                                    // 渲染通道子通道索引
    init_info.Allocator = nullptr;                                            // 分配器，默认空
    init_info.MinImageCount = _MAX_FRAMES_IN_FLIGHT;                          // 最小图像数量（用于多帧同时绘制）
    init_info.ImageCount = static_cast<uint32_t>(_swapChainImages.size());    // 交换链图像数量
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;                            // 多重采样数量，当前设置为1（无多重采样）
    init_info.CheckVkResultFn = check_vk_result;                              // 错误检查回调函数（自定义）

    // 初始化 ImGui Vulkan 后端，完成 Vulkan 相关的绑定设置
    ImGui_ImplVulkan_Init(&init_info);

    // 上传字体纹理到GPU（可选步骤，注释了，需要时取消注释）
    // VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    // ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    // endSingleTimeCommands(commandBuffer);
    // ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void TriangleFunc::initVulkan()
{
    // 创建 Vulkan 实例（createInstance）
    createInstance();

    // 设置调试信息回调（setupDebugMessenger）
    setupDebugMessenger();

    // 创建窗口表面（createSurface）
    createSurface();

    // 选择合适的物理设备（GPU）（pickPhysicalDevice）
    pickPhysicalDevice();

    // 创建逻辑设备及队列（createLogicalDevice）
    createLogicalDevice();

    // 创建交换链（createSwapChain）
    createSwapChain();

    // 创建交换链图像视图（createImageViews）
    createImageViews();

    // 创建渲染通道（createRenderPass）
    createRenderPass();

    // 创建图形管线（createGraphicsPipeline）
    createGraphicsPipeline();

    // 创建帧缓冲（createFramebuffers）
    createFramebuffers();

    // 创建命令池（createCommandPool）
    createCommandPool();

    // 分配命令缓冲区
    createCommandBuffers();

    // 创建用于帧同步的信号量和栅栏
    createSyncObjects();
}

void TriangleFunc::mainLoop()
{
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        drawFrame();
    }

    // 等待 GPU 完成所有操作
    vkDeviceWaitIdle(_device);
}

void TriangleFunc::cleanup()
{
    // 清理交换链相关的资源，包括帧缓冲、图像视图和交换链本身
    cleanupSwapChain();

    // 销毁图形管线对象
    vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
    // 销毁管线布局对象
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);

    // 销毁渲染通道（Render Pass）
    vkDestroyRenderPass(_device, _renderPass, nullptr);

    // 销毁用于同步的信号量和栅栏资源
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);    // 渲染完成信号量
        vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);    // 图像可用信号量
        vkDestroyFence(_device, _inFlightFences[i], nullptr);                  // 同步帧完成栅栏
    }

    // 销毁 ImGui 使用的描述符池
    vkDestroyDescriptorPool(_device, _imguiDescriptorPool, nullptr);

    // 释放所有命令缓冲
    vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

    // 销毁命令池，同时会释放所有命令缓冲区
    vkDestroyCommandPool(_device, _commandPool, nullptr);

    // 销毁逻辑设备，释放所有通过该设备创建的 Vulkan 对象
    vkDestroyDevice(_device, nullptr);

    // 如果启用了验证层，则销毁调试工具回调
    if (_enableValidationLayers) {
        destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
    }

    // 销毁与窗口系统交互的表面对象
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    // 销毁 Vulkan 实例，释放 Vulkan 运行时资源
    vkDestroyInstance(_instance, nullptr);

    // 销毁 GLFW 窗口，释放窗口资源
    glfwDestroyWindow(_window);

    // 终止 GLFW 库，清理 GLFW 分配的全局资源
    glfwTerminate();
}

void TriangleFunc::createInstance()
{
    PrintVec("支持的 Vulkan 实例扩展:", checkValidationInstanceExtensions());

    // 启动验证层
    if (_enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("请求了验证层，但不可用!");
    }

    // vk程序创建信息结构
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "你好,小三角";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // vk创建实例信息结构
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // 设置创建实例需要启用的扩展(例如窗口扩展)
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // 验证层开启时注入调试回调信息
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (_enableValidationLayers) {
        // 设置创建实例信息言验证层(只有在Debug模式下)
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // 创建vulkan实例(2:指向自定义分配器回调的指针,nullptr即可)
    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("创建vk实例失败!");
    }
}

void TriangleFunc::setupDebugMessenger()
{
    if (!_enableValidationLayers) {
        return;
    }

    // 调试回调结构信息（Debug Messenger）
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    // 设置调试消息传递器
    if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("无法设置调试消息传递器!");
    }
}

void TriangleFunc::pickPhysicalDevice()
{
    // 获取系统物理设备数量(GPU)
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("未能找到支持Vulkan的GPU!");
    }

    // 获取系统物理设备信息
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    // 候选设备及得分，按高分优先排序
    std::multimap<int, VkPhysicalDevice, std::greater<int>> candidates;
    for (const auto &device : devices) {
        int score = rateDeviceSuitability(device);
        if (score > 0) {
            candidates.insert(std::make_pair(score, device));
        }
    }

    // 选择得分最高的设备作为目标 GPU
    if (!candidates.empty()) {
        _physicalDevice = candidates.begin()->second;
    } else {
        throw std::runtime_error("未能找到符合要求的 Vulkan 物理设备！");
    }
}

void TriangleFunc::createLogicalDevice()
{
    // 查找当前物理设备支持的队列族（图形 + 呈现）
    QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

    // Imgui使用
    _graphicsQueueFamily = indices.graphicsFamily.value();

    // 使用 std::set 去重，确保不会重复创建相同队列族
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // 为每个唯一队列族创建一个 VkDeviceQueueCreateInfo
    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        // 逻辑队列创建信息
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // 设置要启用的物理设备特性（此处暂未启用任何特性）
    VkPhysicalDeviceFeatures deviceFeatures{};

    // 逻辑设备创建信息
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    // 启用设备扩展（如 swapchain 可在此启用）
    createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

    // 如启用了验证层，则附加层名称
    if (_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("未能创建逻辑设备!");
    }

    // 获取图形队列句柄
    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);

    // 获取呈现队列句柄
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
}

void TriangleFunc::createSurface()
{
    if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("未能创建窗口表面!");
    }
}

void TriangleFunc::createSwapChain()
{
    // 查询交换链支持详情
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);

    // 选择表面格式（颜色格式 + 色彩空间）
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

    // 选择呈现模式（如 FIFO、MAILBOX）
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

    // 选择交换链图像尺寸（分辨率）
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // 期望的图像数量（至少为最小数量 + 1，提升性能）
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // 如果最大图像数量有限制，确保不超过该限制
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // 填写交换链创建信息结构体
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;    // 一般为1，除非是立体或多视图渲染
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // 获取图形和呈现队列族索引
    QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // 如果图形队列族和呈现队列族不同，设置共享模式为并发
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // 同一队列族则使用独占模式（更高效）
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;        // 忽略
        createInfo.pQueueFamilyIndices = nullptr;    // 忽略
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;    // 图像变换（一般无变换）
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;               // 不透明的 alpha 合成
    createInfo.presentMode = presentMode;                                        // 选择的呈现模式
    createInfo.clipped = VK_TRUE;                                                // 剔除不可见像素，提升性能

    createInfo.oldSwapchain = VK_NULL_HANDLE;    // 旧交换链（窗口大小变更时用）

    // 创建交换链
    if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // 获取交换链图像句柄数量
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);

    // 获取交换链图像句柄
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

    // 记录交换链图像格式和尺寸，后续创建图像视图时使用
    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;
}

void TriangleFunc::createImageViews()
{
    // 根据图像数量分配图像视图数组
    _swapChainImageViews.resize(_swapChainImages.size());

    // 为每个交换链图像创建图像视图
    for (size_t i = 0; i < _swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapChainImages[i];         // 指定图像资源
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;    // 2D 视图
        createInfo.format = _swapChainImageFormat;      // 图像格式必须与交换链一致

        // 通道重映射（此处为默认：保持不变）
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // 图像子资源范围（这里只使用颜色，单层，非 mipmap）
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        // 创建图像视图并存储
        if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void TriangleFunc::createGraphicsPipeline()
{
    // === 1. 加载 shader 字节码文件 ===
    auto vertShaderCode = readFile("spv/vert.spv");
    auto fragShaderCode = readFile("spv/frag.spv");

    // === 2. 创建 shader 模块 ===
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // === 3. 配置 shader 阶段 ===
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";    // 着色器入口函数

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // === 4. 顶点输入阶段（当前无顶点数据）===
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    // === 5. 输入装配（组装为三角形）===
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // === 6. 视口与裁剪设置（采用动态设置）===
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // === 7. 光栅化阶段 ===
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // === 8. 多重采样（关闭）===
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // === 9. 颜色混合（直接输出颜色，不做混合）===
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // === 10. 动态状态（视口/剪裁由 command buffer 设置）===
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // === 11. 创建管线布局（无 descriptor set）===
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // === 12. 最终组合图形管线所有结构体 ===
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = _renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    // === 13. 销毁临时着色器模块（管线已复制其内容）===
    vkDestroyShaderModule(_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(_device, vertShaderModule, nullptr);
}

void TriangleFunc::createRenderPass()
{
    // 1. 附件描述（交换链图像）
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChainImageFormat;                     // 与交换链格式一致
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;                    // 无多重采样
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;               // 渲染前清除颜色
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;             // 渲染后保留内容（用于显示）
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // 不使用 stencil
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;        // 开始时不关心图像内容
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // 呈现到屏幕

    // 2. 附件引用（用于 subpass）
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;    // 指向上面那个附件
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // 3. 子通道配置（绑定图形管线，写入颜色附件）
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;    // 图形管线
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // 4. 渲染通道创建信息
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // 5. 创建渲染通道对象
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void TriangleFunc::createFramebuffers()
{
    // 调整帧缓冲容器大小，与图像视图数量一致
    _swapChainFramebuffers.resize(_swapChainImageViews.size());

    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
        // 每个 framebuffer 的附件仅包含一个颜色图像视图
        VkImageView attachments[] = {_swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;    // 使用同一个渲染通道
        framebufferInfo.attachmentCount = 1;         // 仅绑定一个图像视图
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _swapChainExtent.width;
        framebufferInfo.height = _swapChainExtent.height;
        framebufferInfo.layers = 1;    // 不使用立体图层或数组层

        // 创建 framebuffer 对象
        if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void TriangleFunc::createCommandPool()
{
    // 查找当前物理设备中支持图形指令的队列族索引
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);

    // 配置命令池创建信息
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;         // 允许单独重置命令缓冲
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();    // 绑定图形队列族

    // 创建命令池对象
    if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void TriangleFunc::createCommandBuffers()
{
    // 为每帧预留一个命令缓冲
    _commandBuffers.resize(_MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;                 // 从已创建的命令池中分配
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;    // 主命令缓冲
    allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

    // 分配命令缓冲
    if (vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void TriangleFunc::createSyncObjects()
{
    // 预分配每帧所需的同步对象
    _imageAvailableSemaphores.resize(_MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(_MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(_MAX_FRAMES_IN_FLIGHT);

    // 创建信号量的配置信息
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // 创建栅栏的配置信息（初始化为已触发，允许第一次调用 wait）
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // 为每一帧创建一组同步对象
    for (size_t i = 0; i < _MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS
            || vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS
            || vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void TriangleFunc::drawFrame()
{
    // 等待当前帧对应的 Fence，确保上一帧的渲染完成
    vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    // 获取下一张可用于渲染的交换链图像索引
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame],
                                            VK_NULL_HANDLE, &imageIndex);

    // 如果交换链已过期（窗口大小改变等原因），重新创建交换链并返回
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // 重置当前帧的 Fence，准备提交新的命令缓冲
    vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

    // 重置当前帧对应的命令缓冲区，准备记录新命令
    vkResetCommandBuffer(_commandBuffers[_currentFrame], 0);

    // 记录命令缓冲区，指定当前渲染目标图像索引
    recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex);

    // 准备提交信息，等待图像可用信号量，保证图像可写
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    // 指定提交的命令缓冲区
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];

    // 指定信号量，在渲染完成后发出，通知可以呈现
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // 提交命令缓冲区到图形队列，并绑定 Fence 以便同步
    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // 准备呈现信息，等待渲染完成信号量，保证图像可读
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    // 进行图像呈现操作
    result = vkQueuePresentKHR(_presentQueue, &presentInfo);

    // 处理窗口大小改变或交换链子优化问题，重新创建交换链
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        _framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    // 更新当前帧索引，循环使用多帧同步机制
    _currentFrame = (_currentFrame + 1) % _MAX_FRAMES_IN_FLIGHT;
}

std::vector<std::string> TriangleFunc::checkValidationInstanceExtensions()
{
    // 获取 Vulkan 实例扩展数量
    uint32_t extensionCount = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
        std::cerr << "获取Vulkan 实例扩展数量错误!" << std::endl;
        return {};
    }

    // 获取 Vulkan 实例扩展信息
    std::vector<VkExtensionProperties> extensions(extensionCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    if (result != VK_SUCCESS) {
        std::cerr << "获取Vulkan 实例扩展信息错误!" << std::endl;
        return {};
    }

    std::vector<std::string> extensionNames;
    for (const auto &ext : extensions) {
        extensionNames.emplace_back(ext.extensionName);
    }

    return extensionNames;
}

bool TriangleFunc::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool TriangleFunc::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    // 查询设备支持的扩展数量
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    // 获取所有可用扩展属性
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // 将所需扩展转为可擦除的集合
    std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

    // 遍历所有可用扩展，剔除已满足的扩展
    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    // 如果集合为空，说明所有扩展都已满足
    return requiredExtensions.empty();
}

int TriangleFunc::rateDeviceSuitability(VkPhysicalDevice device)
{
    if (!isDeviceSuitable(device)) {
        return 0;
    }

    // 获取指定物理设备的 属性，填充 VkPhysicalDeviceProperties 结构体
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // 获取该物理设备支持的 功能特性（如：几何着色器、tessellation 等）。
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    // 优先使用独立显卡（Discrete GPU）
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // 支持的最大 2D 纹理尺寸越大，图形质量潜力越高
    score += deviceProperties.limits.maxImageDimension2D;

    // 不支持几何着色器的设备直接淘汰
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    PrintPhysicalDev(deviceProperties, score);
    return score;
}

QueueFamilyIndices TriangleFunc::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    // 查询物理设备支持的队列族数量
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        throw std::runtime_error("未能找到当前物理设备的队列族!");
    }

    // 获取队列族属性列表。
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        // 检查是否支持图形指令
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // 检查是否支持将图像呈现到指定 surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        // 如果都找到了，提前结束搜索
        if (indices.isComplete()) {
            break;
        }

        i++;
    }
    return indices;
}

SwapChainSupportDetails TriangleFunc::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    // 查询表面能力（如最小图像数量、当前图像尺寸等）
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

    // 查询支持的表面格式数量
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

    // 如果有格式，获取格式列表
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
    }

    // 查询支持的呈现模式数量
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

    // 如果有模式，获取模式列表
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR TriangleFunc::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    // 回退策略：使用第一个可用格式
    return availableFormats[0];
}

VkPresentModeKHR TriangleFunc::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // 所有 Vulkan 设备都必须支持 FIFO 模式
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D TriangleFunc::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    // 如果 currentExtent 不为特殊值，说明平台已指定大小（如某些操作系统）
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        // 从窗口中获取帧缓冲实际尺寸（单位：像素）
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        // 限制图像大小在设备支持的范围内
        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);

        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

std::vector<char> TriangleFunc::readFile(const std::string &filename)
{
    // 以二进制 + 光标定位到末尾（以便直接获取文件大小）
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("shader文件打开失败!");
    }

    // 获取文件大小（光标当前位置即为文件末尾）
    size_t fileSize = static_cast<size_t>(file.tellg());

    // 分配缓冲区并读取文件内容
    std::vector<char> buffer(fileSize);
    file.seekg(0);                         // 重置读指针到文件开头
    file.read(buffer.data(), fileSize);    // 读取所有内容到 buffer

    file.close();
    return buffer;
}

VkShaderModule TriangleFunc::createShaderModule(const std::vector<char> &code)
{
    // 设置着色器模块创建信息
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();                                     // 字节大小
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());    // 注意对齐

    VkShaderModule shaderModule;

    // 创建着色器模块
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void TriangleFunc::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // 命令缓冲开始录制
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // 配置渲染通道开始信息（使用当前帧的帧缓冲）
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass;
    renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapChainExtent;

    // 设置清除颜色（RGBA = 黑色不透明）
    VkClearValue clearColor = {{{_backColor.x, _backColor.y, _backColor.z, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    // 开始渲染过程（子通道执行方式：inline）
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // 绑定图形管线（必须在 render pass 内部绑定）
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

    // 设置视口（与交换链大小一致）
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_swapChainExtent.width);
    viewport.height = static_cast<float>(_swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // 设置裁剪矩形（与视口一致）
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // 发出绘制三角形命令（顶点数 = 3，实例数 = 1）
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // ImGui 渲染命令，必须在 RenderPass 内调用！
    renderImGui(commandBuffer);

    // 结束渲染过程
    vkCmdEndRenderPass(commandBuffer);

    // 命令缓冲结束录制
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void TriangleFunc::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<TriangleFunc *>(glfwGetWindowUserPointer(window));    // 获取绑定的应用实例指针
    app->_framebufferResized = true;    // 标记帧缓冲已被调整大小，延迟处理重建交换链
}

void TriangleFunc::recreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(_window, &width, &height);

    // 当窗口被最小化时（宽或高为0），等待直到用户恢复窗口
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(_window, &width, &height);
        glfwWaitEvents();    // 等待事件（例如窗口恢复）
    }

    vkDeviceWaitIdle(_device);    // 确保 GPU 不再使用旧的交换链资源

    cleanupSwapChain();    // 销毁旧的交换链相关资源

    createSwapChain();       // 重新创建交换链
    createImageViews();      // 重新创建图像视图
    createFramebuffers();    // 重新创建帧缓冲
}

void TriangleFunc::cleanupSwapChain()
{
    // 销毁交换链中所有的帧缓冲对象，释放相关显存资源
    for (auto framebuffer : _swapChainFramebuffers) {
        vkDestroyFramebuffer(_device, framebuffer, nullptr);
    }
    _swapChainFramebuffers.clear();    // 清空帧缓冲列表

    // 销毁交换链中所有的图像视图，释放对应的图像资源引用
    for (auto imageView : _swapChainImageViews) {
        vkDestroyImageView(_device, imageView, nullptr);
    }
    _swapChainImageViews.clear();    // 清空图像视图列表

    // 销毁交换链对象本身，释放交换链占用的资源
    vkDestroySwapchainKHR(_device, _swapChain, nullptr);
    _swapChain = VK_NULL_HANDLE;    // 标记交换链为空，避免误用
}

bool TriangleFunc::checkValidationLayerSupport()
{
    // 获取系统支持的验证层数量
    uint32_t layerCount;
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (result != VK_SUCCESS) {
        std::cerr << "获取Vulkan 实例验证层数量错误!" << std::endl;
        return {};
    }

    // 获取系统支持的验证层信息
    std::vector<VkLayerProperties> availableLayers(layerCount);
    result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    if (result != VK_SUCCESS) {
        std::cerr << "获取Vulkan 实例验证层信息错误!" << std::endl;
        return {};
    }

    // 检查列表中是否存在所有层
    for (const char *layerName : _validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}

std::vector<const char *> TriangleFunc::getRequiredExtensions()
{
    // 获取glfw窗口扩展
    const char **glfwExtensions;
    uint32_t glfwExtensionCount = 0;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // 如果启用了验证层
    if (_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void TriangleFunc::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkResult TriangleFunc::createDebugUtilsMessengerEXT(VkInstance instance,
                                                    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                    const VkAllocationCallbacks *pAllocator,
                                                    VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void TriangleFunc::destroyDebugUtilsMessengerEXT(VkInstance instance,
                                                 VkDebugUtilsMessengerEXT debugMessenger,
                                                 const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL TriangleFunc::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                           const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                           void *pUserData)
{
    std::cerr << "验证层: " << pCallbackData->pMessage << std::endl;

    // 重要消息,需要提醒
    //  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    //     // Message is important enough to show
    // }

    return VK_FALSE;
}

void TriangleFunc::createImGuiDescriptorPool()
{
    // 定义一个数组，指定各种类型的描述符及其数量，满足 ImGui 在 Vulkan 中的资源绑定需求
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    // 描述符池创建信息结构体
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // 允许单个描述符集的释放
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // 描述符池可容纳的最大描述符集数量，等于类型数量乘以每类型数量
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    // 描述符类型数量
    pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
    // 指向描述符类型数组
    pool_info.pPoolSizes = pool_sizes;

    // 创建 Vulkan 描述符池，失败时抛出异常
    if (vkCreateDescriptorPool(_device, &pool_info, nullptr, &_imguiDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create ImGui descriptor pool!");
    }
}

void TriangleFunc::check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS) {
        return;    // 成功，无需处理
    }

    // 打印错误信息，方便调试
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);

    // 严重错误时终止程序执行
    if (err < 0) {
        abort();
    }
}

VkCommandBuffer TriangleFunc::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // 标记该命令缓冲为一次性提交，便于驱动优化
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void TriangleFunc::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    // 结束命令缓冲录制
    vkEndCommandBuffer(commandBuffer);

    // 提交命令缓冲信息
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // 将命令缓冲提交到图形队列执行
    vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    // 阻塞等待队列执行完成，确保命令缓冲执行完毕
    vkQueueWaitIdle(_graphicsQueue);

    // 释放命令缓冲，归还命令池
    vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
}

void TriangleFunc::renderImGui(VkCommandBuffer cmdBuf)
{
    // 开始新一帧 ImGui 渲染准备（Vulkan + GLFW）
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 创建一个示例窗口和控件
    ImGui::Begin(u8"控制窗口!");
    ImGui::ColorEdit3(u8"背景色", (float *)&_backColor);    // 颜色编辑器，绑定自定义清屏颜色变量
    ImGui::End();

    // 结束 ImGui 帧，并生成绘制数据
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();

    // 使用 Vulkan 命令缓冲执行 ImGui 绘制命令
    ImGui_ImplVulkan_RenderDrawData(draw_data, cmdBuf);
}