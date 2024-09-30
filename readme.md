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
D:\devtools\Qt\Tools\mingw730_64\bin	# D:\devtools\Qt\Qt5.12.9\Tools\mingw730_64\bin
```

### 2.2 配置编译器
2.2 和 2.3 内容 均在 Clion 中的设置完成  
`File > Settings > Build、Execution、Deployment>Toolchains` 配置编译器 **(最好是配置成qt安装的编译器)**
请根据实际修改

```bash
# 添加编译器
name: MinGW_QT
Toolsets: D:\devtools\Qt\Tools\mingw730_64
Cmake: Bundled
Debugger: D:\devtools\Qt\Tools\mingw730_64\bin\gdb.exe
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
	Program: D:\devtools\Qt\5.12.9\mingw73_64\bin\designer.exe
	Arguments: $FileName$
	Working directory: $FileDir$
```

**UIC:**

```bash
name: UIC
description: UI文件

Tool Settings
	Program: D:\devtools\Qt\5.12.9\mingw73_64\bin\uic.exe
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
set(CMAKE_PREFIX_PATH "D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake")

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



### 3.5 配置MySQL8 数据库驱动

#### 3.5.1 拷贝MySQL8 lib、dll 库文件

找到自己安装路径，无脑安装的路径  `C:\Program Files\MySQL\MySQL Server 8.0\lib` ，解压的看自己解压位置

拷贝两个文件  `libmysql.dll` 和 `libmysql.lib`   到自己喜欢的位置作为备用 



#### 3.5.2 编译QT MySQL驱动

尝试了网上的方法还是出现 



##### 3.5.2.1 尝试1

- 用打开qt的mysql驱动插件目录 `D:\devtools\Qt\5.12.9\Src\qtbase\src\plugins\sqldrivers\mysql`

- 修改 `mysql.pro`文件

  ```cmake
  TARGET = qsqlmysql
  
  HEADERS += $$PWD/qsql_mysql_p.h
  SOURCES += $$PWD/qsql_mysql.cpp $$PWD/main.cpp
  
  # 注释 mysql
  #QMAKE_USE += mysql
  
  OTHER_FILES += mysql.json
  
  PLUGIN_CLASS_NAME = QMYSQLDriverPlugin
  include(../qsqldriverbase.pri)
  
  # 该路径是的Mysql的头文件（include）路径
  INCLUDEPATH +="C:\Program Files\MySQL\MySQL Server 8.0\include"
  # 这是MySQl的库文件路径
  LIBS +="C:\Program Files\MySQL\MySQL Server 8.0\lib\libmysql.lib"
  # 为了方便查找，不妨增加一条语句，该语句用来指明编译后的结果输出的位置
  DESTDIR = ../mysql/lib/  #  libqsqlmysql.a libqsqlmysqld.a qsqlmysql.dll qsqlmysqld.dll
  # D:\devtools\Qt\5.12.9\Src\qtbase\src\plugins\sqldrivers\mysql\lib
  ```
  
  
  
-  无脑拷贝四个文件到你使用的QT mingw插件目录下(我的编译器路径在开头 `2.1` 已经有写了 `D:\devtools\Qt\5.12.9\mingw73_64`)

  `D:\devtools\Qt\5.12.9\mingw73_64\plugins\sqldrivers`  拷贝到这里面

- 再拷贝前面提到的 `libmysql.dll` 和 `libmysql.lib`(这两个文件位于你的`MySQL`安装目录)文件到QT mingw 的`bin`目录里

  `D:\devtools\Qt\5.12.9\mingw73_64\bin`

- 结果还是提示 `QSqlDatabase: QMYSQL driver not loaded`

- 但是它显示可用？不理解。`QSqlDatabase: available drivers: QSQLITE QMYSQL QMYSQL3 QODBC QODBC3 QPSQL QPSQL7`



##### 3.5.2.2 尝试2



- 用打开qt的mysql驱动插件目录 `D:\devtools\Qt\5.12.9\Src\qtbase\src\plugins\sqldrivers\mysql`

- 修改 `mysql.pro`文件

  ```cmake
  TARGET = qsqlmysql
  
  HEADERS += $$PWD/qsql_mysql_p.h
  SOURCES += $$PWD/qsql_mysql.cpp $$PWD/main.cpp
  
  # 注释 mysql
  #QMAKE_USE += mysql
  
  OTHER_FILES += mysql.json
  
  PLUGIN_CLASS_NAME = QMYSQLDriverPlugin
  include(../qsqldriverbase.pri)
  
  # 使用 mysql-connector-c++-8.0.32-winx64.zip 官网已经编译好的驱动
  INCLUDEPATH +="C:\Program Files\MySQL\MySQL Server 8.0\include"
  #这是MySQl的库文件路径
  LIBS +="C:\Program Files\MySQL\MySQL Server 8.0\lib\libmysql.lib"
  #为了方便查找，不妨增加一条语句，该语句用来指明编译后的结果输出的位置
  DESTDIR = ../mysql/lib/
  # D:\devtools\Qt\5.12.9\Src\qtbase\src\plugins\sqldrivers\mysql\lib
  ```




##### 3.5.2.3 尝试3

GitHub项目地址:

[thecodemonkey86/qt_mysql_driver: Typical symptom: QMYSQL driver not loaded. Solution: get pre-built Qt SQL driver plug-in required to establish a connection to MySQL / MariaDB using Qt. Download qsqlmysql.dll binaries built from official Qt source code (github.com)](https://github.com/thecodemonkey86/qt_mysql_driver)

在该项目中搜索符合你QT版本和对应编译器版本的驱动插件下载

所需文件:

https://objects.githubusercontent.com/github-production-repository-file-5c1aeb/208150766/5519852?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAVCODYLSA53PQK4ZA%2F20240927%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20240927T155456Z&X-Amz-Expires=300&X-Amz-Signature=d65ac7016dfde0e4d64bcd7bc2bf6fb1ec032d1b5774db2f39332e1e6b844ad8&X-Amz-SignedHeaders=host&response-content-disposition=attachment%3Bfilename%3Dqsqlmysql.dll_Qt_SQL_driver_5.12.10_MinGW_7.3_64-Bit.zip&response-content-type=application%2Fx-zip-compressed

解压替换上面编译的文件



QT Creator的项目此时已经正常了

Clion + Cmake 项目还需要修改 Cmakelists.txt 文件

```cmake
# 添加 MySQL 驱动库的路径
set(mysqldriver_plugin "D:/mysqlib/lib")
link_directories(${mysqldriver_plugin})

# 上面的内容在 add_executable 之前进行添加，下面这个在环境依赖拷贝中添加，完整内容见下，如果有错请指出

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/qsqlmysql.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/qsqlmysqld.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

    # 在 Windows 下复制 libmysql.dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/../libmysql.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
```



完整 `Cmakelists.txt`

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
set(CMAKE_PREFIX_PATH "D:/devtools/Qt/5.12.9/mingw73_64/lib/cmake")

# 查找 Qt5 组件
find_package(Qt5 COMPONENTS Core Gui Widgets Quick LinguistTools Sql REQUIRED NETWORK CONCURRENT)

# 配置 QRC 文件
# 添加qrc依赖,还需要将QRC文件加入编译文件列表才可以
set(QRC_SOURCE_FILE resources/res.qrc)
qt5_add_resources(QT_RESOURCES ${QRC_SOURCE_FILE})

# 添加 MySQL 驱动库的路径
set(mysqldriver_plugin "D:/mysqlib/lib")
link_directories(${mysqldriver_plugin})

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp
        widget.cpp
        widget.h
        widget.ui
        ${QT_RESOURCES}
)

# 链接 Qt5 库 # 链接 MySQL Connector/C++ 库，名称可能是 `mysqlcppconn` 或 `mysqlcppconn8`
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Concurrent Qt5::Sql qsqlmysql)

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

    # MYSQL 驱动需要 配置下面内容，不然一定连不上数据库，报错 QMYSQL driver not loaded
    # Windows 下复制 qsqlmysql.dll 和 qsqlmysqld.dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/qsqlmysql.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/qsqlmysqld.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

    # 在 Windows 下复制 libmysql.dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${mysqldriver_plugin}/../libmysql.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif()
```



测试代码，查出数据就行，数据库信息自行修改

```c++
#include <QSqlDatabase>
#include  <QSqlQuery>
#include <QSqlError>
```



```c++
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("root");
    db.setPort(3306);
    db.setDatabaseName("sys_wms");

    if (!db.open())
    {
        QMessageBox::warning(this, "fail", db.lastError().text());
        qDebug() << "Failed to connect to database";
        return;
    }
    else
    {
        qDebug() << "Connected to database";
    }
    QSqlQuery query;
    query.exec("select * from sys_user");
    while (query.next())
    {
        qDebug() << query.value("name").toString();
    }
    db.close();
```





# 4. 安装更多插件

## 4.1 安装opencv

for MinGW:

修改 "D:\env\opencv\opencv-4.10.0\3rdparty\ffmpeg\ffmpeg.cmake" 这个文件

源文件内容:

```xml
# Binaries branch name: ffmpeg/4.x_20240522
# Binaries were created for OpenCV: 8393885a39dac1e650bf5d0aaff84c04ad8bcdd3
ocv_update(FFMPEG_BINARIES_COMMIT "394dca6ceb3085c979415e6385996b6570e94153")
ocv_update(FFMPEG_FILE_HASH_BIN32 "bdfbd1efb295f3e54c07d2cb7a843bf9")
ocv_update(FFMPEG_FILE_HASH_BIN64 "bfef029900f788480a363d6dc05c4f0e")
ocv_update(FFMPEG_FILE_HASH_CMAKE "8862c87496e2e8c375965e1277dee1c7")

function(download_win_ffmpeg script_var)
  set(${script_var} "" PARENT_SCOPE)

  set(ids BIN32 BIN64 CMAKE)
  set(name_BIN32 "opencv_videoio_ffmpeg.dll")
  set(name_BIN64 "opencv_videoio_ffmpeg_64.dll")
  set(name_CMAKE "ffmpeg_version.cmake")

  set(FFMPEG_DOWNLOAD_DIR "${OpenCV_BINARY_DIR}/3rdparty/ffmpeg")

  set(status TRUE)
  foreach(id ${ids})
    ocv_download(FILENAME ${name_${id}}
               HASH ${FFMPEG_FILE_HASH_${id}}
               URL
                 "$ENV{OPENCV_FFMPEG_URL}"
                 "${OPENCV_FFMPEG_URL}"
				# 修改下面url 添加 https://mirror.ghproxy.com/ 前缀即可
                 "https://raw.githubusercontent.com/opencv/opencv_3rdparty/${FFMPEG_BINARIES_COMMIT}/ffmpeg/"
               DESTINATION_DIR ${FFMPEG_DOWNLOAD_DIR}
               ID FFMPEG
               RELATIVE_URL
               STATUS res)
    if(NOT res)
      set(status FALSE)
    endif()
  endforeach()
  if(status)
    set(${script_var} "${FFMPEG_DOWNLOAD_DIR}/ffmpeg_version.cmake" PARENT_SCOPE)
  endif()
endfunction()

if(OPENCV_INSTALL_FFMPEG_DOWNLOAD_SCRIPT)
  configure_file("${CMAKE_CURRENT_LIST_DIR}/ffmpeg-download.ps1.in" "${CMAKE_BINARY_DIR}/win-install/ffmpeg-download.ps1" @ONLY)
  install(FILES "${CMAKE_BINARY_DIR}/win-install/ffmpeg-download.ps1" DESTINATION "." COMPONENT libs)
endif()

ocv_install_3rdparty_licenses(ffmpeg license.txt readme.txt)

```

修改后的:

```xml
# Binaries branch name: ffmpeg/4.x_20240522
# Binaries were created for OpenCV: 8393885a39dac1e650bf5d0aaff84c04ad8bcdd3
ocv_update(FFMPEG_BINARIES_COMMIT "394dca6ceb3085c979415e6385996b6570e94153")
ocv_update(FFMPEG_FILE_HASH_BIN32 "bdfbd1efb295f3e54c07d2cb7a843bf9")
ocv_update(FFMPEG_FILE_HASH_BIN64 "bfef029900f788480a363d6dc05c4f0e")
ocv_update(FFMPEG_FILE_HASH_CMAKE "8862c87496e2e8c375965e1277dee1c7")

function(download_win_ffmpeg script_var)
  set(${script_var} "" PARENT_SCOPE)

  set(ids BIN32 BIN64 CMAKE)
  set(name_BIN32 "opencv_videoio_ffmpeg.dll")
  set(name_BIN64 "opencv_videoio_ffmpeg_64.dll")
  set(name_CMAKE "ffmpeg_version.cmake")

  set(FFMPEG_DOWNLOAD_DIR "${OpenCV_BINARY_DIR}/3rdparty/ffmpeg")

  set(status TRUE)
  foreach(id ${ids})
    ocv_download(FILENAME ${name_${id}}
               HASH ${FFMPEG_FILE_HASH_${id}}
               URL
                 "$ENV{OPENCV_FFMPEG_URL}"
                 "${OPENCV_FFMPEG_URL}"
                 "https://mirror.ghproxy.com/https://raw.githubusercontent.com/opencv/opencv_3rdparty/${FFMPEG_BINARIES_COMMIT}/ffmpeg/"
               DESTINATION_DIR ${FFMPEG_DOWNLOAD_DIR}
               ID FFMPEG
               RELATIVE_URL
               STATUS res)
    if(NOT res)
      set(status FALSE)
    endif()
  endforeach()
  if(status)
    set(${script_var} "${FFMPEG_DOWNLOAD_DIR}/ffmpeg_version.cmake" PARENT_SCOPE)
  endif()
