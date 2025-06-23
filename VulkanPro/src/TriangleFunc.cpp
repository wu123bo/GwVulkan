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
    mainLoop();
    cleanup();
}

void TriangleFunc::initWindow()
{
    //��ʼ��GLFW ��
    glfwInit();

    //����OpenGl API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //���ɵ������ڴ�С
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //����GLFW����
    _window = glfwCreateWindow(_width, _height, "Vulkan", nullptr, nullptr);
}

void TriangleFunc::initVulkan()
{
    //���� Vulkan ʵ����createInstance��
    createInstance();

    //���õ�����Ϣ�ص���setupDebugMessenger��
    setupDebugMessenger();

    //�������ڱ��棨createSurface��
    createSurface();

    //ѡ����ʵ������豸��GPU����pickPhysicalDevice��
    pickPhysicalDevice();

    //�����߼��豸�����У�createLogicalDevice��
    createLogicalDevice();

    //������������createSwapChain��
    createSwapChain();

    //����������ͼ����ͼ��createImageViews��
    createImageViews();

    //������Ⱦͨ����createRenderPass��
    createRenderPass();

    //����ͼ�ι��ߣ�createGraphicsPipeline��
    createGraphicsPipeline();

    //����֡���壨createFramebuffers��
    createFramebuffers();

    // ��������أ�createCommandPool��
    createCommandPool();

    // �����������
    createCommandBuffer();

    // ��������֡ͬ�����ź�����դ��
    createSyncObjects();
}

void TriangleFunc::mainLoop()
{
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        drawFrame();
    }
}

void TriangleFunc::cleanup()
{
    // �ȴ� GPU ������в���
    vkDeviceWaitIdle(_device);

    // ����ͬ���ź������ͷ���Դ
    vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);

    // ������Ⱦ����ź������ͷ���Դ
    vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);

    // ����֡���դ�����ͷ���Դ
    vkDestroyFence(_device, _inFlightFence, nullptr);

    // ��������أ��ͷ�����������ط�����������
    vkDestroyCommandPool(_device, _commandPool, nullptr);

    // ����ÿһ��֡���������ͼ����ͼ����Ⱦͨ���󶨣�
    for (auto framebuffer : _swapChainFramebuffers) {
        vkDestroyFramebuffer(_device, framebuffer, nullptr);
    }

    // ����ͼ�ι���
    vkDestroyPipeline(_device, _graphicsPipeline, nullptr);

    // ���ٹ��߲��֣�����Դ���֣�
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);

    // ������Ⱦͨ����Render Pass��
    vkDestroyRenderPass(_device, _renderPass, nullptr);

    // �������н�����ͼ����ͼ��ImageView��
    for (auto imageView : _swapChainImageViews) {
        vkDestroyImageView(_device, imageView, nullptr);
    }

    // ���ٽ�����
    vkDestroySwapchainKHR(_device, _swapChain, nullptr);

    // �����߼��豸
    vkDestroyDevice(_device, nullptr);

    // ��������˵��Բ㣬�����ٵ��Իص����
    if (_enableValidationLayers) {
        destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
    }

    // �����봰��ϵͳ�����ı���
    vkDestroySurfaceKHR(_instance, _surface, nullptr);

    // ���� Vulkan ʵ��
    vkDestroyInstance(_instance, nullptr);

    // ���� GLFW �����Ĵ��ڣ����ͷ� GLFW ��ռ��Դ
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void TriangleFunc::createInstance()
{
    PrintVec("֧�ֵ� Vulkan ʵ����չ:", checkValidationInstanceExtensions());

    //������֤��
    if (_enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("��������֤�㣬��������!");
    }

    // vk���򴴽���Ϣ�ṹ
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "���,С����";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // vk����ʵ����Ϣ�ṹ
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    //���ô���ʵ����Ҫ���õ���չ(���細����չ)
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    //��֤�㿪��ʱע����Իص���Ϣ
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (_enableValidationLayers) {
        //���ô���ʵ����Ϣ����֤��(ֻ����Debugģʽ��)
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    //����vulkanʵ��(2:ָ���Զ���������ص���ָ��,nullptr����)
    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("����vkʵ��ʧ��!");
    }
}

