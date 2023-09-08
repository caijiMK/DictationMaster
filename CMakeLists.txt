cmake_minimum_required(VERSION 3.0.0)
project(DictationMaster VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(DictationMaster DictationMaster.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
