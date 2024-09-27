# Clion 开发 QT 项目
系统环境如下：  
版本	Windows 11 专业工作站版  
版本号	23H2  
操作系统版本	22631.4169  
QT：5.12.9  



## 1. 安装 QT Creator
无脑全选，所有gcc、msvc编译器勾上




## 2. 配置 Clion
### 2.1 `Windows` 系统环境变量里面 `path`变量配置 `QT Creator` 安装路径下的其中一个编译器路径(clion编译完程序后，运行时需要)

```bash
D:\devtools\Qt\Qt5.12.9\Tools\mingw730_64\bin
```

### 2.2 配置编译器
2.2 和 2.3 内容 均在 Clion 中的设置完成  
`File > Settings > Build、Execution、Deployment>Toolchains` 配置编译器 **(最好是配置成qt安装的编译器)**
请根据实际修改
```bash
# 添加编译器
name: MinGW_QT
Toolsets: D:\devtools\Qt\Qt5.12.9\Tools\mingw730_64D:\devtools\Qt\Qt5.12.9\Tools\mingw730_64
Cmake: Bundled
Debugger: D:\devtools\Qt\Qt5.12.9\Tools\mingw730_64\bin\gdb.exe
```

配置Cmake
略……Debug、Release 自行添加

### 2.3 配置QT UI编译器

`Tools > External Tools` 添加两个 UI 编译工具，该工具会把 `.ui` 文件编译成 `cpp` 文件

**Qt Designer:**

Program: qt安装路径交叉编译器里bin目录下的designer.exe

```bash
name: Qt Designer
description: qt界面设计

Tool Settings
	Program: D:\devtools\Qt\Qt5.12.9\5.12.9\mingw73_64\bin\designer.exe
	Arguments: $FileName$
	Working directory: $FileDir$
```

**UIC:**

```bash
name: UIC
description: UI文件

Tool Settings
	Program: D:\devtools\Qt\Qt5.12.9\5.12.9\mingw73_64\bin\uic.exe
	Arguments: $FileName$ -o ui_$FileNameWithoutExtension$.h 
	Working directory: $FileDir$
```

**对于使用Qt Designer时无法直接拖拽控件，需要修改如下内容。(遇到冲突内容，自行决定是否保留)**

`Editor > File and Code Templates > QT Designer Form`

```bash
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
  <author/>
  <comment/>
  <exportmacro/>
  <class>${NAMESPACE_SPECIFIER}${NAME}</class>
  <widget class="${PARENT_CLASS}" name="${NAMESPACE_SPECIFIER}${NAME}">
    <property name="geometry">
      <rect>
        <x>0</x>
        <y>0</y>
        <width>400</width>
        <height>300</height>
      </rect>
    </property>
    <property name="windowTitle">
      <string>${NAME}</string>
          #if( 'QMainWindow' == ${PARENT_CLASS} )
            <widget class="QWidget" name="centralWidget"/>
          #end
    </property>${CLASS_DEPENDENT_CONTENT}
  </widget>
  <pixmapfunction/>
  <connections/>
</ui>
```



## 3. 编译从Clion生成的qt项目测试环境是否正常

### 3.1 qDebug() 在控制台没有输出

点击按钮，可以发现CLion控制台没有显示qDebug()的输出消息，这里需要设置一下，打开运行->编辑配置，添加环境参数`QT_ASSUME_STDERR_HAS_CONSOLE=1`保存后重新运行即可发现功能已经正常。点击按钮，控制台输出PUSHED。

### 3.2 缺少DLL库

我的 `Win 11` 无法编译 `Clion` 生成的项目，需要修改 `CmakeLists.txt` 文件，详见 `Windows 特定设置` 注释内容

```cmake
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

# 查找 Qt5 组件
find_package(Qt5 COMPONENTS Core Gui Widgets Quick LinguistTools REQUIRED NETWORK CONCURRENT)

# 配置 QRC 文件
# 添加qrc依赖,还需要将QRC文件加入编译文件列表才可以
set(QRC_SOURCE_FILE resources/res.qrc)
qt5_add_resources(QT_RESOURCES ${QRC_SOURCE_FILE})

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp
        widget.cpp
        widget.h
        widget.ui
        ${QT_RESOURCES}
)

# 链接 Qt5 库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Concurrent)

# Windows 特定设置
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
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
```

### 3.3 组件功能测试
略……自测

### 3.4 .qrc 文件添加
修改CmakeLists.txt

##### qt 5.12.9

```cmake
set(QRC_SOURCE_FILE resources/res.qrc)
qt5_add_resources(QT_RESOURCES ${QRC_SOURCE_FILE})
add_executable(xxx xxx.cpp ${QRC_SOURCE_FILE})
```

​	

##### qt 5.15+

```cmake
# 添加qrc依赖
set(QRC_SOURCE_FILE resources/res.qrc)
# qt 5.15+版本才有的命令
qt_add_resources(${QRC_SOURCE_FILE})

# 还需要将QRC文件加入编译文件列表才可以
add_executable(xxx xxx.cpp ${QRC_SOURCE_FILE})
```

#### res.qrc内容如下(请根据实际修改)

prefix就是前缀的虚拟文件夹名，在代码中引用规则为`:虚拟文件夹名/文件路径`
一定要在CMake中配置qrc文件(上面有添加方法)

```xml
<RCC>
    <!-- prefix="/image 以下file中的路径image就相当于是前缀，clion中这么配置即可-->
    <qresource>
        <file>image/1.png</file>
    </qresource>

    <!-- prefix="/lang 以下file中的路径lang就相当于是前缀，根据实际修改-->
    <qresource>
        <file>lang/test.txt</file>
    </qresource>

    <!-- prefix="/qss 以下file中的路径lang就相当于是前缀，添加 qresource > file时，请确保实际文件存放在正确位置，如果 qss/test.txt 不存在，编译时会报错-->
    <!-- file>qss/test.txt</file ninja: error: 'project_name/resources/qss/test.txt', needed by 'qrc_res.cpp', missing and no known rule to make it-->
    <qresource>
        <file>qss/style.qss</file>
    </qresource>
</RCC>
```

#### 代码中引用qrc资源文件，请根据实际修改

```c++
//加载样式表
QFile file(":/qss/style.qss");

```

#### 在qss文件中引用图片资源

同样是`:/虚拟文件夹名/文件路径`

```css
QCheckBox{
	image:url(:/image/1.png);
}
```

!!! 引用之前一定要在qrc文件中写清楚文件所在位置，下面有文件报错异常

#### 

#### qrc中的文件不存在，编译时报错 ninja:……

```bash
 ninja: error: 'project_name/resources/qss/test.txt', needed by 'qrc_res.cpp', missing and no known rule to make it-->
```

说明 `resources/qss/test.txt`不存在，自行确认文件情况

```xml
<qresource>
	<file>qss/test.txt</file>
<qresource>
```

