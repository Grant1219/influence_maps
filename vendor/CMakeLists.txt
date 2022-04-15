find_package(Allegro5 REQUIRED)

add_library(imgui STATIC)

set(IMGUI_BASEDIR "imgui")

target_include_directories(imgui PRIVATE
    ${ALLEGRO5_INCLUDE_DIRS}
    ${IMGUI_BASEDIR}
    ${IMGUI_BASEDIR}/misc/cpp
    ${IMGUI_BASEDIR}/backends
)

target_sources(imgui PRIVATE
    ${IMGUI_BASEDIR}/imgui.cpp
    ${IMGUI_BASEDIR}/imgui_demo.cpp
    ${IMGUI_BASEDIR}/imgui_draw.cpp
    ${IMGUI_BASEDIR}/imgui_widgets.cpp
    ${IMGUI_BASEDIR}/imgui_tables.cpp
    ${IMGUI_BASEDIR}/backends/imgui_impl_allegro5.cpp
    ${IMGUI_BASEDIR}/misc/cpp/imgui_stdlib.cpp
)

target_link_libraries(imgui PRIVATE
    ${ALLEGRO5_LIBRARIES}
)

target_include_directories(influence_maps PRIVATE
    ${IMGUI_BASEDIR}
    ${IMGUI_BASEDIR}/misc/cpp
    ${IMGUI_BASEDIR}/backends
)