void TriangleFunc::setupDebugMessenger()
{
    if (!_enableValidationLayers) {
        return;
    }

    //���Իص��ṹ��Ϣ��Debug Messenger��
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    //���õ�����Ϣ������
    if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("�޷����õ�����Ϣ������!");
    }
}

void TriangleFunc::pickPhysicalDevice()
{
    //��ȡϵͳ�����豸����(GPU)
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("δ���ҵ�֧��Vulkan��GPU!");
    }

    //��ȡϵͳ�����豸��Ϣ
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    // ��ѡ�豸���÷֣����߷���������
    std::multimap<int, VkPhysicalDevice, std::greater<int>> candidates;
    for (const auto &device : devices) {
        int score = rateDeviceSuitability(device);
        if (score > 0) {
            candidates.insert(std::make_pair(score, device));
        }
    }

    // ѡ��÷���ߵ��豸��ΪĿ�� GPU
    if (!candidates.empty()) {
        _physicalDevice = candidates.begin()->second;
    } else {
        throw std::runtime_error("δ���ҵ�����Ҫ��� Vulkan �����豸��");
    }
}

void TriangleFunc::createLogicalDevice()
{
    // ���ҵ�ǰ�����豸֧�ֵĶ����壨ͼ�� + ���֣�
    QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

    // ʹ�� std::set ȥ�أ�ȷ�������ظ�������ͬ������
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // Ϊÿ��Ψһ�����崴��һ�� VkDeviceQueueCreateInfo
    float queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        //�߼����д�����Ϣ
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // ����Ҫ���õ������豸���ԣ��˴���δ�����κ����ԣ�
    VkPhysicalDeviceFeatures deviceFeatures{};

    // �߼��豸������Ϣ
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    // �����豸��չ���� swapchain ���ڴ����ã�
    createInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = _deviceExtensions.data();

    // ����������֤�㣬�򸽼Ӳ�����
    if (_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
        createInfo.ppEnabledLayerNames = _validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("δ�ܴ����߼��豸!");
    }

    // ��ȡͼ�ζ��о��
    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);

    // ��ȡ���ֶ��о��
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
}

void TriangleFunc::createSurface()
{
    if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) {
        throw std::runtime_error("δ�ܴ������ڱ���!");
    }
}

void TriangleFunc::createSwapChain()
{
    // ��ѯ������֧������
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);

    // ѡ������ʽ����ɫ��ʽ + ɫ�ʿռ䣩
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

    // ѡ�����ģʽ���� FIFO��MAILBOX��
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);

    // ѡ�񽻻���ͼ��ߴ磨�ֱ��ʣ�
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // ������ͼ������������Ϊ��С���� + 1���������ܣ�
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // ������ͼ�����������ƣ�ȷ��������������
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // ��д������������Ϣ�ṹ��
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;    // һ��Ϊ1����������������ͼ��Ⱦ
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // ��ȡͼ�κͳ��ֶ���������
    QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // ���ͼ�ζ�����ͳ��ֶ����岻ͬ�����ù���ģʽΪ����
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        // ͬһ��������ʹ�ö�ռģʽ������Ч��
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;        // ����
        createInfo.pQueueFamilyIndices = nullptr;    // ����
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;    // ͼ��任��һ���ޱ任��
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;               // ��͸���� alpha �ϳ�
    createInfo.presentMode = presentMode;                                        // ѡ��ĳ���ģʽ
    createInfo.clipped = VK_TRUE;                                                // �޳����ɼ����أ���������

    createInfo.oldSwapchain = VK_NULL_HANDLE;    // �ɽ����������ڴ�С���ʱ�ã�

    // ����������
    if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // ��ȡ������ͼ��������
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
    _swapChainImages.resize(imageCount);

    // ��ȡ������ͼ����
    vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

    // ��¼������ͼ���ʽ�ͳߴ磬��������ͼ����ͼʱʹ��
    _swapChainImageFormat = surfaceFormat.format;
    _swapChainExtent = extent;
}

