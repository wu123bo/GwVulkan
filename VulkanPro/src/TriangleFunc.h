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
     * @brief ��ʼ������ϵͳ���� GLFW����
     *
     * �������� Vulkan ��Ⱦ�Ĵ��ھ����
     */
    void initWindow();

    /**
     * @brief ��ʼ�� ImGui���� Vulkan �� GLFW�����ں������ƽ��档
     */
    void initImgui();

    /**
     * @brief ��ʼ�� Vulkan ��ض���
     */
    void initVulkan();

    /**
     * @brief ��ѭ����
     *
     * �������¼������ֳ������У�ֱ���û��رմ��ڡ�
     */
    void mainLoop();

    /**
     * @brief ���� Vulkan ʹ�ù����з����������Դ��
     *
     * ������Ӧ�ڳ����˳�ǰ���ã�ȷ������ Vulkan ������ȷ�����Ա�����Դй¶��
     * ����˳����ѭ������ϵ������ײ���Դ����߲�ӿ����ͷš�
     */
    void cleanup();

private:
    /**
     * @brief ���� Vulkan ʵ����VkInstance����
     *
     * ����Ӧ����Ϣ��������չ����֤��ȣ������� vkCreateInstance��
     */
    void createInstance();

    /**
     * @brief ���� Vulkan ��֤����Իص���Debug Messenger����
     *
     * ������֤�����������ڿ����׶����ڲ��� API ʹ�ô���
     */
    void setupDebugMessenger();

    /**
     * @brief ��ϵͳ���� Vulkan �����豸��ѡ��һ�����ʵ��豸��
     *
     * ѡ���׼ʾ����֧�ֱ���Ķ��������չ������������Ⱦ�����ͼ���������豸��
     */
    void pickPhysicalDevice();

    /**
     * @brief �����߼��豸����ȡ�����ͼ������ֶ��С�
     *
     * ���ڵ�ǰ�����豸��ʼ�� VkDevice������ȡ���о����
     *
     */
    void createLogicalDevice();

    /**
     * @brief ���� Vulkan �봰��ϵͳ�����ı��档
     *
     * ʹ�� GLFW ���� VkSurfaceKHR������ͼ����֡�
     *
     */
    void createSurface();

    /**
     * @brief ���� Vulkan �����������ͼ���ʽ���ߴ�ͳ���ģʽ��ѡ��
     *
     * - ��ѯ�豸֧�ֵĽ�����ϸ�ڡ�
     * - ѡ����ʵı����ʽ������ģʽ�ͽ�����ͼ��ߴ硣
     * - ��������������VkSwapchainKHR����
     * - ��ȡ������������ͼ������
     *
     * @throws std::runtime_error ����������ʧ��ʱ�׳���
     */
    void createSwapChain();

    /**
     * @brief Ϊ�������е�ÿ��ͼ�񴴽���Ӧ��ͼ����ͼ��VkImageView����
     *
     * ͼ����ͼ������ͼ���ʹ�÷�ʽ���� 2D ��ɫ��������
     * �Ǻ�������֡���塢��ȾĿ��Ļ�����
     *
     * @throws std::runtime_error ��������һ����ͼʧ��ʱ�׳��쳣��
     */
    void createImageViews();

    /**
     * @brief ���� Vulkan ͼ�ι��ߣ�Graphics Pipeline����
     *
     * ͼ�ι��߽�������ɫ����Ƭ����ɫ��������̶����ܽ׶������һ��
     * ����������Ⱦ�����л��Ƽ���ͼԪ��
     *
     * ������ִ�У�
     * - ��ɫ��ģ�����
     * - �̶����ܽ׶����ã��������롢�ӿڡ�װ�䡢��դ������ɫ�ȣ�
     * - ���߲��ִ���
     * - ���� vkCreateGraphicsPipelines ����ͼ�ι��߶���
     *
     * @throws std::runtime_error �������ʧ��
     */
    void createGraphicsPipeline();

    /**
     * @brief ���� Vulkan ��Ⱦͨ����Render Pass����
     *
     * ��Ⱦͨ��������һ����Ⱦ��ʹ�õĸ�������ͨ���ṹ���Լ����ǵ�������ϵ��
     * ��ǰֻ������һ����ɫ�������ڽ�����ͼ��������
     *
     * @throws std::runtime_error ���������Ⱦͨ��ʧ�ܡ�
     */
    void createRenderPass();

    /**
     * @brief Ϊ�������е�ÿһ��ͼ����ͼ����֡�������Framebuffer����
     *
     * ÿ��֡������һ��ͼ����ͼ�󶨣�������ָ������Ⱦͨ����_renderPass���н�����Ⱦ�����
     * ͨ��ÿ֡��Ⱦ��Ӧһ��ͼ����ͼ������贴�����֡���壬�뽻����ͼ��һһ��Ӧ��
     *
     * @throws std::runtime_error ���֡���崴��ʧ�ܡ�
     */
    void createFramebuffers();

    /**
     * @brief ��������أ�Command Pool�������ڷ������������
     *
     * ����ظ�������������ڴ���䣬����ͼ�������ͨ���óط��䡣
     * �˴��󶨵�ͼ�ζ����壬������Ⱦ���������Ҳ���Ӵ�����ط��䡣
     *
     * ���� VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT ��־���������õ�������塣
     *
     * @throws std::runtime_error �������ش���ʧ�ܡ�
     */
    void createCommandPool();

    /**
     * @brief Ϊÿһ֡����������壨Primary Command Buffers����
     *
     * Vulkan �е���������ڼ�¼ GPU Ҫִ�е���Ⱦָ�
     * ���ڲ����˶�֡��������˫����������壩��ÿһ֡����Ҫһ������������塣
     *
     * - ʹ�� _commandPool ��������塣
     * - ����弶��Ϊ VK_COMMAND_BUFFER_LEVEL_PRIMARY����ʾ��ֱ���ύ�����С�
     * - ������������ _MAX_FRAMES_IN_FLIGHT������֧�ֶ�֡������
     *
     * @throws std::runtime_error �����������ʧ�ܡ�
     */
    void createCommandBuffers();

    /**
     * @brief ����ÿ֡�����ͬ�������ź�����դ������
     *
     * Vulkan ʹ��ͬ��������Э�� CPU �� GPU ֮���ִ��˳�򣬱�����Դ��ͻ��
     * ������Ϊÿ֡������ͨ��Ϊ 2 �� 3 ֡����������ͬ������
     * - ͼ������ź�����_imageAvailableSemaphores������ͼ���ȡ���ʱ��������
     * - ��Ⱦ����ź�����_renderFinishedSemaphores�����������ִ�����ʱ������������ʹ�á�
     * - CPU-GPU դ����_inFlightFences����ȷ��ÿ֡��ʼʱ��һ֡��������ִ����ϡ�
     *
     * �����ź�������Ϊ��ʼδ����״̬��դ������Ϊ��ʼ���Ѵ��������������һ֡������ʼ��Ⱦ��
     *
     * @throws std::runtime_error �����һͬ�����󴴽�ʧ�ܡ�
     */
    void createSyncObjects();

    /**
     * @brief ÿ֡��Ⱦ�߼���Frame Rendering����
     *
     * ������ʵ���� Vulkan �е�֡��Ⱦ���̣����ö�֡������Frames in Flight�����ƣ�
     * ʹ�ö���ź�����semaphores����դ����fences������ CPU �� GPU ͬ��������
     *
     * ��Ⱦ���̸�����
     * 1. �ȴ���ǰ֡��դ����ȷ����һ֡ʹ�õ���Դ�Ѿ����ã�
     * 2. �ӽ������л�ȡһ�ſ���ͼ��
     * 3. ��������岢��¼�µ���Ⱦ���
     * 4. �ύ��Ⱦ���ͼ�ζ��У�ʹ���ź���ͬ�� GPU ��Ⱦ���̣�
     * 5. ����Ⱦ��ɵ�ͼ���ύ�����ֶ��н�����ʾ��
     * 6. ���µ�ǰ֡������ʵ��ѭ��֡����������Դ��ͻ����
     *
     * ע�⣺
     * - ��֡����ͨ������Ϊ 2 �� 3 ֡��
     * - ʹ���ź���ȷ��ͼ���ȡ����Ⱦ˳��
     * - ʹ��դ��ȷ�������������ǰ���ᱻ GPU ʹ���С�
     */
    void drawFrame();

