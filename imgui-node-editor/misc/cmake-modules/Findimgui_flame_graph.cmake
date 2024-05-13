if (TARGET imgui_flame_graph)
    return()
endif()

find_package(imgui REQUIRED)

set(_imgui_flame_graph_SourceDir ${IMGUI_NODE_EDITOR_ROOT_DIR}/external/imgui-flame-graph)
set(_imgui_flame_graph_Sources
    ${_imgui_flame_graph_SourceDir}/imgui_widget_flamegraph.h
    ${_imgui_flame_graph_SourceDir}/imgui_widget_flamegraph.cpp
)

add_library(imgui_flame_graph STATIC
    ${_imgui_flame_graph_Sources}
)

target_include_directories(imgui_flame_graph PUBLIC
    ${_imgui_flame_graph_SourceDir}
)

target_link_libraries(imgui_flame_graph PUBLIC imgui)

source_group(TREE ${_imgui_flame_graph_SourceDir} FILES ${_imgui_flame_graph_Sources})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui_flame_graph
    REQUIRED_VARS
        _imgui_flame_graph_SourceDir
)
