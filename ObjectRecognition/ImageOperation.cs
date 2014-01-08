#define DEBUG_DRAW
//#undef  DEBUG_DRAW
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;
using ObjectRecognition.Config;

namespace ObjectRecognition
{
    public class ImageOperation
    {
        #region Base operations

        /// <summary>
        /// Calculate the difference of two color, which equals to abs(R)+abs(G)+abs(B).
        /// </summary>
        /// <param name="c1"></param>
        /// <param name="c2"></param>
        /// <returns></returns>
        public static int calDiff(Color c1, Color c2)
        {
            return Math.Abs(c1.R - c2.R) + Math.Abs(c1.G - c2.G) + Math.Abs(c1.B - c2.B);
        }

        /// <summary>
        /// Calculate the distance of two different points.
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <returns></returns>
        public static double calPointDistance(iPoint p1, iPoint p2)
        {
            return Math.Sqrt(
                Math.Pow(p1.Width_Position - p2.Width_Position, 2)
                +
                Math.Pow(p1.Height_Position - p2.Height_Position, 2)
                );
        }

        /// <summary>
        /// Transfer the source image to bmp format.
        /// </summary>
        /// <param name="sourcePath"></param>
        /// <returns></returns>
        public static Bitmap saveTo24BMP(string sourcePath)
        {
            return generateBMP(ImageType.ThreeBit, sourcePath);
        }

