#include "StdAfx.h"
#include "ImageSegments.h"
#include "CommonMethods.h"
#include "ReadVerifyCode.h"

#define DEBUG_SEGMENT
#undef	DEBUG_SEGMENT

void image_segments_to_four(int width, int height, BYTE* data)
{
	int ct = 0;
	int i = 0, j = 0;
	vector<int> rows;
	int s_horizon[CHARACTER_NUM];
	int e_horizon[CHARACTER_NUM];
	vector<Point> sample_rec_left_top;
	vector<Point> sample_rec_right_bottom;

	BYTE* cdata = (BYTE*)malloc(sizeof(BYTE) * width * height);
	singlizeimage(width, height, cdata, data);

	// find the pure black vertical lines' positions
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height && cdata[j * width + i] == c_BLACK; j++);
		if(j == height)
		{
			rows.push_back(i);
		}
	}

	if(rows[rows.size() - 1] != width - 1)
	{
		rows.push_back(width - 1);
	}

	for(i = 1; i < int(rows.size()); i++)
	{
		if(rows[i] - rows[i - 1] >= MIN_WIDTH)
		{
			s_horizon[ct] = rows[i - 1];
			e_horizon[ct] = rows[i];
			
			image_sub_segments(width, height, rows[i - 1], rows[i], cdata, sample_rec_left_top, sample_rec_right_bottom);
			ct++;
		}
	}

#ifdef DEBUG_SEGMENT

	char debugPath[PathLength];
	strcpy_s(debugPath, SAMPLE_CHAR_DEBUG);

	fill_image_white(width, height, data, sample_rec_left_top, sample_rec_right_bottom);

	Save(debugPath, width, height, data);

	free(cdata);
	return;

#endif

	for(i = 0; i < CHARACTER_NUM; i++)
	{
		save_image_part_by_two_points(sample_rec_left_top[i].h, sample_rec_left_top[i].w, sample_rec_right_bottom[i].h, sample_rec_right_bottom[i].w, width, cdata);
	}

	free(cdata);
}

void image_sub_segments(int width, int height, int horizontal_start, int horizontal_end, BYTE* data, vector<Point>& sample_rec_left_top, vector<Point>& sample_rec_right_bottom)
{
	int i = 0, j = 0;
	int lt_height = 0;
	int lt_width = horizontal_start;
	int rb_height = height - 1;
	int rb_width = horizontal_end;

	for(i = 0; i < height; i++)
	{
		for(j = horizontal_start; j <= horizontal_end && data[i * width + j] == c_BLACK; j++);
		if(j <= horizontal_end)
		{
			lt_height = i;
			break;
		}
	}

	for(i = height - 1; i >= 0; i--)
	{
		for(j = horizontal_start; j <= horizontal_end && data[i * width + j] == c_BLACK; j++);
		if(j <= horizontal_end)
		{
			rb_height = i;
			break;
		}
	}

	sample_rec_left_top.push_back(Point(lt_height, lt_width));
	sample_rec_right_bottom.push_back(Point(rb_height, rb_width));
}

// if the real width cannot be divided by 4, the width has to be increased to a number which can be divided by 4
void save_image_part_by_two_points(int rec_left_height, int rec_left_width, int rec_right_height, int rec_right_width, int width, BYTE* data)
{
	int i = 0;
	int j = 0;
	int rem = 0;
	int cur = 0;
	int tmp = 0;
	clock_t timer = clock();
	char savePath[PathLength];

	vector<long> v_times;

	int s_width = rec_right_width - rec_left_width + 1;
	int s_height = rec_right_height - rec_left_height + 1;

	while((s_width + rem) % 4 != 0) { rem++; }

	BYTE* image_part = (BYTE*)malloc(sizeof(BYTE) * (s_width + rem) * s_height * 3);

	strcpy_s(savePath, SAMPLE_CHAR_OUTPUT);

	v_times.push_back(long(timer));

	SpellSaveFilePathByNumbers(BMPTYPE, v_times, savePath);

	for(i = 0; i < s_height; i++)
	{
		for(j = 0; j < s_width; j++)
		{
			cur = INDEX(i, j, (s_width + rem));
			tmp = (rec_left_height + i) * width + (rec_left_width + j);
			image_part[cur] = image_part[cur + 1] = image_part[cur + 2] = data[tmp];
			cur += 3;
		}
		for(j = 0; j < rem; j++)
		{
			image_part[cur] = image_part[cur + 1] = image_part[cur + 2] = c_BLACK;
			cur += 3;
		}
	}

	Save(savePath, s_width + rem, s_height, image_part);

	free(image_part);
}

void fill_image_white(int width, int height, BYTE* data, vector<Point>& sample_rec_left_top, vector<Point>& sample_rec_right_bottom)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int tmp = 0;
	bool flg = true;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			flg = true;
			for(k = 0; k < int(sample_rec_left_top.size()) && flg; k++)
			{
				if(i >= sample_rec_left_top[k].h && 
				   i <= sample_rec_right_bottom[k].h &&
				   j >= sample_rec_left_top[k].w &&
				   j <= sample_rec_right_bottom[k].w
				  )
				{
					flg = false;
				}
			}
			if(flg)
			{
				tmp = INDEX(i, j, width);

				data[tmp] = data[tmp + 1] = data[tmp + 2] = c_WHITE;
			}
		}
	}	
}