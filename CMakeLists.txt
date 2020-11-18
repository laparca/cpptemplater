cmake_minimum_required(VERSION 3.7)

project(cpptemplater)

add_executable(cpptemplater src/cpptemplater.cc)
target_include_directories(cpptemplater PUBLIC include)
target_compile_features(cpptemplater PUBLIC cxx_std_20)


