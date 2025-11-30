add_executable(test_jsengine EXCLUDE_FROM_ALL test_jsengine.cpp)
target_link_libraries(test_jsengine PRIVATE booksource)

add_executable(test_object_bind EXCLUDE_FROM_ALL test_object_bind.cpp)
target_link_libraries(test_object_bind PRIVATE booksource)

add_executable(test_quickjs EXCLUDE_FROM_ALL test_quickjs.cpp)
target_link_libraries(test_quickjs PRIVATE booksource)

add_executable(test_bs_parse EXCLUDE_FROM_ALL test_bs_parse.cpp)
target_link_libraries(test_bs_parse PRIVATE booksource)

add_executable(test_webbook EXCLUDE_FROM_ALL test_webbook.cpp)
target_link_libraries(test_webbook PRIVATE booksource)

# 生成一个头文件，用于确定当前项目的路径
set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/config/test_utils.in
        ${CMAKE_CURRENT_SOURCE_DIR}/test_utils.h
)

add_test(NAME TestJsEngine COMMAND test_jsengine)
add_test(NAME TestObjectBind COMMAND test_object_bind)
add_test(NAME TestQuickJS COMMAND test_quickjs)
add_test(NAME TestBsParse COMMAND test_bs_parse)
add_test(NAME TestWebBook COMMAND test_webbook)
