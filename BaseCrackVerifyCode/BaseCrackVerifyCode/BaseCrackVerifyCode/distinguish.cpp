#include "stdafx.h"
#include "BaseCrackVerifyCode.h"

namespace CrackVerifyCode
{
	distinguish::distinguish(int width, int height, BYTE*& data, BYTE ret[CHARACTER_NUM], const char* openFile)
	{
#ifdef DEBUG_ERROR
		printf("%d,%d\n", width, height);

		char temPath[500] = "..\\data\\error_image_distinguish.txt";
		FILE* eFile = 0;

		fopen_s(&eFile, temPath, "wb");
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				fprintf(eFile, "%d\t", data[i * width + j]);
			}
			fprintf(eFile, "\n");
		}
		fclose(eFile);
#endif

		segment(width, height, data);

		int status = readtemplates(TEMPLATE_SAVE_PATH);

		for(int i = 0; i < CHARACTER_NUM; i++)
		{
			ret[i] = distinguishOneCharacterByCos(i, width, data, openFile); //distinguishOneCharacter(i, width, data, openFile);
		}
	}

	inline void 
	distinguish::segment(int width, int height, BYTE*& data)
	{
		int i = 0;
		int j = 0;
		int ct = 0;
		int s_horizon[CHARACTER_NUM];
		int e_horizon[CHARACTER_NUM];
		vector<int> rows;

#ifdef DEBUG_ERROR
		printf("width=%d,height=%d\n", width, height);
		printf("c_BLACK=%d\n", c_BLACK);
#endif

		for(i = 0; i < width; i++)
		{
			for(j = 0; j < height && data[j * width + i] < p_value; j++);
			if(j == height)
			{
				rows.push_back(i);
			}
#ifdef DEBUG_ERROR
		printf("%d,rows.size(%d)=%d\n",data[j * width + i], i, j);
#endif
		}
		if(rows[rows.size() - 1] != width - 1)
		{
			rows.push_back(width - 1);
		}

#ifdef DEBUG_ERROR
		printf("rows.size()=%d\n", rows.size());
#endif

		for(i = 1; i < int(rows.size()); i++)
		{
			if(rows[i] - rows[i - 1] >= MIN_WIDTH)
			{
				s_horizon[ct] = rows[i - 1];
				e_horizon[ct] = rows[i];
				subsegment(width, height, rows[i - 1], rows[i], data);
				ct++;
			}
#ifdef DEBUG_ERROR
		printf("%d,%d\n", rows[i - 1], rows[i]);
#endif
		}
#ifdef DEBUG_ERROR
		printf("%d\n", ct);
#endif
	}

	inline void 
	distinguish::subsegment(int width, int height, int h_start, int h_end, BYTE* data)
	{
		int i = 0;
		int j = 0;
		int lt_height = 0;
		int lt_width = h_start;
		int rb_height = height - 1;
		int rb_width = h_end;

		// find the black horizontal lines
		// from top to bottom to find the first top non-black line
		for(i = 0; i < height; i++)
		{
			for(j = h_start; j <= h_end && data[i * width + j] == c_BLACK; j++);
			if(j <= h_end)
			{
				lt_height = i;
				break;
			}
		}

		// from bottom to top to find the first bottom non-black line
		for(i = height - 1; i >= 0; i--)
		{
			for(j = h_start; j <= h_end && data[i * width + j] == c_BLACK; j++);
			if(j <= h_end)
			{
				rb_height = i;
				break;
			}
		}

		// find the black vertical lines
		// this is ommitted since this operation has been done in segment(...)
		// so if there are anything incorrect, we should modify that function

		// save the left-top and right-bottom points for this character
		left_top.push_back(Point(lt_height, lt_width));
		right_bottom.push_back(Point(rb_height, rb_width));
	}

	inline int
	distinguish::readtemplates(const char* path)
	{
		int status = FILE_ACCESS_STATUS_SUCCESS;
		char content[LineLength];
		ifstream fin(path);

		while(fin.getline(content, LineLength))
		{
			char name = content[0];
			double moments[MOMENTS_LENGTH] = {};

			common::transferData(content, name, moments);
			ctemplates.push_back(charTemplate(0, 0, BYTE(name), 0, moments));
		}		
		fin.clear();
		fin.close();

		return status;
	}

	inline void 
	distinguish::readtemplates_data(BYTE name, const char* path)
	{
		graphics _graphics;
		BMPIMAGE image = (bmpImage*)malloc(sizeof(bmpImage));
		BMPINFO info = (infoFetch*)malloc(sizeof(infoFetch));
		BYTE* data = 0;

		if(_graphics.Open(path, image, info) == FILE_ACCESS_STATUS_SUCCESS)
		{
			double moments[MOMENTS_LENGTH];
			BYTE* fake = (BYTE*)malloc(sizeof(BYTE) * info->bmpSize);
			data = (BYTE*)malloc(sizeof(BYTE) * info->w * info->h);

			memcpy(fake, info->data, sizeof(BYTE) * info->bmpSize);

			common::singlizeimage(info->w, info->h, data, fake);

			calRegularMoment(info->w, info->h, data, moments);
			ctemplates.push_back(charTemplate(info->w, info->h, name, data, moments));

			free(fake);
		}
		free(image);
		free(info);
	}

	inline void 
	distinguish::clearoneline(int s_col, int e_col, int s_row, int e_row, int width, BYTE* data)
	{
		int ih = 0;
		int iw = 0;

		for(ih = s_col; ih <= e_col; ih++)
		{
			for(iw = s_row; iw <= e_row; iw++)
			{
				data[ih * width + iw] = c_WHITE;
			}
		}
	}

	inline void 
	distinguish::remove_redunant_pixels(int width, int height, BYTE* data)
	{
		for(int i = 0; i < CHARACTER_NUM; i++)
		{
			remove_black_pixels(0, left_top[i].h - 1, left_top[i].w, right_bottom[i].w, width, data);
			remove_black_pixels(right_bottom[i].h + 1, height - 1, left_top[i].w, right_bottom[i].w, width, data);
		}
	}

	inline void 
	distinguish::remove_black_pixels(int vertical_start, int vertical_end, int horizontal_start, int horizontal_end, int width, BYTE* data)
	{
		int vertical = 0;
		int horizontal = 0;
		for(vertical = vertical_start; vertical <= vertical_end; vertical++)
		{
			for(horizontal = horizontal_start; horizontal <= horizontal_end; horizontal++)
			{
				data[vertical * width + horizontal] = c_WHITE;
			}
		}		
	}

	inline LL 
	distinguish::calMpq(int p, int q, int width, int height, BYTE* matrix)
	{
		LL ret = 0;
		int i = 0;
		int j = 0;

		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				if(matrix[i * width + j] == c_WHITE)
				{
					ret += common::calPow(i, p) * common::calPow(j, q);
				}
			}
		}

		return ret;
	}

	inline double 
	distinguish::calCenterMoment(int p, int q, int width, int height, BYTE* matrix)
	{
		LL i = 0, j = 0;
		LL U00 = 0, m10 = 0, m01 = 0;
		double Upq = 0;

		m10 = calMpq(1, 0, width, height, matrix);
		m01 = calMpq(0, 1, width, height, matrix);

		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; j++)
			{
				if(matrix[i * width + j] == c_WHITE)
				{
					Upq += double(common::calPow((i - m10), p) * common::calPow((j - m01), q));
					U00 += 1;
				}
			}
		}

		if(U00 == 0) { U00 = 1; }
		U00 = common::calPow(U00, (p + q + 2) >> 1);

		return double(Upq * 1.0 / U00);
	}

	inline void 
	distinguish::calRegularMoment(int width, int height, BYTE* matrix, double moments[MOMENTS_LENGTH])
	{
		double centerM[7] = {
			calCenterMoment(0, 2, width, height, matrix),
			calCenterMoment(0, 3, width, height, matrix),
			calCenterMoment(1, 1, width, height, matrix),
			calCenterMoment(1, 2, width, height, matrix),
			calCenterMoment(2, 0, width, height, matrix),
			calCenterMoment(2, 1, width, height, matrix),
			calCenterMoment(3, 0, width, height, matrix)	
		};

		double a1 = centerM[1] + centerM[4];
		double a2 = centerM[3] + centerM[6];
		double a3 = a2 * a2 - 3 * a1 * a1;
		double a4 = 3 * a2 * a2 - a1 * a1;
		double a5 = centerM[6] - 3 * centerM[3];
		double a6 = 3 * centerM[5] - centerM[1];
		double a7 = centerM[4] - centerM[0];

		double huM[7] = {
			iABS(centerM[0] + centerM[4]),
			a7 * a7 + 4 * centerM[2] * centerM[2],
			a5 * a5 + a6 * a6,
			a1 * a1 + a2 * a2,
			iABS(a2 * a3 * a5 + a1 * a4 * a6),
			iABS((a2 * a2 - a1 * a1) * a7 + 4 * centerM[2] * a1 * a2),
			iABS(a2 * a3 * a6 - a1 * a4 * a5)
		};

		double powHuM[4] = {};
		powHuM[0] = huM[0] * huM[0];
		powHuM[1] = powHuM[0] * huM[0];
		powHuM[2] = powHuM[1] * huM[0];
		powHuM[3] = powHuM[1] * powHuM[1];

		double lg2 = log((double)2);

		if(huM[0] != 0)
		{
			moments[0] = log(huM[1] * 1.0 / powHuM[0]) / lg2;
			moments[1] = log(huM[2] * 1.0 / powHuM[1]) / lg2;
			moments[2] = log(huM[3] * 1.0 / powHuM[1]) / lg2;
			moments[3] = log(huM[4] * 1.0 / powHuM[3]) / lg2;
			moments[4] = log(huM[5] * 1.0 / powHuM[2]) / lg2;
			moments[5] = log(huM[6] * 1.0 / powHuM[3]) / lg2;
		}
	}

	inline double 
	distinguish::calMomentSimilar(double moments1[MOMENTS_LENGTH], double moments2[MOMENTS_LENGTH])
	{
		double ret = 0;
		for(int i = 0; i < 6; i++)
		{
			ret += i * PI * 1000 * pow(moments1[i] - moments2[i], 2);
		}
		return ret;
	}

	inline double 
	distinguish::calMomentSimilarByCos(double moments1[MOMENTS_LENGTH], double moments2[MOMENTS_LENGTH])
	{
		double p1 = 0;
		double p2 = sqrt(common::calSquare(MOMENTS_LENGTH, moments1));
		double p3 = sqrt(common::calSquare(MOMENTS_LENGTH, moments2));

		for(int i = 0; i < MOMENTS_LENGTH; i++)
		{
			p1 += moments1[i] * moments2[i];
		}
		if(p2 == 0 || p3 == 0) { return 0; }

		return (p1 * 100.0) / (p2 * p3);
	}

	BYTE 
	distinguish::distinguishOneCharacterByCos(int index, int width, BYTE* data, const char* openFile)
	{
		int i = 0;
		int j = 0;
		int cnt = 0;
		BYTE ret = 'a';
		double tmp = 0;
		double dis_max = INT_MIN;

		int i_width = right_bottom[index].w - left_top[index].w + 1;
		int i_height = right_bottom[index].h - left_top[index].h + 1;

		double selfMoments[MOMENTS_LENGTH];

		BYTE* i_data = (BYTE*)malloc(sizeof(BYTE) * i_width * i_height);

		for(i = left_top[index].h; i <= right_bottom[index].h; i++)
		{
			for(j = left_top[index].w; j <= right_bottom[index].w; j++)
			{
				i_data[cnt] = data[i * width + j];
				cnt++;
			}
		}

		calRegularMoment(i_width, i_height, i_data, selfMoments);
		
		tmp = dis_max = calMomentSimilarByCos(selfMoments, ctemplates[0].moments);
		ret = ctemplates[0].name;

#ifdef	DEBUG_FIND_PROPER_DISTANCE
		char distance_path[PathLength];
		strcpy_s(distance_path, SAVE_PROPER_DISTANCE);

		FILE* fp = 0;
		fp = fopen(distance_path, "at");
		
		if(NULL == fp)
		{
			printf("proper distance record file not exist.\n");
		}

		fprintf(fp, "source path=%s, source character's index=%d, compare character=%c, distance=%.15f\n", openFile, index, ret, tmp);
#endif

//		if(tmp - 1.0 < 1e-18)
//		{
//#ifdef	DEBUG_FIND_PROPER_DISTANCE
//
//			fclose(fp);
//#endif
//			return ret;
//		}

		for(i = 1; i < int(ctemplates.size()); i++)
		{
			tmp = calMomentSimilarByCos(selfMoments, ctemplates[i].moments);
#ifdef	DEBUG_FIND_PROPER_DISTANCE

			fprintf(fp, "source path=%s, source character's index=%d, compare character=%c, distance=%.15f\n", openFile, index, ctemplates[i].name, tmp);
#endif
			if(tmp > dis_max)
			{
				dis_max = tmp;
				ret = ctemplates[i].name;
			}
//			if(tmp - 1.0 < 1e-18)
//			{
//#ifdef	DEBUG_FIND_PROPER_DISTANCE
//
//				fclose(fp);
//#endif
//				return ret;
//			}
		}
#ifdef	DEBUG_FIND_PROPER_DISTANCE

		fclose(fp);
#endif
		return ret;
	}

	BYTE 
	distinguish::distinguishOneCharacter(int index, int width, BYTE* data, const char* openFile)
	{
		int i = 0;
		int j = 0;
		int cnt = 0;
		int itmp = 0;
		bool flg = false;
		BYTE ret = 'a';
		BYTE c_min = 'a';
		double tmp = 0;
		double max = INT_MIN;
		double momentSimilar = INT_MAX;
		map<char, int> m_distance;
		map<char, int> m_count_kinds;

#ifdef DEBUG_ERROR
		printf("%d,%d,%d\n", index, int(right_bottom.size()), int(left_top.size()));

		if(int(right_bottom.size()) != 4 || int(left_top.size()) != 4)
		{
			printf("why this is happened!\n");
			return '*';
		}
#endif

		int i_width = right_bottom[index].w - left_top[index].w + 1;
		int i_height = right_bottom[index].h - left_top[index].h + 1;

		double selfMoments[MOMENTS_LENGTH];

		BYTE* i_data = (BYTE*)malloc(sizeof(BYTE) * i_width * i_height);

		for(i = left_top[index].h; i <= right_bottom[index].h; i++)
		{
			for(j = left_top[index].w; j <= right_bottom[index].w; j++)
			{
				i_data[cnt] = data[i * width + j];
				cnt++;
			}
		}		

		calRegularMoment(i_width, i_height, i_data, selfMoments);

		tmp = momentSimilar = calMomentSimilar(selfMoments, ctemplates[0].moments);
		ret = c_min = ctemplates[0].name;

#ifdef	DEBUG_FIND_PROPER_DISTANCE
		char distance_path[PathLength];
		strcpy_s(distance_path, SAVE_PROPER_DISTANCE);

		FILE* fp = 0;
		fp = fopen(distance_path, "at");
		
		if(NULL == fp)
		{
			printf("proper distance record file not exist.\n");
		}

		fprintf(fp, "source path=%s, source character's index=%d, compare character=%c, distance=%.15f\n", openFile, index, ret, tmp);
#endif

		for(i = 0; i < int(ctemplates.size()); i++)
		{
			tmp = calMomentSimilar(selfMoments, ctemplates[i].moments);

#ifdef	DEBUG_FIND_PROPER_DISTANCE

			fprintf(fp, "source path=%s, source character's index=%d, compare character=%c, distance=%.15f\n", openFile, index, ctemplates[i].name, tmp);

#endif
			if(tmp < momentSimilar)
			{
				c_min = ctemplates[i].name;
				//ret = ctemplates[i].name;
				momentSimilar = tmp;
			}

			flg = false;
			for(map<char, int>::iterator it = m_count_kinds.begin(); it != m_count_kinds.end() && !flg; it++)
			{
				flg = true;
			}

			if(flg)
			{
				m_count_kinds[ctemplates[i].name]++;
			}
			else
			{
				m_count_kinds[ctemplates[i].name] = 1;
				m_distance[ctemplates[i].name] = 0;
			}

			// if current distance is less than minimum value
			if(tmp < PROPER_DISTANCE)
			{
				m_distance[ctemplates[i].name]++;
			}
		}

		if(m_distance.size() > 0)
		{
			max = INT_MIN;
			for(map<char, int>::iterator it = m_distance.begin(); it != m_distance.end(); it++)
			{
				tmp = 1.0 * (*it).second / (m_count_kinds[(*it).first]);
				if(tmp > max)
				{
					max = tmp;
					ret = (*it).first;
				}
			}
		}

		ret = c_min; // if keep the return character has the nearest distance with the input image shape

		if(ret == 'a')
		{
			printf("\nThere is an 'a'.\n");
		}

#ifdef	DEBUG_FIND_PROPER_DISTANCE

		fclose(fp);
#endif
		free(i_data);

		return ret;
	}
}