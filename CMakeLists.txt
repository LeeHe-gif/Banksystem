cmake_minimum_required(VERSION 3.16)

project(BankSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt包
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network)

# 设置包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 设置源文件
set(PROJECT_SOURCES
    main.cpp
    loginwindow.cpp
    mainwindow.cpp
    databasemanager.cpp
)

# 设置头文件
set(PROJECT_HEADERS
    loginwindow.h
    mainwindow.h
    databasemanager.h
)

# 设置UI文件
set(PROJECT_FORMS
    loginwindow.ui
    mainwindow.ui
)

# 创建可执行文件
add_executable(BankSystem
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_FORMS}
)

# 链接Qt库
target_link_libraries(BankSystem PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# Windows平台链接MySQL库
if(WIN32)
    target_link_libraries(BankSystem PRIVATE
        -lmysql
        -lws2_32
    )
    # 设置MySQL库路径
    target_include_directories(BankSystem PRIVATE
        "C:/Program Files/MySQL/MySQL Server 8.0/include"
    )
    target_link_directories(BankSystem PRIVATE
        "C:/Program Files/MySQL/MySQL Server 8.0/lib"
    )
endif()
