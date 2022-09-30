cmake_minimum_required(VERSION 3.13)

project(mediator)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(test src/test.cpp)
target_include_directories(test PRIVATE include)
target_link_libraries(test PRIVATE CONAN_PKG::gtest)

target_sources(test PRIVATE src/test.cpp include/staticmediator.hpp)