private:
    /**
     * @brief ��ȡ��ǰϵͳ֧�ֵ� Vulkan ʵ����չ�б�
     *
     * ���ڲ�ѯ���п��õ� Vulkan ʵ����չ���ƣ�
     * ��������֤ GLFW ����Թ����������չ�Ƿ�֧�֡�
     *
     * @return std::vector<std::string> ֧�ֵ���չ�����б�
     */
    std::vector<std::string> checkValidationInstanceExtensions();

private:
    /**
     * @brief �жϸ����������豸�Ƿ��������Ҫ��
     *
     * ��������Ƿ�֧��ͼ�ζ����塢��Ҫ���豸��չ���� swapchain��������֧�ֵȡ�
     *
     * @param device �������� Vulkan �����豸��VkPhysicalDevice����
     * @return true  ���豸�������������������ڴ����߼��豸��
     * @return false �豸������Ҫ����������
     */
    bool isDeviceSuitable(VkPhysicalDevice device);

    /**
     * @brief ��������豸�Ƿ�֧������������豸��չ��
     *
     * �������ж��豸�Ƿ�֧�� VK_KHR_swapchain �ȹؼ���չ��
     *
     * @param device Ҫ���� Vulkan �����豸��
     * @return true  �豸֧�����б�����չ��
     * @return false ����ȱ��һ����չ��
     */
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    /**
     * @brief ���� Vulkan �����豸�������Բ���֡�
     *
     * ���ڴӶ���豸��ѡ����� GPU��
     *
     * @param device Ҫ�����������豸��
     * @return int �÷֣��߷�����ѡ�񣩡�
     */
    int rateDeviceSuitability(VkPhysicalDevice device);

    /**
     * @brief ����ָ�������豸֧�ֵ�ͼ�ζ���������ֶ����塣
     *
     * �����豸֧�ֵ����ж����壬�����Ƿ�֧�֣�
     * - ͼ��ָ�VK_QUEUE_GRAPHICS_BIT��
     * - ָ�� surface ��ͼ�����֧�֣�vkGetPhysicalDeviceSurfaceSupportKHR��
     *
     * @param device Ҫ��ѯ�� Vulkan �����豸��
     * @return QueueFamilyIndices ����ͼ�ζ��кͳ��ֶ��е�����������ҵ�����
     * @throws std::runtime_error �������������Ϊ 0��
     */
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

