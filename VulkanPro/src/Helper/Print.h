#ifndef PRINT_H_
#define PRINT_H_

#include <iostream>
#include <vector>

/**
 * @brief ��ӡ std::vector �����е�����Ԫ�أ����� Debug ģʽ�£���
 *
 * @tparam T Ԫ�����ͣ���֧����������� <<��
 * @param vec ����ӡ�� vector ������
 */
template <typename T>
void PrintVec(const T &vec)
{
#ifndef NDEBUG
    for (const auto &idx : vec) {
        std::cout << idx << std::endl;
    }
#endif
}

/**
 * @brief ��ӡ���� std::vector �����е�����Ԫ�أ����� Debug ģʽ�£���
 *
 * @tparam T Ԫ�����ͣ���֧����������� <<��
 * @param vec ����ӡ�� vector ������
 */
template <typename T>
void PrintVec(const std::string &title, const T &vec)
{
#ifndef NDEBUG
    std::cout << title << std::endl;
    PrintVec(vec);
#endif
}

/**
 * @brief ��ӡ Vulkan �����豸���Լ����֣��� Debug ģʽ�����ã���
 *
 * @param deviceProperties Vulkan �����豸���Խṹ�塣
 * @param score �豸���֡�
 */
template <typename T, typename T2>
void PrintPhysicalDev(const T &deviceProperties, T2 score)
{
#ifndef NDEBUG
    std::cout << "�豸����: " << deviceProperties.deviceName << std::endl;
    std::cout << "API �汾: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion) << "." << VK_VERSION_PATCH(deviceProperties.apiVersion)
              << std::endl;
    std::cout << "����: " << score << std::endl;
#endif
}

#endif    // !PRINT_H_
