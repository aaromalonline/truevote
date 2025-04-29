cmake_minimum_required(VERSION 3.16)
project(evote)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenSSL REQUIRED)

add_executable(evote
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(evote PRIVATE
    Qt6::Widgets
    OpenSSL::SSL
    OpenSSL::Crypto
)