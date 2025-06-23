#include <optional>

/**
 * @brief �洢 Vulkan ������������Ϣ������ѡ����ʵ������豸��
 *
 * Vulkan �еĲ�ͬ��������ͼ�λ��ơ�ͼ����ֵȣ���Ҫʹ��֧����Ӧ���ܵĶ����塣
 * �˽ṹ�����ڼ�¼���ҵ���ͼ�ζ�����ͳ��ֶ��������������ṩ�����жϺ�����
 */
struct QueueFamilyIndices {
    /**
     * @brief ͼ�ζ�������������ѡֵ����
     *
     * ����豸֧��ͼ�β�����������������ֵ��������Ч������
     */
    std::optional<uint32_t> graphicsFamily;

    /**
     * @brief ���ֶ�������������ѡֵ����
     *
     * ����豸֧�ֽ�ͼ����ֵ�ָ�� Surface����ֵ��������Ч������
     */
    std::optional<uint32_t> presentFamily;

    /**
     * @brief �ж��Ƿ����ҵ���������Ķ����塣
     *
     * һ��ͼ��Ӧ����Ҫͬʱ֧��ͼ�λ�����ͼ����֣�������������嶼��Ҫ��
     *
     * @return true ���ͼ�κͳ��ֶ��������Ч��
     * @return false ����
     */
    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

/**
 * @brief �洢������֧�ֵ���ϸ��Ϣ��
 *
 * ��ѡ�񽻻������ã����ʽ��ģʽ����С��ǰ�����ѯ�����豸��ָ�� surface ��֧�������
 */
struct SwapChainSupportDetails {
    /**
     * @brief ����������Ϣ������С/���ͼ���������ֱ��ʷ�Χ�ȣ���
     */
    VkSurfaceCapabilitiesKHR capabilities;

    /**
     * @brief ֧�ֵı����ʽ�б���ɫ��ʽ + ɫ�ʿռ䣩��
     */
    std::vector<VkSurfaceFormatKHR> formats;

    /**
     * @brief ֧�ֵĳ���ģʽ�б��� FIFO��MAILBOX �ȣ���
     */
    std::vector<VkPresentModeKHR> presentModes;
};
