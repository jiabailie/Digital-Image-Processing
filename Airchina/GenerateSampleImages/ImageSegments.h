#ifndef _IMAGE_SEGMENTS_H_
#define _IMAGE_SEGMENTS_H_

#include "GenerateSampleImages.h"

void image_segments_to_four(int width, int height, BYTE* data);

void image_sub_segments(int width, int height, int horizontal_start, int horizontal_end, BYTE* data, vector<Point>& sample_rec_left_top, vector<Point>& sample_rec_right_bottom);

void save_image_part_by_two_points(int rec_left_height, int rec_left_width, int rec_right_height, int rec_right_width, int width, BYTE* data);

void fill_image_white(int width, int height, BYTE* data, vector<Point>& sample_rec_left_top, vector<Point>& sample_rec_right_bottom);

#endif