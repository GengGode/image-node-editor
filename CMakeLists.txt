cmake_minimum_required(VERSION 3.12)

include(cmake/find_deps.cmake)
include(cmake/find_opencv.cmake)
include(cmake/auto_subdirectory.cmake)

project(image-node-editor)

set(IMAGE_NODE_EDITOR_BUILD_OPENCV OFF CACHE BOOL "Build with opencv")

# Define IMGUI_NODE_EDITOR_ROOT_DIR pointing to project root directory
get_filename_component(IMGUI_NODE_EDITOR_ROOT_DIR "${CMAKE_SOURCE_DIR}/imgui-node-editor" ABSOLUTE CACHE)

# Enable solution folders in Visual Studio and Folders in Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Point CMake where to look for module files.
list(APPEND CMAKE_MODULE_PATH ${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/cmake-modules)

# Node editor use C++14
set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)
# Node editor use Utf-8 source 
if (MSVC)
    add_compile_options("/utf-8")
    # 启用控制台窗口
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
else()
    add_compile_options("-finput-charset=UTF-8")
endif()
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finput-charset=UTF-8")

if(IMAGE_NODE_EDITOR_BUILD_OPENCV)
    set(opencv_dir "${CMAKE_SOURCE_DIR}/deps/opencv" CACHE STRING "opencv dir")
    if(NOT EXISTS ${opencv_dir})
        message(STATUS "opencv dir not found, auto download deps...")
        find_opencv(opencv_dir 4.8.0)
    endif()
    set(WITH_DYNAMIC_OPENCV ON)
    set(CMAKE_BUILD_TYPE Debug)
    find_static_library(opencv ${opencv_dir})
    set(opencv_include_dir "${opencv_INCLUDE_DIR}")
else()
    if(WIN32)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
    endif()
    # if exist opencv-lib-Debug.zip or opencv-lib-Release.zip
    if(EXISTS "${IMGUI_NODE_EDITOR_ROOT_DIR}/external/opencv-lib-${CMAKE_BUILD_TYPE}.zip"
        AND NOT EXISTS "${IMGUI_NODE_EDITOR_ROOT_DIR}/external/opencv-lib-${CMAKE_BUILD_TYPE}")
        message(STATUS "opencv-lib-${CMAKE_BUILD_TYPE}.zip found, auto download deps...")
        # unzip opencv-lib-Debug.zip or opencv-lib-Release.zip
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvf "${IMGUI_NODE_EDITOR_ROOT_DIR}/external/opencv-lib-${CMAKE_BUILD_TYPE}.zip" WORKING_DIRECTORY "${IMGUI_NODE_EDITOR_ROOT_DIR}/external")
        message(STATUS "unzip opencv-lib-${CMAKE_BUILD_TYPE}.zip success")
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OpenCV_LIBS opencv_world480d.lib)
        set(OpenCV_BINS opencv_world480d.dll)
    else()
        set(OpenCV_LIBS opencv_world480.lib)
        set(OpenCV_BINS opencv_world480.dll)
    endif()

    set(Opencv_DIR "${IMGUI_NODE_EDITOR_ROOT_DIR}/external/opencv-lib-${CMAKE_BUILD_TYPE}")
    message(STATUS "Opencv_DIR = ${Opencv_DIR}")
    #message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
    set(OpenCV_INCLUDE_DIRS "${Opencv_DIR}/include")
    #message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")
    set(OpenCV_LIB_DIR "${Opencv_DIR}/lib")
    set(OpenCV_BIN_DIR "${Opencv_DIR}/bin")
endif()

