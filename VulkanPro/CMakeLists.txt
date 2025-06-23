CMAKE_MINIMUM_REQUIRED (VERSION 3.5.2)

SET (ProName VulkanPro)

PROJECT (${ProName})

INCLUDE_DIRECTORIES(
	../3rd/stb/
	../3rd/glm/
	../3rd/glfw/include/
	../3rd/vulkan/include/
)

SET(SRC
	src/main.cpp

	src/MacroHead.h

	src/Helper/Print.h
	src/Helper/Print.cpp

	src/TriangleFunc.h
	src/TriangleFunc.cpp
)

ADD_EXECUTABLE(${ProName} ${SRC}  ${RC})

SET(VULKAN_LIB_DIR ${CMAKE_SOURCE_DIR}/3rd/vulkan/lib/)
TARGET_LINK_DIRECTORIES(${ProName} PRIVATE ${VULKAN_LIB_DIR})

TARGET_LINK_LIBRARIES (${ProName} glfw)
TARGET_LINK_LIBRARIES (${ProName} vulkan-1)




