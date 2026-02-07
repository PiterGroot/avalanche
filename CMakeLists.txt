cmake_minimum_required(VERSION 3.15)

project(avalanche VERSION 0.1.0
    DESCRIPTION "single-header dependency-free C++ falling-sand library"
    HOMEPAGE_URL "https://github.com/PiterGroot/avalanche"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(avalanche INTERFACE)
target_include_directories(avalanche INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Demos)
