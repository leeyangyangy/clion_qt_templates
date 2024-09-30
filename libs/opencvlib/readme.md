自行修改`opencv`路径和qt `mingw`环境

```bat
@echo off

:: 设置代理
set all_proxy=socks5://127.0.0.1:20080

:: 更新 PATH 环境变量
set PATH=%PATH%;D:/devtools/Qt/Tools/mingw730_64/bin;D:/devtools/JetBrains/CLion 2024.2.0.1/bin/cmake/win/x64/bin/cmake.exe
set PYTHON_EXECUTABLE=D:/env/python/python2/python2.7.2/bin
set PYTHON2_EXECUTABLE=D:/env/python/python2/python2.7.2/bin

:: 设置 Qt 相关的环境变量
set WITH_QT=true
set QT_MAKE_EXECUTABLE=D:/devtools/Qt/5.12.9/mingw73_64/bin/qmake.exe
set OPENCV_EXTRA_MODULES_PATH=D:/env/opencv/opencv-4.5.1/extra_modules/opencv_contrib-4.5.1/modules
set Qt5Concurrent_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5Concurrent
set Qt5_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5
set Qt5Core_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5Core
set Qt5Gui_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5Gui
set Qt5OpenGL_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5OpenGL
set Qt5Test_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5Test
set Qt5Widgets_DIR=D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake/Qt5Widgets

:: 设置 CMake 构建类型
set CMAKE_BUILD_TYPE=Release

:: 跳过 OpenCV 版本信息检查
set OPENCV_VS_VERSIONINFO_SKIP=1

:: 进入 OpenCV 源码目录并构建
cd D:/env/opencv/opencv-4.5.1
D:
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
:: # 新版本(4.10.0)已经会自动编译了，后面两步可以跳过
mingw32-make -j20
mingw32-make install

pause
```

