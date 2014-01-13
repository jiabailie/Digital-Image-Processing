using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Drawing;

namespace ChinaSSS
{
	public class Config
	{
		/// <summary>
		/// The threshold sets for distinguishing the background.
		/// </summary>
		private const int THRESHOLD = 145;

		/// <summary>
		/// The threshold sets for filtering the connected components whose size are smaller than the threshold.
		/// </summary>
		private const int CC_SIZE_THRESHOLD = 15;

		public static int ThresHold
		{
			get { return THRESHOLD; }
		}

		public static int CC_Size_Threshold
		{
			get { return CC_SIZE_THRESHOLD; }
		}
	}

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

	public class Operations
	{
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
		/// Using this method to convert .jpg source image into .bmp format.
		/// </summary>
		public static Bitmap ConvertJpg2Bmp(string sourcePath)
		{
			Bitmap source = null;
			try
			{
				source = new Bitmap(sourcePath);
			}
			catch (Exception oe)
			{
				Console.WriteLine(oe.Message);
			}
			return source;
		}

		/// <summary>
		/// Do uniformization operation for certain image.
		/// </summary>
		/// <param name="img"></param>
		public static void UniformizationBmp(Bitmap img)
		{
			int iw = 0, ih = 0;
			int avg = 0;
			for (iw = 0; iw < img.Width; iw++)
			{
				for (ih = 0; ih < img.Height; ih++)
				{
					Color tmp = img.GetPixel(iw, ih);
					avg = (tmp.R + tmp.G + tmp.B) / 3;
					if (avg >= Config.ThresHold) { avg = 255; }
					else { avg = 0; }
					tmp = Color.FromArgb(avg, avg, avg);
					img.SetPixel(iw, ih, tmp);
				}
			}
		}

		/// <summary>
		/// Write one connected component to certain bmp image.
		/// </summary>
		/// <param name="ccImage"></param>
		/// <param name="sourceImage"></param>
		public static Bitmap SaveConnectedComponentAsBmp(ConnectedComponent ccImage, Bitmap sourceImage)
		{
			int width = ccImage.Right_Bottom.Width_Position - ccImage.Left_Top.Width_Position;
			int height = ccImage.Right_Bottom.Height_Position - ccImage.Left_Top.Height_Position;

			Bitmap ccBmp = new Bitmap(width + 1, height + 1, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

			Graphics ccGraphics = Graphics.FromImage(ccBmp);

			// Set the background color as black.
			ccGraphics.Clear(Color.FromArgb(0, 0, 0));

			ccGraphics.Dispose();

			foreach (iPoint point in ccImage.PointSet)
			{
				ccBmp.SetPixel(point.Width_Position - ccImage.Left_Top.Width_Position,
							   point.Height_Position - ccImage.Left_Top.Height_Position,
							   sourceImage.GetPixel(point.Width_Position, point.Height_Position)
				);
			}

			return ccBmp;
		}

		/// <summary>
		/// Find all connected components and write them into files.
		/// </summary>
		public static List<Bitmap> FindConnectedComponents(Bitmap source)
		{
			List<Bitmap> imageParts = new List<Bitmap>();
			try
			{
				List<ConnectedComponent> ccImage = findAllConnectedComponents(source);
				foreach (ConnectedComponent cc in ccImage)
				{
					imageParts.Add(SaveConnectedComponentAsBmp(cc, source));
				}
			}
			catch (Exception oe)
			{
				Console.WriteLine(oe.Message);
			}
			return imageParts;
		}

		/// <summary>
		/// Using breadth first search to find all white connected components.
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="visited"></param>
		/// <param name="ccResult"></param>
		public static void BFS(int width, int height, ref bool[,] visited, ConnectedComponent ccResult, Bitmap sourceImage)
		{
			int i = 0;
			int len = 4;
			int tx = 0, ty = 0;

			int[] dirX = new int[] { 0, 1, 0, -1 };
			int[] dirY = new int[] { 1, 0, -1, 0 };

			visited[width, height] = true;

			try
			{
				Queue<iPoint> pointQueue = new Queue<iPoint>();
				pointQueue.Enqueue(new iPoint(width, height));
				while (pointQueue.Count > 0)
				{
					iPoint tmp = pointQueue.Dequeue();
					ccResult.PointSet.Add(new iPoint(tmp.Width_Position, tmp.Height_Position));
					for (i = 0; i < len; i++)
					{
						tx = tmp.Width_Position + dirX[i];
						ty = tmp.Height_Position + dirY[i];
						if (tx >= 0 && tx < sourceImage.Width &&
							ty >= 0 && ty < sourceImage.Height &&
							!visited[tx, ty] &&
							judgeWhitePoint(sourceImage.GetPixel(tx, ty)))
						{
							visited[tx, ty] = true;
							pointQueue.Enqueue(new iPoint(tx, ty));
						}
					}
				}
			}
			catch (Exception ecc)
			{
				throw new Exception(ecc.Message);
			}
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
		/// Find all connected-components in the image and return related information.
		/// </summary>
		/// <param name="sourceImage"></param>
		/// <returns></returns>
		public static List<ConnectedComponent> findAllConnectedComponents(Bitmap sourceImage)
		{
			int i = 0, j = 0;
			int tw = 0, th = 0;

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
						if (judgeWhitePoint(sourceImage.GetPixel(i, j)) && !visited[i, j])
						{
							visited[i, j] = true;

							ConnectedComponent currentCC = new ConnectedComponent();

							currentCC.PointSet.Add(new iPoint(i, j));
							try
							{
								BFS(i, j, ref visited, currentCC, sourceImage);
							}
							catch (Exception ea)
							{
								throw new Exception(ea.Message);
							}

							FormatConnectedComponent(currentCC);

							tw = currentCC.Right_Bottom.Width_Position - currentCC.Left_Top.Width_Position;
							th = currentCC.Right_Bottom.Height_Position - currentCC.Left_Top.Height_Position;

							if (tw < width / 4 && (tw >= Config.CC_Size_Threshold || th >= Config.CC_Size_Threshold))
							{
								ccResult.Add(currentCC);
							}
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
	}

	/// <summary>
	/// The main branch to receive a image path and generate its' parts.
	/// </summary>
	public class GenerateImageParts
	{
		public static List<Bitmap> generateImageParts(string filePath)
		{
			Bitmap source = Operations.ConvertJpg2Bmp(filePath);
			Operations.UniformizationBmp(source);
			List<Bitmap> imageParts = Operations.FindConnectedComponents(source);
			return imageParts;
		}
	}
}
