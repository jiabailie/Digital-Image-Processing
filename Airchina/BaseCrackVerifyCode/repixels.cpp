#include "stdafx.h"
#include "BaseCrackVerifyCode.h"

namespace CrackVerifyCode
{
 int grow_direction[IMAGE_GROW_DIMES][4] = {
         { 0,  -1,  0,  1}, 
         {-1,   0,  1,  0}, 
         {-1,  -1,  1,  1}, 
         {-1,   1,  1, -1}
         //{-1,   0,  0,  1}, 
         //{ 0,  -1, -1,  0}, 
         //{ 0,  -1,  1,  0}, 
         //{ 1,   0,  0,  1},
         //{-1,  -1, -1,  1}, 
         //{-1,  -1,  1, -1}, 
         //{ 1,  -1,  1,  1}, 
         //{ 1,   1, -1,  1}
       };

 repixels::repixels(int dt_change, int do_change, int width, int height, long bmpSize, BYTE* data, BYTE ret[CHARACTER_NUM], const char* openFile)
 {
  long i = 0; 

  vector<Point> left_top;
  vector<Point> right_bottom;

  averagepixels(bmpSize, data);
  common::binarization(bmpSize, data);

  BYTE* fake     = (BYTE*)malloc(sizeof(BYTE) * width * height);
  BYTE* dt_data  = (BYTE*)malloc(sizeof(BYTE) * width * height);
  BYTE* do_data  = (BYTE*)malloc(sizeof(BYTE) * width * height);

  common::singlizeimage(width, height, fake, data);

  find_thick_rectangle(width, height, left_top, right_bottom, fake);
  merge_rectangles(left_top, right_bottom);

  filledges(width, height, fake);

  memcpy(dt_data, fake, sizeof(BYTE) * width * height);
  memcpy(do_data, fake, sizeof(BYTE) * width * height);

  for(i = 0; i <= dt_change; i++)
  {
   changepixels(DT_CHANGE_WHITE, width, height, bmpSize, dt_data);
  }
  changepixels(DO_CHANGE_WHITE, width, height, bmpSize, dt_data);

  for(i = 0; i <= do_change; i++)
  {
   changepixels(DO_CHANGE_WHITE, width, height, bmpSize, do_data);
  }

  mergeimages(width, height, dt_data, do_data, fake);

  draw_rectangeles(left_top, right_bottom, width, height, fake);

  for(i = 0; i < REMOVE_LONELY_PIXEL; i++)
  {
   removelonelypixel(width, height, fake);
  }

  for(i = 0; i < IMAGE_GROW_TIMES; i++)
  {
   imagegrowing(width, height, fake);
  }

#ifdef DEBUG_ERROR
  char temPath[500] = "..\\data\\error_image_repixels.txt";
  FILE* eFile = 0;

  fopen_s(&eFile, temPath, "wb");
  for(int i = 0; i < height; i++)
  {
   for(int j = 0; j < width; j++)
   {
    fprintf(eFile, "%d\t", fake[i * width + j]);
   }
   fprintf(eFile, "\n");
  }
  fclose(eFile);
#endif

  distinguish(width, height, fake, ret, openFile);

  free(fake);
  free(dt_data);
  free(do_data);
 }

 void repixels::averagepixels(long bmpSize, BYTE* data)
 {
  long il = 0;
  BYTE avg = 0;

  for(il = 0; il < bmpSize; il += 3)
  {
   avg = BYTE(int((int(data[il]) + int(data[il + 1]) + int(data[il + 2])) / 3));
   data[il] = data[il + 1] = data[il + 2] = avg;
  }
 }