void TriangleFunc::createImageViews()
{
    // ����ͼ����������ͼ����ͼ����
    _swapChainImageViews.resize(_swapChainImages.size());

    // Ϊÿ��������ͼ�񴴽�ͼ����ͼ
    for (size_t i = 0; i < _swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = _swapChainImages[i];         // ָ��ͼ����Դ
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;    // 2D ��ͼ
        createInfo.format = _swapChainImageFormat;      // ͼ���ʽ�����뽻����һ��

        // ͨ����ӳ�䣨�˴�ΪĬ�ϣ����ֲ��䣩
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // ͼ������Դ��Χ������ֻʹ����ɫ�����㣬�� mipmap��
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        // ����ͼ����ͼ���洢
        if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void TriangleFunc::createGraphicsPipeline()
{
    // === 1. ���� shader �ֽ����ļ� ===
    auto vertShaderCode = readFile("spv/vert.spv");
    auto fragShaderCode = readFile("spv/frag.spv");

    // === 2. ���� shader ģ�� ===
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // === 3. ���� shader �׶� ===
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";    // ��ɫ����ں���

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    // === 4. ��������׶Σ���ǰ�޶������ݣ�===
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    // === 5. ����װ�䣨��װΪ�����Σ�===
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // === 6. �ӿ���ü����ã����ö�̬���ã�===
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // === 7. ��դ���׶� ===
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // === 8. ���ز������رգ�===
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // === 9. ��ɫ��ϣ�ֱ�������ɫ��������ϣ�===
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

    // === 10. ��̬״̬���ӿ�/������ command buffer ���ã�===
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // === 11. �������߲��֣��� descriptor set��===
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // === 12. �������ͼ�ι������нṹ�� ===
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

    // === 13. ������ʱ��ɫ��ģ�飨�����Ѹ��������ݣ�===
    vkDestroyShaderModule(_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(_device, vertShaderModule, nullptr);
}

void TriangleFunc::createRenderPass()
{
    // 1. ����������������ͼ��
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChainImageFormat;                     // �뽻������ʽһ��
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;                    // �޶��ز���
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;               // ��Ⱦǰ�����ɫ
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;             // ��Ⱦ�������ݣ�������ʾ��
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;    // ��ʹ�� stencil
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;        // ��ʼʱ������ͼ������
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;    // ���ֵ���Ļ

    // 2. �������ã����� subpass��
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;    // ָ�������Ǹ�����
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // 3. ��ͨ�����ã���ͼ�ι��ߣ�д����ɫ������
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;    // ͼ�ι���
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // 4. ��Ⱦͨ��������Ϣ
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // 5. ������Ⱦͨ������
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void TriangleFunc::createFramebuffers()
{
    // ����֡����������С����ͼ����ͼ����һ��
    _swapChainFramebuffers.resize(_swapChainImageViews.size());

    for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
        // ÿ�� framebuffer �ĸ���������һ����ɫͼ����ͼ
        VkImageView attachments[] = {_swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = _renderPass;    // ʹ��ͬһ����Ⱦͨ��
        framebufferInfo.attachmentCount = 1;         // ����һ��ͼ����ͼ
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = _swapChainExtent.width;
        framebufferInfo.height = _swapChainExtent.height;
        framebufferInfo.layers = 1;    // ��ʹ������ͼ��������

        // ���� framebuffer ����
        if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void TriangleFunc::createCommandPool()
{
    // ���ҵ�ǰ�����豸��֧��ͼ��ָ��Ķ���������
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_physicalDevice);

    // ��������ش�����Ϣ
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;         // ���������������
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();    // ��ͼ�ζ�����

    // ��������ض���
    if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void TriangleFunc::createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;                 // ���������
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;    // ���������
    allocInfo.commandBufferCount = 1;                     // ����һ��

    // ��������岢�����Ա����
    if (vkAllocateCommandBuffers(_device, &allocInfo, &_commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void TriangleFunc::createSyncObjects()
{
    // �ź���������Ϣ
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // դ��������Ϣ����ʼΪ signaled ״̬����ֹ�״εȴ�����
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // ����ͼ������ź�������Ⱦ����ź�����֡��դ��
    if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS
        || vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS
        || vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void TriangleFunc::drawFrame()
{
    // �ȴ���һִ֡����ɣ���ֹ GPU ����ʹ��ͬһ������ͼ����Դ
    vkWaitForFences(_device, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(_device, 1, &_inFlightFence);

    // �ӽ������л�ȡ��ǰ֡ʹ�õ�ͼ�񣨻��ź������� image ���ã�
    uint32_t imageIndex;
    vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    // ���ò�����¼������壨�󶨵�ǰ֡������Դ��
    vkResetCommandBuffer(_commandBuffer, 0);
    recordCommandBuffer(_commandBuffer, imageIndex);

    // �ύ�������ͼ�ζ���
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // �ȴ�ͼ������ź������ٿ�ʼִ�л�������
    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};    // �������ɫ�׶εȴ�
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    // Ҫ�ύ�������
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    // ��Ⱦ��ɺ󴥷����ź���
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // �ύ������� _inFlightFence ���������
    if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // ���ֽ׶Σ���ͼ�����뽻��������ʾ����Ļ
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;    // �ȴ���Ⱦ����ź�
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(_presentQueue, &presentInfo);    // �ύ�����ֶ���
}

std::vector<std::string> TriangleFunc::checkValidationInstanceExtensions()
{
    //��ȡ Vulkan ʵ����չ����
    uint32_t extensionCount = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS) {
        std::cerr << "��ȡVulkan ʵ����չ��������!" << std::endl;
        return {};
    }

    //��ȡ Vulkan ʵ����չ��Ϣ
    std::vector<VkExtensionProperties> extensions(extensionCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    if (result != VK_SUCCESS) {
        std::cerr << "��ȡVulkan ʵ����չ��Ϣ����!" << std::endl;
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
    // ��ѯ�豸֧�ֵ���չ����
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    // ��ȡ���п�����չ����
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    // ��������չתΪ�ɲ����ļ���
    std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

    // �������п�����չ���޳����������չ
    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    // �������Ϊ�գ�˵��������չ��������
    return requiredExtensions.empty();
}

int TriangleFunc::rateDeviceSuitability(VkPhysicalDevice device)
{
    if (!isDeviceSuitable(device)) {
        return 0;
    }

    //��ȡָ�������豸�� ���ԣ���� VkPhysicalDeviceProperties �ṹ��
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    //��ȡ�������豸֧�ֵ� �������ԣ��磺������ɫ����tessellation �ȣ���
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    // ����ʹ�ö����Կ���Discrete GPU��
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // ֧�ֵ���� 2D ����ߴ�Խ��ͼ������Ǳ��Խ��
    score += deviceProperties.limits.maxImageDimension2D;

    // ��֧�ּ�����ɫ�����豸ֱ����̭
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    PrintPhysicalDev(deviceProperties, score);
    return score;
}

QueueFamilyIndices TriangleFunc::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    //��ѯ�����豸֧�ֵĶ���������
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        throw std::runtime_error("δ���ҵ���ǰ�����豸�Ķ�����!");
    }

    //��ȡ�����������б�
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {
        // ����Ƿ�֧��ͼ��ָ��
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // ����Ƿ�֧�ֽ�ͼ����ֵ�ָ�� surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        // ������ҵ��ˣ���ǰ��������
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

    // ��ѯ��������������Сͼ����������ǰͼ��ߴ�ȣ�
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

    // ��ѯ֧�ֵı����ʽ����
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

    // ����и�ʽ����ȡ��ʽ�б�
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
    }

    // ��ѯ֧�ֵĳ���ģʽ����
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

    // �����ģʽ����ȡģʽ�б�
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

    // ���˲��ԣ�ʹ�õ�һ�����ø�ʽ
    return availableFormats[0];
}

VkPresentModeKHR TriangleFunc::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // ���� Vulkan �豸������֧�� FIFO ģʽ
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D TriangleFunc::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    // ��� currentExtent ��Ϊ����ֵ��˵��ƽ̨��ָ����С����ĳЩ����ϵͳ��
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        // �Ӵ����л�ȡ֡����ʵ�ʳߴ磨��λ�����أ�
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        // ����ͼ���С���豸֧�ֵķ�Χ��
        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);

        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

std::vector<char> TriangleFunc::readFile(const std::string &filename)
{
    // �Զ����� + ��궨λ��ĩβ���Ա�ֱ�ӻ�ȡ�ļ���С��
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("shader�ļ���ʧ��!");
    }

    // ��ȡ�ļ���С����굱ǰλ�ü�Ϊ�ļ�ĩβ��
    size_t fileSize = static_cast<size_t>(file.tellg());

    // ���仺��������ȡ�ļ�����
    std::vector<char> buffer(fileSize);
    file.seekg(0);                         // ���ö�ָ�뵽�ļ���ͷ
    file.read(buffer.data(), fileSize);    // ��ȡ�������ݵ� buffer

    file.close();
    return buffer;
}

VkShaderModule TriangleFunc::createShaderModule(const std::vector<char> &code)
{
    // ������ɫ��ģ�鴴����Ϣ
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();                                     // �ֽڴ�С
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());    // ע�����

    VkShaderModule shaderModule;

    // ������ɫ��ģ��
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void TriangleFunc::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // ����忪ʼ¼��
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // ������Ⱦͨ����ʼ��Ϣ��ʹ�õ�ǰ֡��֡���壩
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass;
    renderPassInfo.framebuffer = _swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapChainExtent;

    // ���������ɫ��RGBA = ��ɫ��͸����
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    // ��ʼ��Ⱦ���̣���ͨ��ִ�з�ʽ��inline��
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // ��ͼ�ι��ߣ������� render pass �ڲ��󶨣�
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

    // �����ӿڣ��뽻������Сһ�£�
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_swapChainExtent.width);
    viewport.height = static_cast<float>(_swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // ���òü����Σ����ӿ�һ�£�
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // ����������������������� = 3��ʵ���� = 1��
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // ������Ⱦ����
    vkCmdEndRenderPass(commandBuffer);

    // ��������¼��
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

bool TriangleFunc::checkValidationLayerSupport()
{
    //��ȡϵͳ֧�ֵ���֤������
    uint32_t layerCount;
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (result != VK_SUCCESS) {
        std::cerr << "��ȡVulkan ʵ����֤����������!" << std::endl;
        return {};
    }

    //��ȡϵͳ֧�ֵ���֤����Ϣ
    std::vector<VkLayerProperties> availableLayers(layerCount);
    result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    if (result != VK_SUCCESS) {
        std::cerr << "��ȡVulkan ʵ����֤����Ϣ����!" << std::endl;
        return {};
    }

    //����б����Ƿ�������в�
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
    //��ȡglfw������չ
    const char **glfwExtensions;
    uint32_t glfwExtensionCount = 0;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    //�����������֤��
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
    std::cerr << "��֤��: " << pCallbackData->pMessage << std::endl;

    //��Ҫ��Ϣ,��Ҫ����
    // if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    //    // Message is important enough to show
    //}

    return VK_FALSE;
}
