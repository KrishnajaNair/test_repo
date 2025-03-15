cmake_minimum_required(VERSION 3.8)

project(mtsc_test LANGUAGES C)

add_executable(${PROJECT_NAME} main.c)

target_compile_options(${PROJECT_NAME} PRIVATE 
    $<$<C_COMPILER_ID:GNU>:-Werror -Wall -Wextra -Wconversion -Wsign-conversion>
    $<$<C_COMPILER_ID:MSVC>: /FC /W4>
)

target_link_libraries(${PROJECT_NAME} mtsc::mtsc)

# For C language we cannot use Gtest
add_test (NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Visual Studio folders
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tests")
