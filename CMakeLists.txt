cmake_minimum_required(VERSION 3.5)

project(libcommon)

file(GLOB_RECURSE SRC "common/*.cpp" "common/*.h")
add_library(libcommon ${SRC})
set(TINY_LIBRARIES "${TINY_LIBRARIES};libcommon" PARENT_SCOPE)