endfunction()

if(OPENCV_INSTALL_FFMPEG_DOWNLOAD_SCRIPT)
  configure_file("${CMAKE_CURRENT_LIST_DIR}/ffmpeg-download.ps1.in" "${CMAKE_BINARY_DIR}/win-install/ffmpeg-download.ps1" @ONLY)
  install(FILES "${CMAKE_BINARY_DIR}/win-install/ffmpeg-download.ps1" DESTINATION "." COMPONENT libs)
endif()

ocv_install_3rdparty_licenses(ffmpeg license.txt readme.txt)

```







```
@echo off

:: 设置代理
set all_proxy=socks5://127.0.0.1:20080

:: 更新 PATH 环境变量
set PATH=%PATH%;D:\devtools\Qt\Tools\mingw730_64\bin;D:\env\CMake\bin
set PYTHON_EXECUTABLE=D:\env\python\python2\python2.7.2\bin

:: 设置 Qt 相关的环境变量
set QT_MAKE_EXECUTABLE=D:\devtools\Qt\5.12.9\mingw73_64\bin\qmake.exe
set Qt5Concurrent_DIR=D:\devtools\Qt\5.12.9\mingw73_64\lib\cmake\Qt5Concurrent
set Qt5Core_DIR=D:\devtools\Qt\5.12.9\mingw73_64\lib\cmake\Qt5Core
set Qt5Gui_DIR=D:\devtools\Qt\5.12.9\mingw73_64\lib\cmake\Qt5Gui
set Qt5Test_DIR=D:\devtools\Qt\5.12.9\mingw73_64\lib\cmake\Qt5Test
set Qt5Widgets_DIR=D:\devtools\Qt\5.12.9\mingw73_64\lib\cmake\Qt5Widgets
set Qt5OpenGL_DIR=D:\devtools\Qt\5.12.9\mingw73_64\lib\cmake\Qt5OpenGL

:: 设置 CMake 构建类型
set CMAKE_BUILD_TYPE=Release

:: 跳过 OpenCV 版本信息检查
set OPENCV_VS_VERSIONINFO_SKIP=1

:: 进入 OpenCV 源码目录并构建
cd D:\env\opencv\opencv-4.5.1
D:
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
# 新版本(4.10.0)已经会自动编译了，后面两步可以跳过
mingw32-make -j20
mingw32-make install

pause
```



拷贝编译完的opencv

qmake:

```cmake
# opencv
win32{
INCLUDEPATH += D:\env\opencv\opencv-4.5.1\build\install\include
LIBS += D:\env\opencv\opencv-4.5.1\build\lib\libopencv_*.a
}
```



cmake:

```cmake
# OpenCV 构建目录
set(OpenCVDIR "D:/env/opencv/opencv-4.5.1/build/install")

find_package(OpenCV 4.5.1 REQUIRED COMPONENTS core imgproc highgui imgcodecs flann calib3d features2d videoio highgui ml objdetect stitching)

# OpenCV 头文件
include_directories(${OpenCV_INCLUDE_DIRS})

# 打印 OpenCV 库以便调试
# message(STATUS "OpenCV libraries found: ${OpenCV_LIBS}")

# 链接 Qt5 和 OpenCV 库
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
```



完整cmakelists.txt:

```cmake
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
# 使用如下命令 mingw-make && mingw-install 得到编译产物在编译输出目录的 install 目录下
set(OpenCV_DIR "D:/env/opencv/opencv-4.5.1/build/install")
find_package(OpenCV 4.5.1 REQUIRED COMPONENTS core imgproc highgui imgcodecs flann calib3d features2d videoio highgui ml objdetect stitching)
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
```



编写代码测试:

widget.cpp:

```c++
#include <opencv2/highgui/highgui.hpp>
```



```c++


    const cv::Mat image = cv::imread("E:/ROG-4k.jpg", 1);
    // create image window named "My Image"
    cv::namedWindow("My Image");
    // show the image on window
    cv::imshow("My Image", image);
```





## 4.2 安装Redis client

[AnthonySnow887/QtRedisClient: NoSQL Redis client library based on Qt 5 (github.com)](https://github.com/AnthonySnow887/QtRedisClient)
