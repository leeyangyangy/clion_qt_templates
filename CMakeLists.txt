cmake_minimum_required(VERSION 3.20)

# 项目名称，避免写死
set(PROJECT_NAME qt_clion_template)
project(${PROJECT_NAME})

# 设置 C++ 标准和其它 Qt 依赖
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# 查看完整编译命令
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置 Qt 安装路径
set(CMAKE_PREFIX_PATH "D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake")

# 查找 Qt5 组件
find_package(Qt5 COMPONENTS Core Gui Widgets Quick LinguistTools Sql Network Concurrent REQUIRED)

# 配置 QRC 文件
set(QRC_SOURCE_FILE resources/res.qrc)
qt5_add_resources(QT_RESOURCES ${QRC_SOURCE_FILE})

# 添加 MySQL 驱动库的路径
set(mysqldriver_plugin ${CMAKE_SOURCE_DIR}/libs/mysqlib)
link_directories(${mysqldriver_plugin}/lib)

# OpenCV 构建目录 https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows  https://github.com/huihut/OpenCV-MinGW-Build?tab=readme-ov-file
set(OpenCV_DIR D:/env/opencv/OpenCV-MinGW-Build-OpenCV-4.5.2-x64)
find_package(OpenCV 4.5.2 REQUIRED COMPONENTS core imgproc highgui imgcodecs flann calib3d features2d videoio highgui ml objdetect stitching)
# 设置 OpenCV 头文件路径
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp widget.cpp widget.h widget.ui ${QT_RESOURCES})

# 链接 Qt5 和 OpenCV 库
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::Concurrent
        Qt5::Sql
        qsqlmysql
        ${OpenCV_LIBS}
)

# Windows 特定设置
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()

    # 创建插件目录并复制平台插件
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()

    # 复制所需的 Qt DLL 文件
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach ()

    # 使用 windeployqt 工具自动部署 DLL（可选）
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${QT_INSTALL_PATH}/bin/windeployqt.exe" "$<TARGET_FILE:${PROJECT_NAME}>"
    )

    # 复制 MySQL 驱动和 libmysql.dll
    foreach (MYSQL_DLL qsqlmysql.dll qsqlmysqld.dll)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${mysqldriver_plugin}/lib/${MYSQL_DLL}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach ()

    # 在 Windows 下复制 libmysql.dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/libmysql.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif ()