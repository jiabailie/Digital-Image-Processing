#include "repixels.h"
#include "io.cpp"

#define DEBUG_OUTPUT
#undef	DEBUG_OUTPUT

const int p_value = 127;
const char* PROCESS_BOOL_SAVE_PATH1 = "..\\data\\dt_data.txt";
const char* PROCESS_BOOL_SAVE_PATH2 = "..\\data\\do_data.txt";
const char* PROCESS_BOOL_SAVE_PATH3 = "..\\data\\fake.txt";

void repixels(int dt_change, int do_change, int width, int height, long bmpSize, unsigned char* data)
{
	long i = 0;
	averagepixels(bmpSize, data);
	binarization(bmpSize, data);

	unsigned char* fake	   = (unsigned char*)malloc(sizeof(unsigned char) *  width * height);
	unsigned char* dt_data = (unsigned char*)malloc(sizeof(unsigned char) *  width * height);
	unsigned char* do_data = (unsigned char*)malloc(sizeof(unsigned char) *  width * height);

	singlizeimage(width, height, fake, data);
	filledges(width, height, fake);

	memcpy(dt_data, fake, sizeof(unsigned char) * width * height);
	memcpy(do_data, fake, sizeof(unsigned char) * width * height);

	for(i = 0; i <= dt_change; i++)
	{
		changepixels(DT_CHANGE_WHITE, width, height, bmpSize, dt_data);
	}
	changepixels(DO_CHANGE_WHITE, width, height, bmpSize, dt_data);

	for(i = 0; i <= do_change; i++)
	{
		changepixels(DO_CHANGE_WHITE, width, height, bmpSize, do_data);
	}

#ifdef DEBUG_OUTPUT
	writeToFile(2, width, height, dt_data, PROCESS_BOOL_SAVE_PATH1);
	writeToFile(2, width, height, do_data, PROCESS_BOOL_SAVE_PATH2);
#endif

	mergeimages(width, height, dt_data, do_data, fake);

	for(i = 0; i < REMOVE_LONELY_PIXEL; i++)
	{
		removelonelypixel(width, height, fake);
	}

	recoverimage(width, height, fake, data);	

	free(fake);
	free(dt_data);
	free(do_data);
}

void averagepixels(long bmpSize, unsigned char* data)
{
	long il = 0;
	unsigned char avg = 0;

	for(il = 0; il < bmpSize; il += 3)
	{
		avg = unsigned char(int((int(data[il]) + int(data[il + 1]) + int(data[il + 2])) / 3));
		data[il] = data[il + 1] = data[il + 2] = avg;
	}
}

void binarization(long bmpSize, unsigned char* data)
{
	long isize = 0;
	for(isize = 0; isize < bmpSize; isize++)
	{
		if(data[isize] > p_value)
		{
			data[isize] = 255;
		}
		else
		{
			data[isize] = 0;
		}
	}
}

void singlizeimage(int width, int height, unsigned char* fake, unsigned char* data)
{
	int i = 0;
	int j = 0;
	int tmp = 0;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			tmp = INDEX(i, j, width);
			fake[i * width + j] = data[tmp];
		}
	}
}

void changepixels(int kind, int width, int height, long bmpSize, unsigned char* data)
{
	int ih = 0;
	int iw = 0;
	int m = 0;
	int n = 0;
	int pos = 0;
	int cnt[2];

	unsigned char* fake = (unsigned char*)malloc(sizeof(unsigned char) * width * height);
	memcpy(fake, data, sizeof(unsigned char) * width * height);

	for(ih = 0; ih < height; ih++)
	{
		for(iw = 0; iw < width; iw++)
		{
			memset(cnt, int(0), sizeof(cnt));

			for(m = -1; m <= 1; m++)
			{
				for(n = -1; n <= 1; n++)
				{
					if(ih + m >= 0 && ih + m < height && iw + n >= 0 && iw + n < width)
					{
						pos = (ih + m) * width + (iw + n);
						if(fake[pos] == 255)
						{
							cnt[1]++;
						}
						else if(fake[pos] == 0)
						{
							cnt[0]++;
						}
					}
				}
			}
			pos = ih * width + iw;
			
			switch(kind)
			{
			case 0:
				// do not change white pixels
				data[pos] = cnt[0] < cnt[1] ? 255 : (data[pos] ? 255 : 0);
				break;
			case 1:
				// change white pixels
				data[pos] = cnt[0] < cnt[1] ? 255 : 0;
				break;
			default:
				break;
			}
		}
	}

	free(fake);	
}

