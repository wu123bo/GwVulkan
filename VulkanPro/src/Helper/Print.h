#ifndef PRINT_H_
#define PRINT_H_

#include <iostream>
#include <vector>

/**
 * @brief 打印 std::vector 容器中的所有元素（仅在 Debug 模式下）。
 *
 * @tparam T 元素类型，需支持输出操作符 <<。
 * @param vec 待打印的 vector 容器。
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
 * @brief 打印标题 std::vector 容器中的所有元素（仅在 Debug 模式下）。
 *
 * @tparam T 元素类型，需支持输出操作符 <<。
 * @param vec 待打印的 vector 容器。
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
 * @brief 打印 Vulkan 物理设备属性及评分（仅 Debug 模式下启用）。
 *
 * @param deviceProperties Vulkan 物理设备属性结构体。
 * @param score 设备评分。
 */
template <typename T, typename T2>
void PrintPhysicalDev(const T &deviceProperties, T2 score)
{
#ifndef NDEBUG
    std::cout << "设备名称: " << deviceProperties.deviceName << std::endl;
    std::cout << "API 版本: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion) << "." << VK_VERSION_PATCH(deviceProperties.apiVersion)
              << std::endl;
    std::cout << "评分: " << score << std::endl;
#endif
}

#endif    // !PRINT_H_
