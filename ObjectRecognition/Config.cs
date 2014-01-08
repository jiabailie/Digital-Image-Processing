using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ObjectRecognition;

namespace ObjectRecognition.Config
{
    /// <summary>
    /// Image type.
    /// </summary>
    public enum ImageType
    {
        /// <summary>
        /// One pixel one bit.
        /// </summary>
        SingleBit = 1,

        /// <summary>
        /// One pixel three bytes.
        /// </summary>
        ThreeBit = 24
    };

    /// <summary>
    /// One object represents one position(w,h) in bitmap.
    /// </summary>
    public class iPoint
    {
        /// <summary>
        /// The horizontal position
        /// </summary>
        private int w;

        /// <summary>
        /// The vertical position
        /// </summary>
        private int h;

        /// <summary>
        /// Initialize the parameters of this object.
        /// </summary>
        /// <param name="_w"></param>
        /// <param name="_h"></param>
        private void iniParameter(int _w, int _h)
        {
            this.w = _w;
            this.h = _h;
        }

        /// <summary>
        /// Default constructor.
        /// </summary>
        public iPoint()
        {
            iniParameter(0, 0);
        }

        /// <summary>
        /// Another constructor which receives two parameters to initialize the object.
        /// </summary>
        /// <param name="_w"></param>
        /// <param name="_h"></param>
        public iPoint(int _w, int _h)
        {
            iniParameter(_w, _h);
        }

        public iPoint(iPoint _ipoint)
        {
            iniParameter(_ipoint.Width_Position, _ipoint.Height_Position);
        }

        public int Width_Position
        {
            get { return w; }
            set { this.w = value; }
        }

        public int Height_Position
        {
            get { return h; }
            set { this.h = value; }
        }
    }

    /// <summary>
    /// One object represents one connected-component which is composed by two points (left_top, right_bottom)
    /// </summary>
    public class ConnectedComponent
    {
        /// <summary>
        /// The left_top point.
        /// </summary>
        private iPoint LEFT_TOP;

        /// <summary>
        /// The right_bottom point.
        /// </summary>
        private iPoint RIGHT_BOTTOM;

        /// <summary>
        /// The point position set.
        /// </summary>
        private List<iPoint> POINT_SET;

        /// <summary>
        /// Initialize the parameters of this object.
        /// </summary>
        /// <param name="_w"></param>
        /// <param name="_h"></param>
        private void iniParameter(iPoint _lt, iPoint _rb, List<iPoint> _pointSet)
        {
            LEFT_TOP = new iPoint(_lt.Width_Position, _lt.Height_Position);
            RIGHT_BOTTOM = new iPoint(_rb.Width_Position, _rb.Height_Position);
            POINT_SET = new List<iPoint>();
            foreach (iPoint point in _pointSet)
            {
                POINT_SET.Add(new iPoint(point.Width_Position, point.Height_Position));
            }
        }

        /// <summary>
        /// Default constructor.
        /// </summary>
        public ConnectedComponent()
        {
            iniParameter(new iPoint(), new iPoint(), new List<iPoint>());
        }

        public ConnectedComponent(iPoint _lt, iPoint _rb, List<iPoint> _pointSet)
        {
            iniParameter(_lt, _rb, _pointSet);
        }

        public ConnectedComponent(ConnectedComponent _cc)
        {
            iniParameter(_cc.Left_Top, _cc.Right_Bottom, _cc.PointSet);
        }

        public iPoint Left_Top
        {
            get { return LEFT_TOP; }
            set { this.LEFT_TOP = new iPoint(value); }
        }

        public iPoint Right_Bottom
        {
            get { return RIGHT_BOTTOM; }
            set { this.RIGHT_BOTTOM = new iPoint(value); }
        }

        public List<iPoint> PointSet
        {
            get
            {
                return POINT_SET;
            }
            set
            {
                this.POINT_SET.Clear();
                foreach (iPoint point in value)
                {
                    this.POINT_SET.Add(point);
                }
            }
        }
    }

    public class CharacterTemplate
    {
        /// <summary>
        /// The character which the template represents.
        /// </summary>
        private char CHARACTER;

        /// <summary>
        /// The height of original image block.
        /// </summary>
        private int HEIGHT;

        /// <summary>
        /// The width of original image block.
        /// </summary>
        private int WIDTH;

        /// <summary>
        /// The array length of this template's element array.
        /// </summary>
        //private int ARRAYLENGTH;

        /// <summary>
        /// The template's element array.
        /// </summary>
        //private ulong[] ELEMENTS;

        /// <summary>
        /// How many 1 in the string.
        /// </summary>
        private int COUNTONE;

        /// <summary>
        /// 0-1 string.
        /// </summary>
        private string ZOSTRING;