void mergeimages(int width, int height, const unsigned char* dt_data, const unsigned char* do_data, unsigned char* data)
{
	int ih = 0;
	int iw = 0;
	int k = 0;
	int m = 0;
	int n = 0;
	int i_dt = 0;
	int i_do = 0;
	int tmp = 0;
	int dis[2];
	int cnt[2];

	for(ih = 0; ih < height; ih++)
	{
		for(iw = 0; iw < width; iw++)
		{
			tmp = ih * width + iw;
			if(dt_data[tmp] == do_data[tmp])
			{
				data[tmp] = dt_data[tmp];
			}
			else
			{
				memset(cnt, 0, sizeof(cnt));
				for(m = -1; m <= 1; m++)
				{
					for(n = -1; n <= 1; n++)
					{
						if(ih + m >= 0 && ih + m < height && iw + m >= 0 && iw + m < width)
						{
							k = (ih + m) * width + (iw + n);
							i_dt = dt_data[k] ? 1 : 0;
							i_do = do_data[k] ? 1 : 0;
							cnt[i_dt] += 1;
							cnt[i_do] += 1;
						}
					}
				}
				if(cnt[0] != cnt[1])
				{
					data[tmp] = cnt[0] > cnt[1] ? 0 : 255;
				}
				else
				{
					memset(dis, INT_MAX, sizeof(dis));
					findnearestpixel(ih, iw, height, width, dt_data[tmp], dis[0], dt_data);
					findnearestpixel(ih, iw, height, width, do_data[tmp], dis[1], do_data);

					if(dis[0] != dis[1])
					{
						data[tmp] = dis[0] < dis[1] ? dt_data[tmp] : do_data[tmp];
					}
					else
					{
						data[tmp] = BOOL_WHITE;
					}
				}
			}
		}
	}
}

void findnearestpixel(int ih, int iw, int height, int width, unsigned char pixel, int& dis, const unsigned char* data)
{
	int i = 0;
	int j = 0;
	int pos = 0;
	dis = INT_MAX;

	for(i = (-1) * MAX_RANGE; i <= MAX_RANGE; i++)
	{
		for(j = (-1) * MAX_RANGE; j <= MAX_RANGE; j++)
		{
			if(iABS(i) + iABS(j) < dis && ih + i >= 0 && ih + i < height && iw + j >= 0 && iw + j < width)
			{
				pos = (ih + i) * width + (iw + j);
				if(data[pos] == pixel)
				{
					dis = iABS(i) + iABS(j);
				}
			}
		}
	}
}

void recoverimage(int width, int height, const unsigned char* fake, unsigned char* data)
{
	int i = 0;
	int j = 0;
	int tmp = 0;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			tmp = INDEX(i, j, width);
			data[tmp] = data[tmp + 1] = data[tmp + 2] = fake[i * width + j];
		}
	}
}

// to one 3 by 3 block, if the center is one kind of pixel
// and the other 8 are another kind of pixel, then replace the center pixel
void removelonelypixel(int width, int height, unsigned char* data)
{
	int ih = 0;
	int iw = 0;
	int m = 0;
	int n = 0;
	int tmp = 0;
	int cnt[2];

	unsigned char* fake = (unsigned char*)malloc(sizeof(unsigned char) * width * height);
	memcpy(fake, data, sizeof(unsigned char) * width * height);

	for(ih = 1; ih < height - 1; ih++)
	{
		for(iw = 1; iw < width - 1; iw++)
		{
			memset(cnt, 0, sizeof(cnt));
			for(m = -1; m <= 1; m++)
			{
				for(n = -1; n <= 1; n++)
				{
					tmp = (ih + m) * width + (iw + n);
					tmp = int(fake[tmp]) == 0 ? 0 : 1;
					cnt[tmp] += 1;
				}
			}
			tmp = ih * width + iw;
			//if(iABS(cnt[0] - cnt[1]) == 7 && fake[tmp] != fake[tmp + 1])
			//{
			//	data[tmp] = fake[tmp + 1];
			//}
			if(cnt[0] == 8 && fake[tmp] == WHITE)
			{
				data[tmp] = BLACK;
			}
			if(cnt[1] == 8 && fake[tmp] == BLACK)
			{
				data[tmp] = WHITE;
			}
		}
	}

	free(fake);
}

// stuff the edge with background color (black)
void filledges(int width, int height, unsigned char* data)
{
	int i = 0;
	int j = 0;
	int tmp = 0;

	// top
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height && data[j * width + i] == WHITE; j++)
		{
			data[j * width + i] = BLACK;
		}
	}

	// bottom
	for(i = 0; i < width; i++)
	{
		for(j = height - 1; j >= 0 && data[j * width + i] == WHITE; j--)
		{
			data[j * width + i] = BLACK;
		}
	}

	// left
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width && data[i * width + j] == WHITE; j++)
		{
			data[i * width + j] = BLACK;
		}
	}

	// right
	for(i = 0; i < height; i++)
	{
		for(j = width - 1; j >= 0 && data[i * width + j] == WHITE; j--)
		{
			data[i * width + j] = BLACK;
		}
	}
}