cmake_minimum_required(VERSION 3.16)

project(
    GuidedRepositoryOpenMP
    VERSION 5.0.0
    DESCRIPTION "Guided examples for learning OpenMP"
    LANGUAGES CXX
)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

include(CTest)
include(FetchContent)

set(MATRIX_CLASS_DEMO_VERSION v2.0.0 CACHE STRING "MatrixClassDemo release tag")
set(CUSTOM_MEMORY_ALLOCATOR_VERSION v7.0.0 CACHE STRING "CustomMemoryAllocator release tag")

FetchContent_Declare(
    MatrixClassDemo
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/MatrixClassDemo.git
    GIT_TAG ${MATRIX_CLASS_DEMO_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    CustomMemoryAllocator
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/CustomMemoryAllocator.git
    GIT_TAG ${CUSTOM_MEMORY_ALLOCATOR_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(MatrixClassDemo CustomMemoryAllocator)

find_package(OpenMP REQUIRED)

add_library(guided_openmp src/MatrixProduct.cpp)
add_library(GuidedRepositoryOpenMP::guided_openmp ALIAS guided_openmp)

target_compile_features(guided_openmp PUBLIC cxx_std_17)
target_include_directories(guided_openmp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(guided_openmp PUBLIC MatrixClassDemo::matrix OpenMP::OpenMP_CXX)

add_subdirectory(example)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