private:
    /**
     * @brief ��ѯָ�������豸�Ե�ǰ���ڱ��棨_surface���Ľ�����֧����Ϣ��
     *
     * ���������������ֱ��ʷ�Χ��ͼ���������ƣ���֧�ֵı����ʽ�ͳ���ģʽ��
     *
     * @param device Ҫ��ѯ�� Vulkan �����豸��
     * @return SwapChainSupportDetails ����֧����Ϣ�Ľṹ�塣
     */
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    /**
     * @brief ѡ�񽻻��������ʽ����ɫ��ʽ + ɫ�ʿռ䣩��
     *
     * ����ѡ�� VK_FORMAT_B8G8R8A8_SRGB + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR��
     * ��δ�ҵ��������ʹ�õ�һ�����ø�ʽ��
     *
     * @param availableFormats ���õı����ʽ�б�
     * @return VkSurfaceFormatKHR ѡ���ı����ʽ��
     */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    /**
     * @brief ѡ�񽻻����ĳ���ģʽ��֡ͬ����ʽ����
     *
     * ����ѡ�� VK_PRESENT_MODE_MAILBOX_KHR�����ӳ١����ػ��壩��
     * �������ã�����˵� VK_PRESENT_MODE_FIFO_KHR��V-Sync�������豸��֧�֣���
     *
     * @param availablePresentModes ���õĳ���ģʽ�б�
     * @return VkPresentModeKHR ѡ���ĳ���ģʽ��
     */
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    /**
     * @brief ѡ�񽻻���ͼ��ķֱ��ʣ�VkExtent2D����
     *
     * �� `currentExtent` ��Ϊ����ֵ����ʾ���ڴ�С�ѹ̶���ֱ�ӷ��ؼ��ɣ�
     * ����Ӵ���ϵͳ��ȡ֡�����С�����������豸֧�ֵ���С/���Χ�ڡ�
     *
     * @param capabilities ����������Ϣ��VkSurfaceCapabilitiesKHR����
     * @return VkExtent2D �������ڽ�������ͼ��ߴ硣
     */
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