 void repixels::changepixels(int kind, int width, int height, long bmpSize, BYTE* data)
 {
  int ih = 0;
  int iw = 0;
  int m = 0;
  int n = 0;
  int pos = 0;
  int cnt[2];

  BYTE* fake = (BYTE*)malloc(sizeof(BYTE) * width * height);
  memcpy(fake, data, sizeof(BYTE) * width * height);

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

 void repixels::mergeimages(int width, int height, const BYTE* dt_data, const BYTE* do_data, BYTE* data)
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

 void repixels::findnearestpixel(int ih, int iw, int height, int width, BYTE pixel, int& dis, const BYTE* data)
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

 void repixels::recoverimage(int width, int height, const BYTE* fake, BYTE* data)
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
 void repixels::removelonelypixel(int width, int height, BYTE* data)
 {
  int ih = 0;
  int iw = 0;
  int m = 0;
  int n = 0;
  int tmp = 0;
  int cnt[2];

  BYTE* fake = (BYTE*)malloc(sizeof(BYTE) * width * height);
  memcpy(fake, data, sizeof(BYTE) * width * height);

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
 void repixels::filledges(int width, int height, BYTE* data)
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

 // toward the black pixels, if there are two white pixels lies at its 
 // (west, east), (north, south), (north-west, south-east), (north-east, south-west)
 // change the black to white
 //******************************************
 // (x - 1, y - 1) (x - 1, y) (x - 1, y + 1)
 // (x,     y - 1) (x,     y) (x,     y + 1)
 // (x + 1, y - 1) (x + 1, y) (x + 1, y + 1)
 //******************************************
 // (x - 1, y - 1), (x + 1, y + 1)
 // (x    , y - 1), (x    , y + 1)
 // (x + 1, y - 1), (x - 1, y + 1)
 // (x - 1, y    ), (x + 1, y    )
 //******************************************
 // (x - 1, y    ), (x,     y + 1)
 // (x,     y - 1), (x - 1, y    )
 // (x,     y - 1), (x + 1, y    )
 // (x + 1, y    ), (x,     y + 1)
 //******************************************
 // (x - 1, y - 1), (x - 1, y + 1)
 // (x - 1, y - 1), (x + 1, y - 1)
 // (x + 1, y - 1), (x + 1, y + 1)
 // (x + 1, y + 1,) (x - 1, y + 1)
 void repixels::imagegrowing(int width, int height, BYTE* data)
 {
  int ih = 0;
  int iw = 0;
  int j = 0;
  int tmp = 0;
  int t[2];

  for(ih = 1; ih < height - 1; ih++)
  {
   for(iw = 1; iw < width - 1; iw++)
   {
     tmp = ih * width + iw;
     if(data[tmp] == c_BLACK)
     {
      for(j = 0; j < IMAGE_GROW_DIMES; j++)
      {
       t[0] = (ih + grow_direction[j][0]) * width + (iw + grow_direction[j][1]);
       t[1] = (ih + grow_direction[j][2]) * width + (iw + grow_direction[j][3]);

       if(data[t[0]] == c_WHITE && data[t[1]] == c_WHITE)
       {
        data[tmp] = c_WHITE;
        break;
       }
      }
     }
   }
  }
 }

 // if there are black rectangle which size is m by n and {m >= 2 and n >= 2}
 // through the observation, the size of black rectangle should be one part of the characters
 void repixels::find_thick_rectangle(int width, int height, vector<Point>& left_top, vector<Point>& right_bottom, BYTE* data)
 {
  int m = 0;
  int n = 0;
  int ih = 0;
  int iw = 0;
  int jh = 0;
  int jw = 0;
  int cnt = 0;
  bool flg = true;

  for(ih = 0; ih <= height - 2; ih++)
  {
   for(iw = 0; iw <= width - 2; iw++)
   {
    flg = true;
    for(jh = ih + 1; jh < height && flg; jh++)
    {
     for(jw = iw + 1; jw < width && flg; jw++)
     {
      cnt = 0;
      for(m = ih; m <= jh && flg; m++)
      {
       for(n = iw; n <= jw && flg; n++)
       {
        if(data[m * width + n] == c_WHITE)
        {
         flg = false;
         continue;
        }
        cnt++;
       }
      }
      if(cnt == (jh - ih + 1) * (jw - iw + 1))
      {
       left_top.push_back(Point(ih, iw));
       right_bottom.push_back(Point(jh, jw));
      }
     }
    }
   }
  }
 }

 void repixels::merge_rectangles(vector<Point>& left_top, vector<Point>& right_bottom)
 {
  int i = 0;
  int j = 0;
  int len = int(left_top.size());
  bool* valid = (bool*)malloc(sizeof(bool) * len);;
  vector<Point> t_left_top(left_top.begin(), left_top.end());
  vector<Point> t_right_bottom(right_bottom.begin(), right_bottom.end());

  Point lt;
  Point rb;

  memset(valid, true, sizeof(bool) * len);

  left_top.clear();
  right_bottom.clear();

  for(i = 0; i < len; i++)
  {
   if(valid[i])
   {
    valid[i] = false;
    lt = t_left_top[i];
    rb = t_right_bottom[i];
    for(j = i; j < len; j++)
    {
     // current rectangle is contained in another larger rectangle
     if(valid[j] && t_left_top[j].h <= lt.h && t_left_top[j].w <= lt.w && t_right_bottom[j].h >= rb.h && t_right_bottom[j].w >= rb.w)
     {
      valid[j] = false;
      lt = t_left_top[j];
      rb = t_right_bottom[j];
     }
     // current rectangle contains another smaller rectangle
     if(valid[j] && t_left_top[j].h >= lt.h && t_left_top[j].w >= lt.w && t_right_bottom[j].h <= rb.h && t_right_bottom[j].w <= rb.w)
     {
      valid[j] = false;
     }
    }
    left_top.push_back(lt);
    right_bottom.push_back(rb);
   }
  }
 }

 void repixels::draw_rectangeles(vector<Point>& left_top, vector<Point>& right_bottom, int width, int height, BYTE* data)
 {
  int i = 0;
  int ih = 0;
  int iw = 0;
  int len = int(left_top.size());

  for(i = 0; i < len; i++)
  {
   for(ih = left_top[i].h; ih <= right_bottom[i].h; ih++)
   {
    for(iw = left_top[i].w; iw <= right_bottom[i].w; iw++)
    {
     data[ih * width + iw] = c_WHITE;
    }
   }
  }
 }
}
