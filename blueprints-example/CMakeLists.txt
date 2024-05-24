add_example_executable(blueprints-example
    blueprints-example.cpp
    utilities/builders.h
    utilities/drawing.h
    utilities/widgets.h
    utilities/builders.cpp
    utilities/drawing.cpp
    utilities/widgets.cpp
    nodes/base_nodes.cpp
    notifiers/Notifier.cpp
    addons/ImFileDialog.cpp
    #textures/image_to_texture.cpp

    nodes/child_nodes/image/image_draw.cpp
    nodes/child_nodes/image/image_source.cpp
    nodes/child_nodes/win32/win32_window.cpp
    nodes/child_nodes/win32/win32_softinput.cpp
)
target_include_directories(blueprints-example PRIVATE nodes)
target_include_directories(blueprints-example PRIVATE utilities)