private:
    /**
     * @brief ��ȡָ���ļ���ȫ�����ݣ�ͨ�����ڼ��� SPIR-V ��ɫ���ֽ��룩��
     *
     * ʹ�ö����Ʒ�ʽ���ļ�������ȡ�������ݵ��������з��ء�
     *
     * @param filename �ļ�·������Ի����·������
     * @return std::vector<char> �ļ���ԭʼ�ֽ����ݡ�
     *
     * @throws std::runtime_error ����ļ���ʧ�ܡ�
     */
    static std::vector<char> readFile(const std::string &filename);

    /**
     * @brief ������ɫ��ģ�飨VkShaderModule����
     *
     * ��Ԥ����� SPIR-V �ֽ��루.spv �ļ������� Vulkan ��ɫ��ģ�飬
     * ��ģ������ں�������ͼ�ι��ߣ��綥����ɫ����Ƭ����ɫ������
     *
     * @param code ���� SPIR-V �ֽ���Ķ��������ݣ�ͨ��ͨ���ļ���ȡ�õ�����
     * @return VkShaderModule �����ɹ�����ɫ��ģ������
     *
     * @throws std::runtime_error �����ɫ��ģ�鴴��ʧ�ܡ�
     */
    VkShaderModule createShaderModule(const std::vector<char> &code);

private:
    /**
     * @brief ¼��ͼ�����ָ����������С�
     *
     * �ú���������ָ������Ⱦ���ߡ������ӿڡ����������Σ�д��ָ��������������У�
     * ���ں����ύ��ͼ�ζ���ִ�С�ÿ֡ͼ����Ҫһ�ݶ�Ӧ�Ļ������
     *
     * @param commandBuffer Ҫд������� VkCommandBuffer��ͨ��Ϊ������壩��
     * @param imageIndex    ��ǰʹ�õĽ�����ͼ������������ѡ���Ӧ��֡���塣
     *
     * @throws std::runtime_error ��������¼�ƿ�ʼ�����ʧ�ܡ�
     */
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

private:
    /**
     * @brief ����֡����ߴ�仯ʱ�Ļص�������
     *
     * ���û��������ڴ�С�����϶����ڱ߽磩ʱ��GLFW ����ô˺�����
     * Vulkan ��Ҫ��֡�����С�����仯���ؽ���������Swapchain����
     * ���Դ˴�����һ����־ `_framebufferResized`������Ⱦѭ���м��ñ�־���ؽ������Դ��
     *
     * @param window �����¼��� GLFW ���ھ����
     * @param width �µ�֡��������ȣ����أ���
     * @param height �µ�֡�������߶ȣ����أ���
     */
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    /**
     * @brief �ؽ����������������Դ��
     *
     * �����ڳߴ緢���仯������С�����û��ֶ�������С����
     * ԭ�еĽ������������á��ú����ȴ����ڻָ���Ч��С��
     * ��ʹ�豸���У�Ȼ������ɵĽ�������Դ�������´�����������
     * ͼ����ͼ��֡�������������ڽ���������Դ��
     *
     * ע�⣺��Ҫ�ڻ��ƹ����е��ô˺�����Ӧ�� `vkQueuePresentKHR`
     * ���� `VK_ERROR_OUT_OF_DATE_KHR` �� `VK_SUBOPTIMAL_KHR` ʱ���ã�
     * ��ͨ�����ڴ�С�ص����õı�־λ������
     */
    void recreateSwapChain();

    /**
     * @brief �����뽻������ص���Դ��
     *
     * �˺���������֡�������ͼ����ͼ�ͽ���������
     * �ڴ��ڳߴ�ı�ʱ��������������Щ����Դ��Ȼ���ؽ��µĽ���������������Դ��
     *
     * ע�⣺�˺����������뽻�����޹ص���Դ������Ⱦͨ����ͼ�ι��ߵȣ�
     * ����Щ��Դ�����ڽ������ĸ�ʽ��ߴ磬ҲӦ���ⲿ�����ؽ���
     */
    void cleanupSwapChain();

