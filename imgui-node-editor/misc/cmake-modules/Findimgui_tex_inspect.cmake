if (TARGET imgui_tex_inspect)
    return()
endif()

find_package(imgui REQUIRED)

set(_imgui_tex_inspect_SourceDir ${IMGUI_NODE_EDITOR_ROOT_DIR}/external/imgui_tex_inspect)
set(_imgui_tex_inspect_Sources
    ${_imgui_tex_inspect_SourceDir}/imgui_tex_inspect.h
    ${_imgui_tex_inspect_SourceDir}/imgui_tex_inspect.cpp
    ${_imgui_tex_inspect_SourceDir}/imgui_tex_inspect_internal.h
)

add_library(imgui_tex_inspect STATIC
    ${_imgui_tex_inspect_Sources}
)

target_include_directories(imgui_tex_inspect PUBLIC
    ${_imgui_tex_inspect_SourceDir}
)

target_link_libraries(imgui_tex_inspect PUBLIC imgui)

source_group(TREE ${_imgui_tex_inspect_SourceDir} FILES ${_imgui_tex_inspect_Sources})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui_tex_inspect
    REQUIRED_VARS
        _imgui_tex_inspect_SourceDir
)
