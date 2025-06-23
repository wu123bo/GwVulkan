#include <optional>

/**
 * @brief 存储 Vulkan 队列族索引信息，用于选择合适的物理设备。
 *
 * Vulkan 中的不同操作（如图形绘制、图像呈现等）需要使用支持相应功能的队列族。
 * 此结构体用于记录查找到的图形队列族和呈现队列族索引，并提供辅助判断函数。
 */
struct QueueFamilyIndices {
    /**
     * @brief 图形队列族索引（可选值）。
     *
     * 如果设备支持图形操作（如绘制命令），此值将包含有效索引。
     */
    std::optional<uint32_t> graphicsFamily;

    /**
     * @brief 呈现队列族索引（可选值）。
     *
     * 如果设备支持将图像呈现到指定 Surface，此值将包含有效索引。
     */
    std::optional<uint32_t> presentFamily;

    /**
     * @brief 判断是否已找到所有所需的队列族。
     *
     * 一般图形应用需要同时支持图形绘制与图像呈现，因此两个队列族都需要。
     *
     * @return true 如果图形和呈现队列族均有效。
     * @return false 否则。
     */
    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

/**
 * @brief 存储交换链支持的详细信息。
 *
 * 在选择交换链设置（如格式、模式、大小）前，需查询物理设备对指定 surface 的支持情况。
 */
struct SwapChainSupportDetails {
    /**
     * @brief 表面能力信息（如最小/最大图像数量、分辨率范围等）。
     */
    VkSurfaceCapabilitiesKHR capabilities;

    /**
     * @brief 支持的表面格式列表（颜色格式 + 色彩空间）。
     */
    std::vector<VkSurfaceFormatKHR> formats;

    /**
     * @brief 支持的呈现模式列表（如 FIFO、MAILBOX 等）。
     */
    std::vector<VkPresentModeKHR> presentModes;
};