        public char Character
        {
            get { return this.CHARACTER; }
            set { this.CHARACTER = value; }
        }

        public int Height
        {
            get { return this.HEIGHT; }
            set { this.HEIGHT = value; }
        }

        public int Width
        {
            get { return this.WIDTH; }
            set { this.WIDTH = value; }
        }

        //public int ArrayLength
        //{
        //    get { return this.ARRAYLENGTH; }
        //    set { this.ARRAYLENGTH = value; }
        //}

        //public ulong[] Elements
        //{
        //    get { return this.ELEMENTS; }
        //    set { this.ELEMENTS = new ulong[this.ArrayLength]; for (int i = 0; i < this.ArrayLength; this.ELEMENTS[i] = value[i]);}
        //}

        public int CountOne
        {
            get { return this.COUNTONE; }
            set { this.COUNTONE = value; }
        }

        public string ZOString
        {
            get { return this.ZOSTRING; }
            set { this.ZOSTRING = value; }
        }

        public CharacterTemplate() { }

        public CharacterTemplate(char _character, int _height, int _width, int _countone,string _zostring)
        {
            this.Character = _character;
            this.Height = _height;
            this.Width = _width;
            this.CountOne = _countone;
            this.ZOString = _zostring;
        } 
    }

    /// <summary>
    /// The class which contains the default parameters.
    /// </summary>
    public class ConfigParameters
    {
        /// <summary>
        /// The threshold sets for distinguishing the background.
        /// </summary>
        private const int THRESHOLD = 140;

        /// <summary>
        /// The threshold sets for finding the same connected component.
        /// </summary>
        private const int DIFF_THRESHOLD = 36;

        /// <summary>
        /// The threshold sets for finding the nearest connected component which can be merged into one.
        /// </summary>
        private const double DISTANCE_THRESHOLD = 5;

        /// <summary>
        /// The threshold sets for filtering the connected components whose size are smaller than the threshold.
        /// </summary>
        private const int CC_SIZE_THRESHOLD = 60;

        /// <summary>
        /// The threshold for judging when template is in the input image.
        /// </summary>
        private const double IMAGE_INCLUDE_THRESHOLD = 0.85;

        /// <summary>
        /// The file path prefix for saving the image as (R,G,B) format.
        /// </summary>
        private const string STR_IMAGE_TEXT_PATH = @"..\..\file\";

        /// <summary>
        /// The file path prefix for saving the connected components as bmp images.
        /// </summary>
        private const string CC_IMAGE_BMP_PATH = @"..\..\cc\";

        /// <summary>
        /// The file path prefix for saving the character templates.
        /// </summary>
        private const string STR_IMAGE_TEMPLATE_PATH = @"..\..\templates\";

        /// <summary>
        /// The file path for saving the image templates.
        /// Format: [Character] [Height] [Width] [Count One] [0-1 String]
        /// </summary>
        private const string STR_IMAGE_TEMPLATE_TEXT_PATH = @"..\..\templates\templates.txt";

        /// <summary>
        /// Write the images' diagonal points to this file.
        /// </summary>
        private const string STR_IMAGE_DIAGONAL_TEXT_PATH = @"..\..\templates\diagonal.txt";

        /// <summary>
        /// Write the images' test results to this file.
        /// </summary>
        private const string STR_TEST_RESULT_TEXT_PATH = @"..\..\templates\result.txt";

        public static int ThresHold
        {
            get { return THRESHOLD; }
        }

        public static int DiffThreshold
        {
            get { return DIFF_THRESHOLD; }
        }

        public static string Str_Image_Text_Path
        {
            get { return STR_IMAGE_TEXT_PATH; }
        }

        public static string CC_Image_Bmp_Path
        {
            get { return CC_IMAGE_BMP_PATH; }
        }

        public static double Image_Include_Threshold
        {
            get { return IMAGE_INCLUDE_THRESHOLD; }
        }

        public static double DistanceThreshold
        {
            get { return DISTANCE_THRESHOLD; }
        }

        public static int CC_Size_Threshold
        {
            get { return CC_SIZE_THRESHOLD; }
        }

        public static string Str_Image_Template_path
        {
            get { return STR_IMAGE_TEMPLATE_PATH; }
        }

        public static string Str_Image_Template_Text_path
        {
            get { return STR_IMAGE_TEMPLATE_TEXT_PATH; }
        }

        public static string Str_Image_Diagonal_Text_Path
        {
            get { return STR_IMAGE_DIAGONAL_TEXT_PATH; }
        }

        public static string Str_Test_Result_Text_Path
        {
            get { return STR_TEST_RESULT_TEXT_PATH; }
        }
    }
}