private:
    /**
     * @brief ��鵱ǰϵͳ�Ƿ�֧��ָ���� Vulkan ��֤�㣨�� VK_LAYER_KHRONOS_validation����
     *
     * �����õ��Թ���ǰ���ô˺���ȷ����֤���Ƿ���á�
     *
     * @return true  ϵͳ֧����֤�㡣
     * @return false ϵͳ��֧����֤�㡣
     */
    bool checkValidationLayerSupport();

    /**
     * @brief ��ȡ Vulkan ʵ�������������չ�б�
     *
     * ���� GLFW Ҫ�����չ����������֤�㣬�򸽼� VK_EXT_debug_utils ��չ��
     *
     * @return std::vector<const char*> ��չ����ָ�����顣
     */
    std::vector<const char *> getRequiredExtensions();

    /**
     * @brief ��� VkDebugUtilsMessengerCreateInfoEXT �ṹ�塣
     *
     * ���ڳ�ʼ��������Ϣ�ص��������������Ϣ�������ͺͻص�����ָ��ȡ�
     * ������ vkCreateDebugUtilsMessengerEXT ����Ϊ vkCreateInstance �� pNext ������
     *
     * @param createInfo  ����ĵ��Իص�������Ϣ�ṹ�壬�������������
     *
     * @note
     *  - �ص�����ʹ��Ĭ�ϵ� debugCallback��
     *  - ������ vkCreateInstance �� pNext����Ҫ��ʵ������ǰ���á�
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    /**
     * @brief ���� Vulkan �ĵ�����Ϣ�ص���Debug Messenger����
     *
     * �˺����� VK_EXT_debug_utils ��չ�ṩ�Ĵ����ӿڣ�
     * ����������ʱ������֤��Ĵ��󡢾�����������ʾ��Ϣ��
     *
     * @param instance       Vulkan ʵ�������
     * @param pCreateInfo    ���Իص�������Ϣ�ṹ��ָ�롣
     * @param pAllocator     �Զ����ڴ����������Ϊ nullptr����
     * @param pDebugMessenger ���ش����ĵ��Իص������
     *
     * @return VkResult      ���������VK_SUCCESS ��ʾ�ɹ�����
     *
     * @note
     *  �˺�������ͨ�� vkGetInstanceProcAddr ��ȡ��Vulkan ��δĬ�ϱ�¶����չ������
     */
    VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);

    /**
     * @brief ���� Vulkan �ĵ�����Ϣ�ص���Debug Messenger����
     *
     * �˺����� VK_EXT_debug_utils ��չ�ṩ�����ٽӿڣ�
     * �����ͷ�ͨ�� vkCreateDebugUtilsMessengerEXT �����ĵ��Ծ����
     *
     * @param instance        Vulkan ʵ�������
     * @param debugMessenger  ��Ҫ���ٵĵ��Իص������
     * @param pAllocator      �Զ����ڴ����������Ϊ nullptr����
     *
     * @note
     *  �˺���Ҳ��ͨ�� vkGetInstanceProcAddr ��̬���ء�
     */
    void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);

    /**
     * @brief Vulkan ��֤����Իص�������
     *
     * ������ VK_EXT_debug_utils ��չ��ע��˺�����
     * ��������ʱ�����֤��ľ��桢��������ʾ��Ϣ��
     *
     * @param messageSeverity ��Ϣ���صȼ����� ERROR/WARNING/INFO����
     * @param messageType     ��Ϣ���ͣ���֤�����ܡ�һ���ԣ���
     * @param pCallbackData   ����������Ϣ�Ľṹ��ָ�롣
     * @param pUserData       �й���Ч�ҿ��ܵ��±�������Ϊ����Ϣ��
     *
     * @return VkBool32       ���� VK_FALSE ����ִ�У�VK_TRUE ���ж� Vulkan ���ã�һ�㷵�� VK_FALSE����
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);

private:
    /**
     * @brief �������� ImGui �� Vulkan �������ء�
     *
     * �������أ�Descriptor Pool���� Vulkan �����ڹ�������������Descriptor Sets���ڴ�Ķ���
     * ImGui �� Vulkan �����Ҫ������ͬ���͵������������������ͼ�񡢻������ȣ�������Դ��
     * �����Ҫ����һ�������ϴ���������أ���֤����Ⱦ�����в���������������䲻������⡣
     *
     * ����ʵ��ϸ�ڣ�
     * - ����һ�� VkDescriptorPoolSize ���飬�о��� ImGui �����õ��ĸ������������ͼ���������
     * - ���� VkDescriptorPoolCreateInfo �ṹ�壬ָ���صĴ���������
     *   - flags ����Ϊ VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT�������ͷŵ�������������
     *   - maxSets �ǳ������������������������Ϊ������������֮�͵ĳ˻���
     *   - poolSizeCount �� pPoolSizes ָ���������������顣
     * - ���� vkCreateDescriptorPool �����������أ�ʧ��ʱ�׳��쳣��
     */
    void createImGuiDescriptorPool();

    /**
     * @brief ��� Vulkan �������õķ��ؽ�������������Ϣ���ڴ���ʱ��ֹ����
     *
     * @param err Vulkan �������ص� VkResult ����롣
     *
     * �������ֵ�� VK_SUCCESS��˵�������ɹ�������ֱ�ӷ��ء�
     * ������ش����룬���ӡ������Ϣ��
     * ����������Ǹ�ֵ����ʾ���ش��󣩣��������� abort() ��ֹ���С�
     */
    static void check_vk_result(VkResult err);

    /**
     * @brief ��ʼһ�ζ���ʹ�õ������¼�ơ�
     *
     * �ú���Ϊһ�����ύ��one-time submit������׼������壺
     *  - ������ط���һ��������壨primary command buffer����
     *  - ��ʼ¼�ƣ�����ʹ�ñ�־Ϊ ONE_TIME_SUBMIT����ʾ Vulkan �û���ֻ��һ�Ρ�
     *
     * @return VkCommandBuffer �����·��䲢��ʼ¼�Ƶ���������
     */
    VkCommandBuffer beginSingleTimeCommands();

    /**
     * @brief ����һ�ζ���ʹ�õ������¼�Ʋ��ύִ�С�
     *
     * �ú��������������¼�ƣ��ύ��ͼ�ζ���ִ�У����ȴ�ִ����ɺ��ͷŸ�����塣
     *
     * @param commandBuffer �ѿ�ʼ¼�Ƶ���������
     */
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    /**
     * @brief �ڸ����� Vulkan ���������Ⱦ ImGui ���档
     *
     * �ú����������� ImGui ��֡������ʾ�� UI �������ݣ����ս� ImGui ���������ύ�� Vulkan ����塣
     *
     * @param cmdBuf �����ύ ImGui ��������� Vulkan ����塣
     */
    void renderImGui(VkCommandBuffer cmdBuf);

