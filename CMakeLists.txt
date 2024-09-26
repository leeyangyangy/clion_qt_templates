# https://www.jetbrains.com/help/clion/qt-tutorial.html#qt-template
cmake_minimum_required(VERSION 3.20)

# 项目名称，避免写死
set(PROJECT_NAME qt_clion_template)
project(${PROJECT_NAME})

# 设置 C++ 标准和其它QT依赖
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置 Qt 安装路径
set(CMAKE_PREFIX_PATH "D:/devtools/Qt/Qt5.12.9/5.12.9/mingw73_64/lib/cmake")

#set(Qt5_DIR "${CMAKE_PREFIX_PATH}/Qt5")

# 配置qrc文件
# 添加qrc依赖,还需要将QRC文件加入编译文件列表才可以
set(QRC_SOURCE_FILE res.qrc)
#qt_add_resources(${QRC_SOURCE_FILE})

# 查找 Qt5 组件
find_package(Qt5 COMPONENTS Core Gui Widgets Quick LinguistTools REQUIRED NETWORK CONCURRENT)

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp
        widget.cpp
        widget.h
        widget.ui
#        ${QRC_SOURCE_FILE}
)

# 第一个参数是项目名，第二个参数是 qrc 路径 ，第三个参数是前缀 第四个参数是资源路径，第五、六个参数是qrc文件名
qt5_add_resources(${PROJECT_NAME} "" PREFIX "/resources" FILES ${QRC_SOURCE_FILE})

# 链接 Qt5 库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Concurrent)

# Windows 特定设置
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    # Windows 特定的设置
    message(STATUS "This is a Windows system.")
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()

    # 创建插件目录并复制平台插件
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif()

    # 复制所需的 Qt DLL 文件
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach()

    # 使用 windeployqt 工具自动部署 DLL（可选）
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${QT_INSTALL_PATH}/bin/windeployqt.exe"
            "$<TARGET_FILE:${PROJECT_NAME}>")
endif()