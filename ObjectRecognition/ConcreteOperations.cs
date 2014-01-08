#define DEBUG
#undef  DEBUG

#define DEBUG_COMPONENT
#undef  DEBUG_COMPONENT

#define DEBUG_DIAGONAL
#undef  DEBUG_DIAGONAL

#define DEBUG_TEST_RESULT
//#undef  DEBUG_TEST_RESULT

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Collections;
using System.IO;
using ObjectRecognition.Config;

namespace ObjectRecognition
{
    public class ConcreteOperations
    {
        /// <summary>
        /// Print the operation branch on the screen.
        /// </summary>
        public static void print_Base_Operation_Branch()
        {
            Console.WriteLine("Please select the operation branch you want:\n");
            Console.WriteLine("1. Write template into the text files.\n");
            Console.WriteLine("2. Distinguish the input image.\n");
        }

        /// <summary>
        /// Do image recognition operation.
        /// </summary>
        public static void ImageRecognition()
        {
            int i = 0;
            List<CharacterTemplate> lsTemplate = FileOperation.read_Template_From_Image_To_Struct();

            for (i = 0; i <= 1024; i++)
            {
                iPoint left_top = new iPoint();
                iPoint right_bottom = new iPoint();

                Bitmap bitmap24 = ImageOperation.saveTo24BMP(string.Format(@"..\..\DATA\i({0}).png", i));
#if DEBUG
                bitmap24.Save(string.Format(@"..\..\DATA\{0}.bmp", i));
#endif

                ImageOperation.changeBackground(bitmap24);

#if DEBUG
                bitmap24.Save(string.Format(@"..\..\DATA\{0}_changebackground.bmp", i));
#endif

                ImageOperation.sameNearbyPixel(bitmap24);

#if DEBUG
                bitmap24.Save(string.Format(@"..\..\DATA\{0}_sameNearbyPixel.bmp", i));
#endif

                ImageOperation.sameNeighbourNearbyPixel(bitmap24);
#if DEBUG
                bitmap24.Save(string.Format(@"..\..\DATA\{0}_sameVerticalNearbyPixel.bmp", i));
#endif

#if DEBUG_COMPONENT
                List<ConnectedComponent> ccImage = ImageOperation.findAllConnectedComponents(bitmap24);
                ImageOperation.mergeAllConnectedComponents(ccImage, bitmap24);
#endif
                ImageOperation.findDiagonalPoints(bitmap24, left_top, right_bottom);

#if DEBUG_DIAGONAL
                FileOperation.write_Image_Diagonal_Points_To_Text(i, left_top, right_bottom);
#endif

                string retValue = ImageOperation.ImageRecogonitionMainBranch(left_top, right_bottom, lsTemplate, bitmap24);

#if DEBUG_TEST_RESULT
                FileOperation.write_Test_Result_To_Text(retValue);
#endif

#if DEBUG
                FileOperation.write_Connected_Components_To_Bmp(i, ccImage, bitmap24);
                FileOperation.write_Bmp_To_RGB_Number(bitmap24, ConfigParameters.Str_Image_Text_Path + string.Format("rgb_text{0}.txt", i));
#endif
                Console.WriteLine(string.Format("Image {0} has been processed.", i));
            }
        }

        /// <summary>
        /// Write the templates into text files.
        /// </summary>
        public static void WriteTemplateIntoTextFiles()
        {
            try
            {
                int iCharName = 0, iCharNum = 0;
                string templateFileName = "";
                List<string> stringTemplates = new List<string>();
                char[] charName = new char[] { '0','1','2','3','4','5',
                '6','7','8','9','A','B',
                'C','D','E','F','G','H',
                'I','J','K','L','M','N',
                'O','P','Q','R','S','T',
                'U','V','W','X','Y','Z'};

                for (iCharName = 0; iCharName < charName.Length; iCharName++)
                {
                    for (iCharNum = 0; iCharNum < (1 << 10); iCharNum++)
                    {
                        try
                        {
                            templateFileName = Config.ConfigParameters.Str_Image_Template_path
                                + charName[iCharName].ToString()
                                + "(" + iCharNum.ToString() + ")"
                                + ".bmp";
                            Bitmap templateImage = new Bitmap(Image.FromFile(templateFileName));
                            stringTemplates.Add(ImageOperation.writeTemplateBmpIntoTextFormat(charName[iCharName], templateImage));
                        }
                        catch (Exception eNotExist)
                        {
                            Console.WriteLine(eNotExist.ToString());
                            break;
                        }
                    }
                }

                FileOperation.write_String_Templates_Into_Text(stringTemplates);
                Console.WriteLine("All templates have been transferred into text.");
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
