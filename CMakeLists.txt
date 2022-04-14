cmake_minimum_required(VERSION 3.10)
project(influence_maps VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

add_executable(influence_maps)
add_subdirectory(vendor)

find_package(Allegro5 REQUIRED)

target_include_directories(influence_maps PRIVATE
    ${ALLEGRO5_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(influence_maps PRIVATE
    src/main.cpp
    src/map.cpp
    src/influence_map.cpp
)

target_link_libraries(influence_maps PRIVATE
    ${ALLEGRO5_LIBRARIES}
    imgui
)
