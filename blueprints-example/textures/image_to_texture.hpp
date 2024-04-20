#pragma once
#include <opencv2/opencv.hpp>
//#include "imgui_impl_opengl3_loader.h"

bool image_to_texture(cv::Mat& cvImage, uint* out_texture, int* out_width, int* out_height);
void release_texture(uint* texture);
