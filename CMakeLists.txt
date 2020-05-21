cmake_minimum_required(VERSION 3.7)

project(cpptemplater)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Isrc/")
add_executable(cpptemplater src/cpptemplater.cc)
target_compile_features(cpptemplater PUBLIC cxx_std_20)


