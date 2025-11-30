cmake_minimum_required(VERSION 3.22)
project(booksource LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

enable_testing()
add_subdirectory(test)
add_subdirectory(third_party/quickjs)
add_subdirectory(third_party/curl)

file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE INCLUDE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/booksource/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/booksource/*.h"
)

add_library(booksource ${SRC_FILES} ${INCLUDE_FILES})

# js引擎: quickjs
target_link_libraries(booksource PUBLIC quickjs)
target_link_libraries(booksource PUBLIC libcurl_shared)

target_include_directories(booksource
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/curl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include
)