# meojson
include(FetchContent)
set(BUILD_SAMPLE OFF)
set(BUILD_TESTING OFF)
FetchContent_Declare(
    meojson
    GIT_REPOSITORY https://github.com/MistEO/meojson.git GIT_TAG master)
FetchContent_MakeAvailable(meojson)



# Macro that will configure an example application
macro(add_example_executable name)
    project(${name})

    set(_Example_Sources
        ${ARGN}
    )

    #source_group("" FILES ${_Example_Sources})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_Example_Sources})

    file(GLOB _Example_CommonResources CONFIGURE_DEPENDS "${IMGUI_NODE_EDITOR_ROOT_DIR}/../data/*")
    file(GLOB _Example_Resources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
    #message(FATAL_ERROR "_Example_Resources = ${_Example_Resources}")

    set(_Example_Type)
    if (WIN32)
        set(_Example_Type WIN32)

        set(ApplicationIcon ${IMGUI_NODE_EDITOR_ROOT_DIR}/../Application/Support/Icon.ico)
        file(TO_NATIVE_PATH "${ApplicationIcon}" ApplicationIcon)
        string(REPLACE "\\" "\\\\" ApplicationIcon "${ApplicationIcon}")
        configure_file(
            ${IMGUI_NODE_EDITOR_ROOT_DIR}/../Application/Support/Resource.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
        )
        source_group(TREE "${IMGUI_NODE_EDITOR_ROOT_DIR}/.." FILES ${_Example_CommonResources})
        source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${_Example_Resources})
        list(APPEND _Example_Resources
            ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
            ${_Example_CommonResources}
        )
        source_group("resources" FILES ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc)
    elseif (APPLE)
        set(_Example_Type MACOSX_BUNDLE)

        set_source_files_properties(${_Example_Resources} ${_Example_CommonResources} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/data"
        )
        set(_Example_Icon "${IMGUI_NODE_EDITOR_ROOT_DIR}/../application/support/Icon.icns")
        list(APPEND _Example_Resources ${_Example_Icon})
        set_source_files_properties(${_Example_Icon} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
    endif()

    add_executable(${name} ${_Example_Type} ${_Example_Sources} ${_Example_Resources} ${_Example_CommonResources})

    find_package(imgui REQUIRED)
    find_package(imgui_node_editor REQUIRED)
    target_link_libraries(${name} PRIVATE imgui imgui_node_editor application)
    target_link_libraries(${name} PRIVATE meojson)
    target_link_libraries(${name} PRIVATE ${OpenCV_LIBS})
    target_link_directories(${name} PRIVATE ${OpenCV_LIB_DIR})
    target_include_directories(${name} PRIVATE ${OpenCV_INCLUDE_DIRS})

    set(_ExampleBinDir ${CMAKE_BINARY_DIR}/bin)

    set_target_properties(${name} PROPERTIES
        FOLDER "examples"
        RUNTIME_OUTPUT_DIRECTORY                "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${_ExampleBinDir}"
        DEBUG_POSTFIX                           _d
        RELWITHDEBINGO_POSTFIX                  _rd
        MINSIZEREL_POSTFIX                      _r
        VS_DEBUGGER_WORKING_DIRECTORY           ${_ExampleBinDir}
        MACOSX_BUNDLE_INFO_PLIST                "${IMGUI_NODE_EDITOR_ROOT_DIR}/../application/support/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME               "${PACKAGE_NAME}"
        MACOSX_BUNDLE_GUI_IDENTIFIER            "com.sandbox.collisions"
        MACOSX_BUNDLE_LONG_VERSION_STRING       "${PACKAGE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING      "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}"
        MACOSX_BUNDLE_ICON_FILE                 Icon.icns
    )

    add_custom_command(
        TARGET ${name}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ARGS ${_ExampleBinDir}/data
    )

    add_custom_command(
        TARGET ${name}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OpenCV_BIN_DIR}\\${OpenCV_BINS}" $<TARGET_FILE_DIR:${name}>
    )

    set(_ResourceRoot ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(_Resource ROOT "${IMGUI_NODE_EDITOR_ROOT_DIR}/../data" ${_Example_CommonResources} ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data" ${_Example_Resources})
        if (_Resource STREQUAL ROOT)
            set(_ResourceRoot FALSE)
            continue()
        elseif(NOT _ResourceRoot)
            set(_ResourceRoot ${_Resource})
            continue()
        endif()

        if ("${_Resource}" MATCHES "\.DS_Store$")
            list(REMOVE_ITEM _Example_Resources ${_Resource})
            list(REMOVE_ITEM _Example_CommonResources ${_Resource})
            continue()
        endif()

        file(RELATIVE_PATH _RelResource ${_ResourceRoot} ${_Resource})

        add_custom_command(
            TARGET ${name}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${_Resource} ${_ExampleBinDir}/data/${_RelResource}
        )
    endforeach()

endmacro()

add_subdirectory(application)

add_subdirectory(blueprints-example)
add_subdirectory(flow-image-app)

