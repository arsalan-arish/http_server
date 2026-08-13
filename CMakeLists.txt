cmake_minimum_required(VERSION 3.31.6)
project(http_server)

add_executable(
    ${PROJECT_NAME} 
    src/main.c
    src/linked_list.c
    src/map.c
    src/url.c
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)