        /// <summary>
        /// The sub-function to generate different format of BMP image.
        /// </summary>
        /// <param name="format"></param>
        /// <param name="sourcePath"></param>
        /// <returns></returns>
        public static Bitmap generateBMP(ImageType format, string sourcePath)
        {
            Bitmap destinationBMP = null;
            Bitmap sourceImage = new Bitmap(Image.FromFile(sourcePath));
            switch (format)
            {
                case ImageType.SingleBit:
                    destinationBMP = new Bitmap(sourceImage.Width, sourceImage.Height, System.Drawing.Imaging.PixelFormat.Format1bppIndexed);
                    break;
                case ImageType.ThreeBit:
                    destinationBMP = new Bitmap(sourceImage.Width, sourceImage.Height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
                    break;
                default:
                    break;
            }

            using (var graph = Graphics.FromImage(destinationBMP))
            {
                graph.DrawImage(
                    sourceImage,
                    new Rectangle(0, 0, sourceImage.Width, sourceImage.Height)
                );
            }

            return destinationBMP;
        }

        /// <summary>
        /// Judge whether the input point is a white point.
        /// </summary>
        /// <param name="color"></param>
        /// <returns></returns>
        public static bool judgeWhitePoint(Color color)
        {
            bool ret = true;

            if (color.R != 255 || color.G != 255 || color.B != 255)
            {
                ret = false;
            }
            return ret;
        }

        /// <summary>
        /// According to the point information to find the left-top and right-bottom point.
        /// </summary>
        /// <param name="currentCC"></param>
        private static void FormatConnectedComponent(ConnectedComponent currentCC)
        {
            int minWidth = int.MaxValue;
            int maxWidth = int.MinValue;
            int minHeight = int.MaxValue;
            int maxHeight = int.MinValue;

            foreach (iPoint point in currentCC.PointSet)
            {
                if (point.Width_Position < minWidth)
                {
                    minWidth = point.Width_Position;
                }
                if (point.Width_Position > maxWidth)
                {
                    maxWidth = point.Width_Position;
                }
                if (point.Height_Position < minHeight)
                {
                    minHeight = point.Height_Position;
                }
                if (point.Height_Position > maxHeight)
                {
                    maxHeight = point.Height_Position;
                }
            }

            currentCC.Left_Top = new iPoint(minWidth, minHeight);
            currentCC.Right_Bottom = new iPoint(maxWidth, maxHeight);
        }

        /// <summary>
        /// Calculate the distance of two connected components.
        /// </summary>
        /// <param name="c1"></param>
        /// <param name="c2"></param>
        /// <returns></returns>
        private static double calculateDistanceBetweenConnectedComponents(ConnectedComponent c1, ConnectedComponent c2)
        {
            double tmp = 0;
            double distance = double.MaxValue;

            foreach (iPoint point1 in c1.PointSet)
            {
                foreach (iPoint point2 in c2.PointSet)
                {
                    tmp = calPointDistance(point1, point2);
                    if (tmp < distance)
                    {
                        distance = tmp;
                    }
                }
            }

            return distance;
        }

        /// <summary>
        /// Calculate the size of certain connected component.
        /// </summary>
        /// <param name="cc"></param>
        /// <returns></returns>
        private static int calConnectedComponentsSize(ConnectedComponent cc)
        {
            int width = cc.Right_Bottom.Width_Position - cc.Left_Top.Width_Position;
            int height = cc.Right_Bottom.Height_Position - cc.Left_Top.Height_Position;

            return width * height;
        }

        /// <summary>
        /// Judge whether two connected components are similar in color.
        /// </summary>
        /// <param name="c1"></param>
        /// <param name="c2"></param>
        /// <returns></returns>
        private static bool judgeConnectedComponentsSimilar(ConnectedComponent c1, ConnectedComponent c2, Bitmap sourceImage)
        {
            bool flag = true;

            for (int i = 0; i < c1.PointSet.Count && flag; i++)
            {
                for (int j = 0; j < c2.PointSet.Count && flag; j++)
                {
                    if (calDiff(
                                sourceImage.GetPixel(c1.PointSet[i].Width_Position, c1.PointSet[i].Height_Position),
                                sourceImage.GetPixel(c2.PointSet[j].Width_Position, c2.PointSet[j].Height_Position)
                                ) > Config.ConfigParameters.DiffThreshold
                       )
                    {
                        flag = false;
                    }
                }
            }

            return flag;
        }

        /// <summary>
        /// Judge whether two connected components are near enough.
        /// </summary>
        /// <param name="c1"></param>
        /// <param name="c2"></param>
        /// <returns></returns>
        private static bool judgeConnectedComponentsNear(ConnectedComponent c1, ConnectedComponent c2)
        {
            double distance = calculateDistanceBetweenConnectedComponents(c1, c2);

            return distance - Config.ConfigParameters.DistanceThreshold < 0.00001 ? true : false;
        }

        #endregion

        #region Pre-processing operations

        /// <summary>
        /// According to the threshold value, change the background to pure white.
        /// </summary>
        /// <param name="sourceImage"></param>
        public static void changeBackground(Bitmap sourceImage)
        {
            int i = 0;
            int j = 0;
            int avg = 0;
            int width = sourceImage.Width;
            int height = sourceImage.Height;

            for (i = 0; i < width; i++)
            {
                for (j = 0; j < height; j++)
                {
                    avg = (sourceImage.GetPixel(i, j).R + sourceImage.GetPixel(i, j).G + sourceImage.GetPixel(i, j).B) / 3;
                    if (avg > Config.ConfigParameters.ThresHold)
                    {
                        sourceImage.SetPixel(i, j, Color.FromArgb(255, 255, 255));
                    }
#if DEBUG_DRAW
                    else
                    {
                        sourceImage.SetPixel(i, j, Color.FromArgb(0, 0, 0));
                    }
#endif
                }
            }
        }

        /// <summary>
        /// Towards anyone pixel, change its value to the nearest neighbour.
        /// </summary>
        /// <param name="sourceImage"></param>
        public static void sameNearbyPixel(Bitmap sourceImage)
        {
            int i = 0, j = 0;
            int m = 0, n = 0;
            int tmpDiff = 0;
            int minDiff = int.MaxValue;
            int width = sourceImage.Width;
            int height = sourceImage.Height;
            Color minColor = Color.FromArgb(255, 255, 255);

            for (i = 1; i < width - 1; i++)
            {
                for (j = 1; j < height - 1; j++)
                {
                    minDiff = int.MaxValue;

                    for (m = -1; m <= 1; m++)
                    {
                        for (n = -1; n <= 1; n++)
                        {
                            if (!(m == 0 && n == 0))
                            {
                                tmpDiff = calDiff(sourceImage.GetPixel(i, j), sourceImage.GetPixel(i + m, j + n));
                                if (tmpDiff < minDiff)
                                {
                                    minDiff = tmpDiff;
                                    minColor = Color.FromArgb(
                                        sourceImage.GetPixel(i + m, j + n).R,
                                        sourceImage.GetPixel(i + m, j + n).G,
                                        sourceImage.GetPixel(i + m, j + n).B
                                    );
                                }
                            }
                        }
                    }

                    sourceImage.SetPixel(i, j, Color.FromArgb(minColor.R, minColor.G, minColor.B));
                }
            }
        }

        /// <summary>
        /// Similaring the vertical nearby pixels.
        /// </summary>
        /// <param name="sourceImage"></param>
        public static void sameNeighbourNearbyPixel(Bitmap sourceImage)
        {
            bool changed = false;
            int i = 0, j = 0, k = 0;
            int width = sourceImage.Width;
            int height = sourceImage.Height;
            Bitmap tmpImage = new Bitmap(sourceImage);
            int[,] factorArray = new int[4, 4] {
                { 0, -1, 0, 1 }, 
                { -1, 0, 1, 0 }, 
                { -1, -1, 1, 1 }, 
                { -1, 1, 1, -1 } 
            };

            for (i = 1; i < width - 1; i++)
            {
                for (j = 1; j < height - 1; j++)
                {
                    changed = false;
                    for (k = 0; k < 4 && !changed; k++)
                    {
                        Color top = tmpImage.GetPixel(i + factorArray[k, 0], j + factorArray[k, 1]);
                        Color bottom = tmpImage.GetPixel(i + factorArray[k, 2], j + factorArray[k, 3]);

                        if (!(top.R == 255 && top.G == 255 && top.B == 255 &&
                              bottom.R == 255 && bottom.G == 255 && bottom.B == 255))
                        {
                            if (calDiff(top, bottom) <= ConfigParameters.DiffThreshold)
                            {
                                sourceImage.SetPixel(i, j, Color.FromArgb(
                                    (top.R + bottom.R) / 2,
                                    (top.G + bottom.G) / 2,
                                    (top.B + bottom.B) / 2)
                                );
                                changed = true;
                            }
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Find the left-top and right-bottom points from Black-White image.
        /// </summary>
        /// <param name="sourceImage"></param>
        /// <param name="left_top"></param>
        /// <param name="right_bottom"></param>
        public static void findDiagonalPoints(Bitmap sourceImage, iPoint left_top, iPoint right_bottom)
        {
            int i = 0;
            int j = 0;
            int minWidth = int.MaxValue;
            int minHeight = int.MaxValue;
            int maxWidth = int.MinValue;
            int maxHeight = int.MinValue;

            for (i = 0; i < sourceImage.Width; i++)
            {
                for (j = 0; j < sourceImage.Height; j++)
                {
                    Color c = sourceImage.GetPixel(i, j);
                    if (c.R != 255 || c.G != 255 || c.B != 255)
                    {
                        if (i < minWidth) { minWidth = i; }
                        if (i > maxWidth) { maxWidth = i; }
                        if (j < minHeight) { minHeight = j; }
                        if (j > maxHeight) { maxHeight = j; }
                    }
                }
            }
            left_top.Height_Position = minHeight;
            left_top.Width_Position = minWidth;
            right_bottom.Height_Position = maxHeight;
            right_bottom.Width_Position = maxWidth;
        }

        #endregion

        #region Operations which related with connected components

        /// <summary>
        /// Find all connected-components in the image and return related information.
        /// </summary>
        /// <param name="sourceImage"></param>
        /// <returns></returns>
        public static List<ConnectedComponent> findAllConnectedComponents(Bitmap sourceImage)
        {
            int i = 0, j = 0;

            int width = sourceImage.Width;
            int height = sourceImage.Height;

            // The array which tells whether the point at position[i,j] has been visited.
            bool[,] visited = new bool[width, height];

            List<ConnectedComponent> ccResult = new List<ConnectedComponent>();

            try
            {
                for (i = 0; i < width; i++)
                {
                    for (j = 0; j < height; j++)
                    {
                        if (!judgeWhitePoint(sourceImage.GetPixel(i, j)) && !visited[i, j])
                        {
                            visited[i, j] = true;

                            ConnectedComponent currentCC = new ConnectedComponent();

                            currentCC.PointSet.Add(new iPoint(i, j));
                            try
                            {
                                DFS(i, j, ref visited, currentCC, sourceImage);
                            }
                            catch (Exception ea)
                            {
                                throw new Exception(ea.Message);
                            }

                            FormatConnectedComponent(currentCC);

                            ccResult.Add(currentCC);
                        }
                    }
                }
            }
            catch (Exception e)
            {
                throw new Exception(e.Message);
            }
            return ccResult;
        }

        /// <summary>
        /// Merge different connected components into one.
        /// </summary>
        /// <param name="cc1"></param>
        /// <param name="cc2"></param>
        /// <returns></returns>
        public static ConnectedComponent mergeConnectedComponents(List<ConnectedComponent> cc)
        {
            // If there are only one element in the list, return the element directly.
            if (cc.Count == 1)
            {
                return cc[0];
            }

            ConnectedComponent ccRet = new ConnectedComponent();

            int minWidth = int.MaxValue, minHeight = int.MaxValue;
            int maxWidth = int.MinValue, maxHeight = int.MinValue;

            foreach (ConnectedComponent ccI in cc)
            {
                minWidth = ccI.Left_Top.Width_Position < minWidth ? ccI.Left_Top.Width_Position : minWidth;
                minHeight = ccI.Left_Top.Height_Position < minHeight ? ccI.Left_Top.Height_Position : minHeight;

                maxWidth = ccI.Right_Bottom.Width_Position > maxWidth ? ccI.Right_Bottom.Width_Position : maxWidth;
                maxHeight = ccI.Right_Bottom.Height_Position > maxHeight ? ccI.Right_Bottom.Height_Position : maxHeight;

                foreach (iPoint ponit in ccI.PointSet)
                {
                    ccRet.PointSet.Add(ponit);
                }
            }
            ccRet.Left_Top = new iPoint(minWidth, minHeight);
            ccRet.Right_Bottom = new iPoint(maxWidth, maxHeight);

            return ccRet;
        }

        /// <summary>
        /// Merge all connected components, the principles are
        /// 1. have similar color (or gray values)
        /// 2. the distance between the two connected components lies in the range [0, threshold]
        /// </summary>
        /// <param name="ccResults"></param>
        public static void mergeAllConnectedComponents(List<ConnectedComponent> ccResults, Bitmap sourceImage)
        {
            int i = 0, j = 0;
            int n = ccResults.Count;
            bool[] visited = new bool[n];
            List<ConnectedComponent> ccResultsCopy = new List<ConnectedComponent>();

            // Copy ccResults to ccResultsCopy
            ccResults.ForEach(ct => ccResultsCopy.Add(ct));

            ccResults.Clear();

            for (i = 0; i < n; i++)
            {
                if (!visited[i])
                {
                    List<ConnectedComponent> mergeParts = new List<ConnectedComponent>();

                    visited[i] = true;
                    mergeParts.Add(ccResultsCopy[i]);
                    for (j = i; j < n; j++)
                    {
                        if (!visited[j] &&
                            judgeConnectedComponentsNear(ccResultsCopy[i], ccResultsCopy[j]) &&
                            judgeConnectedComponentsSimilar(ccResultsCopy[i], ccResultsCopy[j], sourceImage))
                        {
                            visited[j] = true;
                            mergeParts.Add(ccResultsCopy[j]);
                        }
                    }
                    ConnectedComponent retConnectedComponent = mergeConnectedComponents(mergeParts);

                    if (calConnectedComponentsSize(retConnectedComponent) >= Config.ConfigParameters.CC_Size_Threshold)
                    {
                        ccResults.Add(mergeConnectedComponents(mergeParts));
                    }
                }
            }
        }

        /// <summary>
        /// Using depth first search to find all non-white connected components.
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="visited"></param>
        /// <param name="ccResult"></param>
        public static void DFS(int width, int height, ref bool[,] visited, ConnectedComponent ccResult, Bitmap sourceImage)
        {
            int i = 0;
            int j = 0;

            try
            {
                for (i = -1; i <= 1; i++)
                {
                    for (j = -1; j <= 1; j++)
                    {
                        if (!(i == 0 && j == 0) &&
                            width + i >= 0 && width + i < sourceImage.Width &&
                            height + j >= 0 && height + j < sourceImage.Height &&
                            !visited[width + i, height + j] &&
                            calDiff(sourceImage.GetPixel(width, height), sourceImage.GetPixel(width + i, height + j)) <= ConfigParameters.DiffThreshold)
                        {
                            visited[width + i, height + j] = true;

                            ccResult.PointSet.Add(new iPoint(width + i, height + j));

                            DFS(width + i, height + j, ref visited, ccResult, sourceImage);
                        }
                    }
                }
            }
            catch (Exception ecc)
            {
                throw new Exception(ecc.Message);
            }
        }

        #endregion

        #region Template image operations

        /// <summary>
        /// Write bitmap into text file.
        /// Transform the image into a ulong array.
        /// </summary>
        /// <param name="sourceImage"></param>
        public static string writeTemplateBmpIntoTextFormat(char charName, Bitmap sourceImage)
        {
            int i = 0, j = 0, c_one = 0; ;
            int width = sourceImage.Width;
            int height = sourceImage.Height;
            StringBuilder templateString = new StringBuilder();
            StringBuilder bitString = new StringBuilder();
#if DEBUG
            if (charName == '4' && sourceImage.Height == 16 && sourceImage.Width == 12)
            {
                i = 5;
            }
#endif

            for (i = 0; i < height; i++)
            {
                for (j = 0; j < width; j++)
                {
                    Color color = sourceImage.GetPixel(j, i);
                    if (color.R == 255 && color.G == 255 && color.B == 255)
                    {
                        bitString.Append("0");
                    }
                    else
                    {
                        bitString.Append("1");
                        c_one++;
                    }
                }
            }

            //ulong[] elements = BitOperation.TransferOneZeroStringToUlongArray(bitString.ToString());

            // Format: [Character] [Height] [Width] [Count One] [0-1 String]
            //templateString.Append(string.Format("{0} {1} {2} {3} ", charName, sourceImage.Height, sourceImage.Width, elements.Length));
            //foreach (ulong element in elements)
            //{
            //    templateString.Append(string.Format("{0} ", element.ToString()));
            //}

            // Format: [Character] [Height] [Width] [Count One] [0-1 String]
            templateString.Append(string.Format("{0} {1} {2} {3} {4}", charName, sourceImage.Height, sourceImage.Width, c_one, bitString.ToString()));

            return templateString.ToString();
        }

        #endregion

        #region Image searching

        /// <summary>
        /// The main branch for image recogonition.
        /// </summary>
        /// <param name="left_top"></param>
        /// <param name="right_bottom"></param>
        /// <param name="lsTemplate"></param>
        /// <param name="sourceImage"></param>
        /// <returns></returns>
        public static string ImageRecogonitionMainBranch(iPoint left_top, iPoint right_bottom, List<CharacterTemplate> lsTemplate, Bitmap sourceImage)
        {
            int i = 0, j = 0, m = 0, n = 0;
            int iw = 0, ih = 0, tmp = 0;

            int startVertical = left_top.Width_Position;
            int endVertical = right_bottom.Width_Position;
            int startHorizontal = left_top.Height_Position;
            int endHorizontal = right_bottom.Height_Position;

            int sizeVertical = endVertical - startVertical + 1;
            int sizeHorizontal = endHorizontal - startHorizontal + 1;

            string retValue = "";
            bool match = false;
            char cName = ' ';
            int maxMatch = int.MinValue;

            for (i = startVertical; i <= endVertical; )
            {
                match = false;
                maxMatch = int.MinValue;
                for (j = 0; j < lsTemplate.Count; j++)
                {
                    if (endVertical - i + 1 >= lsTemplate[j].Width && sizeHorizontal >= lsTemplate[j].Height)
                    {
                        StringBuilder sbSubRegion = new StringBuilder();
                        for (m = i; m <= endVertical - lsTemplate[j].Width + 1; m++)
                        {
                            for (n = startHorizontal; n <= endHorizontal - lsTemplate[j].Height + 1; n++)
                            {
                                for (iw = 0; iw < lsTemplate[j].Width; iw++)
                                {
                                    for (ih = 0; ih < lsTemplate[j].Height; ih++)
                                    {
                                        Color color = sourceImage.GetPixel(m + iw, n + ih);
                                        if (color.R != 255 || color.G != 255 || color.B != 255)
                                        {
                                            sbSubRegion.Append("1");
                                        }
                                        else
                                        {
                                            sbSubRegion.Append("0");
                                        }
                                    }
                                }
                                //ulong[] sourceSubImageUlongArray = BitOperation.TransferOneZeroStringToUlongArray(sbSubRegion.ToString());
                                if (BitOperation.is_BlockA_Contains_BlockB(lsTemplate[j].CountOne, lsTemplate[j].ZOString, sbSubRegion.ToString(), ref tmp))
                                {
                                    match = true;
                                    //tmp = calMathBlackPositions(sbSubRegion.ToString(), lsTemplate[j].ZOString);
                                    if (tmp > maxMatch)
                                    {
                                        maxMatch = tmp;
                                        cName = lsTemplate[j].Character;
                                    }
                                }
                            }
                        }
                    }
                }
                if (match) { i = m; retValue += cName; }
                else { i++; }
            }
            return retValue;
        }

        ///// <summary>
        ///// Calculate how many black positions are overlapped.
        ///// </summary>
        ///// <param name="sbSubRegion"></param>
        ///// <param name="ulongArray"></param>
        ///// <returns></returns>
        //private static int calMathBlackPositions(string strSubRegion, string templateString)
        //{
        //    try
        //    {
        //        int cnt = 0;
        //        string strArray = "";
        //        foreach (ulong x in ulongArray)
        //        {
        //            strArray += BitOperation.TransferUnsignedLongIntegerToString(x);
        //        }
        //        if (strSubRegion.Length != strArray.Length)
        //        {
        //            throw new Exception("Strings do matching have different length.");
        //        }
        //        for (int i = 0; i < strSubRegion.Length; i++)
        //        {
        //            if (strSubRegion[i] == '1' && strArray[i] == '1')
        //            {
        //                cnt++;
        //            }
        //        }
        //        return cnt;
        //    }
        //    catch (Exception e)
        //    {
        //        Console.WriteLine(e.Message);
        //    }
        //    return 0;
        //}
        #endregion
    }
}
    
