#include "image_to_texture.hpp"
// #include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>

bool image_to_texture(cv::Mat &cvImage, uint *out_texture, int *out_width, int *out_height)
{
    if (cvImage.empty())
        return false;
    if (cvImage.channels() == 1)
        cv::cvtColor(cvImage, cvImage, cv::COLOR_GRAY2RGBA);
    else if (cvImage.channels() == 3)
        cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGBA);

    auto image_width = cvImage.cols;
    auto image_height = cvImage.rows;
    auto image_data = cvImage.data;

    // Create a OpenGL texture identifiers
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

void release_texture(uint *texture)
{
    glDeleteTextures(1, texture);
}