private:
    int _width;

    int _height;

    // GLFW���ھ�������𴰿ڹ�������봦��
    GLFWwindow *_window;

    //���ڱ���ɫ
    glm::vec3 _backColor = glm::vec3(0.0f);

private:
    // Vulkanʵ������������Vulkan���Ӻ�״̬
    VkInstance _instance = VK_NULL_HANDLE;

private:
    // ѡ���������豸��GPU��������֧��Vulkan����
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

private:
    // �������߼��豸�������������豸����
    VkDevice _device = VK_NULL_HANDLE;

    // �߼��豸�е�ͼ�ζ��о���������ύ��������
    VkQueue _graphicsQueue = VK_NULL_HANDLE;

private:
    // �߼��豸�еĳ��ֶ��о���������ύͼ���������
    VkQueue _presentQueue = VK_NULL_HANDLE;

private:
    /*vulkan���ڱ�����ȫ��һ�� ��ѡ����������ֻ��Ҫ������Ⱦ��
    Vulkan ����� ��������û�д������ɼ����ڵȼ��ɵ������ִ�д��� ������ OpenGL �Ǳ���ģ���*/

    // ���ڱ���
    VkSurfaceKHR _surface = VK_NULL_HANDLE;

private:
    // �����������������ڹ���������ʾ��ͼ�񼯺ϡ�
    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;

    // ������ͼ���б�ÿ��ͼ������֡����ʾ�����
    std::vector<VkImage> _swapChainImages;

    // ������ͼ������ظ�ʽ���� VK_FORMAT_B8G8R8A8_SRGB����
    VkFormat _swapChainImageFormat;

    // ������ͼ��ĳߴ磨��ߣ����봰��֡����һ�¡�
    VkExtent2D _swapChainExtent;

    // �������õ��߼��豸��չ�б�Ŀǰ������ VK_KHR_swapchain��
    const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

