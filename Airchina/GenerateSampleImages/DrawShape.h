#ifndef _DRAW_SHAPE_H_
#define _DRAW_SHAPE_H_

#include "CommonMethods.h"

void drawShape(int dt_change, int do_change, int width, int height, long bmpSize, BYTE* data);

void find_black_thick_rectangle(int width, int height, vector<Point>& rec_left_top, vector<Point>& rec_right_bottom, BYTE* data);

void merge_black_rectangles(vector<Point>& rec_left_top, vector<Point>& rec_right_bottom);

void filledges(int width, int height, BYTE* data);

void changepixels(int kind, int width, int height, long bmpSize, BYTE* data);

void merge_two_kinds_images(int width, int height, const BYTE* dt_data, const BYTE* do_data, BYTE* data);

void find_nearest_same_pixel(int ih, int iw, int height, int width, BYTE pixel, int& dis, const BYTE* data);

void draw_black_rectangeles(vector<Point>& rec_left_top, vector<Point>& rec_right_bottom, int width, int height, BYTE* data);

void remove_lonely_pixel(int width, int height, BYTE* data);

void help_image_growing(int width, int height, BYTE* data);

void recover_image_one2three(int width, int height, const BYTE* fake, BYTE* data);

#endif
