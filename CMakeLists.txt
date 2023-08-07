cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(urf_common_cpp VERSION 1.9.0
    LANGUAGES CXX C
    DESCRIPTION "Unified Robotic Framework common objects C++ library")

set(CPACK_PACKAGE_NAME "urf_common_cpp")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/environment/cmake)
include(CPack)
include(UrfCMakeSetup)
include(UrfEditorSetup)

add_subdirectory(src)

option(BUILD_DOC "Build doxygen documentation" OFF)
if(BUILD_DOC)

endif(BUILD_DOC)

option(BUILD_TEST "Build tests" ON)
if(BUILD_TEST)
    include(CTest)
    add_subdirectory(tests)
endif(BUILD_TEST)

SetupEditor()