private:
    // ������ͼ����ͼ�б�ÿ��������ͼ���Ӧһ�� VkImageView��
    // ��������ͼ������Ⱦ�����еķ��ʷ�ʽ�͸�ʽ��Ϣ��
    std::vector<VkImageView> _swapChainImageViews;

private:
    // ��Ⱦͨ���������ڶ���֡�����и�����ʹ�÷�ʽ���������ڣ�����ɫ����ȵȣ���
    VkRenderPass _renderPass;

    // ͼ����Ⱦ���߶��󣬷�װ������ͼ�λ������̣�������ɫ��������װ�䡢��դ���Ƚ׶Σ���
    VkPipeline _graphicsPipeline;

    // ���߲��ֶ���ָ������ɫ���������Դ�󶨽ӿڣ��� descriptor set��push constant �ȣ���
    VkPipelineLayout _pipelineLayout;

private:
    // ��������Ӧ��֡�������б�ÿ��������ͼ���Ӧһ��֡������
    std::vector<VkFramebuffer> _swapChainFramebuffers;

private:
    // ����أ����ڹ���ͷ����������
    VkCommandPool _commandPool;

    // ��Ҫ��������������ڼ�¼����ָ��
    std::vector<VkCommandBuffer> _commandBuffers;

private:
    bool _framebufferResized = false;

    // CPU-GPU ͬ�����󣬱�ǵ�ǰ֡�Ƿ�ִ�����
    std::vector<VkFence> _inFlightFences;

    // �ź�������ʾ������ͼ���Ƿ���ã��ȴ����ź�����ʼ��Ⱦ
    std::vector<VkSemaphore> _imageAvailableSemaphores;

    // �ź�������ʾ��Ⱦ�Ƿ���ɣ��ȴ����ź������ύ��������
    std::vector<VkSemaphore> _renderFinishedSemaphores;

private:
    uint32_t _currentFrame = 0;

    const int _MAX_FRAMES_IN_FLIGHT = 2;

private:
    // ��֤��,ֻ��Debugʱ����
#ifdef NDEBUG
    const bool _enableValidationLayers = false;
#else
    const bool _enableValidationLayers = true;
#endif

    // ������Ϣ
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;

    // Ĭ����֤����Ϣ
    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};

private:
    //imgui
    uint32_t _graphicsQueueFamily = 0;

    VkDescriptorPool _imguiDescriptorPool = VK_NULL_HANDLE;
};

#endif    // !TRIANGLEFUNC_H_
