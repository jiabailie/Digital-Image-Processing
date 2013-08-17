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
    public class FileOperation
    {
        /// <summary>
        /// Save source bitmap image as text which contains all positions' pixel value
        /// random position value = {R, G, B}
        /// </summary>
        /// <param name="sourceImage"></param>
        /// <param name="path"></param>
        public static void write_Bmp_To_RGB_Number(Bitmap sourceImage, string path)
        {
            try
            {
                int i = 0, j = 0;
                StreamWriter sw = new StreamWriter(path);

                for (j = 0; j < sourceImage.Height; j++)
                {
                    for (i = 0; i < sourceImage.Width; i++)
                    {

                        sw.Write("({0},{1},{2}) ",
                            sourceImage.GetPixel(i, j).R.ToString().PadLeft(3, '0').Replace("255", "   "),
                            sourceImage.GetPixel(i, j).G.ToString().PadLeft(3, '0').Replace("255", "   "),
                            sourceImage.GetPixel(i, j).B.ToString().PadLeft(3, '0').Replace("255", "   "));
                    }
                    sw.WriteLine();
                }
                sw.Flush();
                sw.Close();
            }
            catch (Exception ex)
            {
                throw new Exception(ex.Message);
            }
        }

        /// <summary>
        /// Write all connected components to bmp images, one image for one connected component.
        /// </summary>
        /// <param name="ccImages"></param>
        /// <param name="sourceImage"></param>
        public static void write_Connected_Components_To_Bmp(int tIndex, List<ConnectedComponent> ccImages, Bitmap sourceImage)
        {
            for (int i = 0; i < ccImages.Count; i++)
            {
                write_Single_Connected_Component_To_Bmp(tIndex, i, ccImages[i], sourceImage);
            }
        }

        /// <summary>
        /// Write one connected component to certain bmp image.
        /// </summary>
        /// <param name="ccImage"></param>
        /// <param name="sourceImage"></param>
        public static void write_Single_Connected_Component_To_Bmp(int tIndex, int index, ConnectedComponent ccImage, Bitmap sourceImage)
        {
            int width = ccImage.Right_Bottom.Width_Position - ccImage.Left_Top.Width_Position;
            int height = ccImage.Right_Bottom.Height_Position - ccImage.Left_Top.Height_Position;

            Bitmap ccBmp = new Bitmap(width + 1, height + 1, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

            Graphics ccGraphics = Graphics.FromImage(ccBmp);

            // Set the background color as white.
            ccGraphics.Clear(Color.FromArgb(255, 255, 255));

            ccGraphics.Dispose();

            foreach (iPoint point in ccImage.PointSet)
            {
                ccBmp.SetPixel(point.Width_Position - ccImage.Left_Top.Width_Position,
                               point.Height_Position - ccImage.Left_Top.Height_Position,
                               sourceImage.GetPixel(point.Width_Position, point.Height_Position)
                );
            }

            ccBmp.Save(ConfigParameters.CC_Image_Bmp_Path + tIndex.ToString() + "_" + index.ToString() + ".bmp");
        }

        public static void write_String_Templates_Into_Text(List<string> stringTemplates)
        {
            try
            {
                StreamWriter sw = new StreamWriter(new FileStream(ConfigParameters.Str_Image_Template_Text_path, FileMode.OpenOrCreate));
                foreach (string str in stringTemplates)
                {
                    sw.WriteLine(str.Trim());
                }
                sw.Flush();
                sw.Close();

            }
            catch (Exception e)
            {
                Console.WriteLine("The text file for image templates has encountered error.\n{0}\n", e.Message);
            }
        }

        /// <summary>
        /// Read template images (as text format) into struct.
        /// </summary>
        /// <returns></returns>
        public static List<CharacterTemplate> read_Template_From_Image_To_Struct()
        {
            try
            {
                StreamReader sr = new StreamReader(new FileStream(ConfigParameters.Str_Image_Template_Text_path, FileMode.Open));
                List<CharacterTemplate> returnValue = new List<CharacterTemplate>();
                string oneLine = sr.ReadLine();
                // Format: [Character] [Height] [Width] [Count One] [0-1 String]
                while (!string.IsNullOrEmpty(oneLine))
                {
                    string[] components = oneLine.Trim().Split(' ');
                    char Character = components[0][0];
                    int Height = Convert.ToInt32(components[1]);
                    int Width = Convert.ToInt32(components[2]);
                    int CountOne = Convert.ToInt32(components[3]);

                    CharacterTemplate oneTemplate = new CharacterTemplate(Character, Height, Width, CountOne, components[4].Trim());

                    returnValue.Add(oneTemplate);

                    oneLine = sr.ReadLine();
                }

                return returnValue;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            return null;
        }

        /// <summary>
        /// Write the image's diagonal points into a text file.
        /// </summary>
        /// <param name="left_top"></param>
        /// <param name="right_bottom"></param>
        public static void write_Image_Diagonal_Points_To_Text(int index, iPoint left_top, iPoint right_bottom)
        {
            try
            {
                StreamWriter sw = new StreamWriter(new FileStream(ConfigParameters.Str_Image_Diagonal_Text_Path, FileMode.Append));
                sw.WriteLine(string.Format("Image {0}\t[Left-top: \twidth={1}, \theight={2}; \tright_bottom: \twidth={3}, \theight={4}]",
                    index.ToString(),
                    left_top.Width_Position,
                    left_top.Height_Position,
                    right_bottom.Width_Position,
                    right_bottom.Height_Position));

                sw.Flush();
                sw.Close();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        /// <summary>
        /// Write the image's diagonal points into a text file.
        /// </summary>
        /// <param name="left_top"></param>
        /// <param name="right_bottom"></param>
        public static void write_Test_Result_To_Text(string retValue)
        {
            try
            {
                StreamWriter sw = new StreamWriter(new FileStream(ConfigParameters.Str_Test_Result_Text_Path, FileMode.Append));
                sw.WriteLine(retValue);

                sw.Flush();
                sw.Close();

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}