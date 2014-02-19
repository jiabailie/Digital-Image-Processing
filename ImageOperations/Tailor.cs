using System;
using System.Drawing;
using System.Drawing.Imaging;

public class Tailor
{
    /// <summary>
    /// Using threshold to generate templates.
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public static Bitmap Using_Threshold_To_Generate_Templates(Bitmap source)
    {
        byte v = 255;
        int i = 0, j = 0, k = 0;
        int cur = 0, ncur = 0, avg = 0;
        Bitmap destin = null;
        int nw = 0, nh = 0;
        int w = source.Width, h = source.Height;
        int leftX = int.MaxValue, leftY = int.MaxValue;
        int rightX = int.MinValue, rightY = int.MinValue;

        BitmapData sourceData = source.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

        unsafe
        {
            byte* sourcePtr = (byte*)sourceData.Scan0;
            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    v = 255;
                    cur = i * sourceData.Stride + j * 3;
                    avg = Convert.ToInt32((
                        Convert.ToInt32(sourcePtr[cur + 0]) +
                        Convert.ToInt32(sourcePtr[cur + 1]) +
                        Convert.ToInt32(sourcePtr[cur + 2])) / 3);

                    if (avg == 255) { continue; }

                    if (avg != Config.ThresHold)
                    {
                        v = 0;
                        if (i < leftX) { leftX = i; }
                        if (i > rightX) { rightX = i; }
                        if (j < leftY) { leftY = j; }
                        if (j > rightY) { rightY = j; }
                    }
                    for (k = 0; k < 3; ++k)
                    {
                        sourcePtr[cur + k] = v;
                    }
                }
            }

            nw = rightY - leftY + 1;
            nh = rightX - leftX + 1;

            destin = new Bitmap(nw, nh);
            BitmapData destinData = destin.LockBits(new Rectangle(0, 0, nw, nh), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
            byte* destinPtr = (byte*)destinData.Scan0;

            for (i = 0; i < nh; ++i)
            {
                for (j = 0; j < nw; ++j)
                {
                    cur = (i + leftX) * sourceData.Stride + (j + leftY) * 3;
                    ncur = i * destinData.Stride + j * 3;
                    for (k = 0; k < 3; ++k)
                    {
                        destinPtr[ncur + k] = sourcePtr[cur + k];
                    }
                }
            }

            source.UnlockBits(sourceData);
            destin.UnlockBits(destinData);

            source.Dispose();
        }

        return destin;
    }

    public static Bitmap Using_Threshold_To_Generate_Templates(string path)
    {
        Bitmap source = Operations.Convert_Jpg2Bmp(path);
        return Using_Threshold_To_Generate_Templates(source);
    }

    public static void FindBlackRegionSize(Bitmap source, ref int leftH, ref int leftW, ref int rightH, ref int rightW)
    {
        int i = 0, j = 0;
        int w = source.Width;
        int h = source.Height;

        BitmapData sourceData = source.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

        leftH = leftW = int.MaxValue;
        rightH = rightW = int.MinValue;
        unsafe
        {
            byte* sourcePtr = (byte*)sourceData.Scan0;

            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    if (sourcePtr[i * sourceData.Stride + j * 3] == 0)
                    {
                        if (i < leftH) { leftH = i; }
                        if (i > rightH) { rightH = i; }
                        if (j < leftW) { leftW = j; }
                        if (j > rightW) { rightW = j; }
                    }
                }
            }

            source.UnlockBits(sourceData);
        }
        GC.Collect(0);
    }

    public static Bitmap ImageTailor(Bitmap source, int leftH, int leftW, int rightH, int rightW)
    {
        int i = 0, j = 0, k = 0;
        int h = rightH - leftH + 1;
        int w = rightW - leftW + 1;

        Bitmap tailor = new Bitmap(w, h);

        BitmapData sourceData = source.LockBits(new Rectangle(0, 0, source.Width, source.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
        BitmapData tailorData = tailor.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

        unsafe
        {
            byte* sourcePtr = (byte*)sourceData.Scan0;
            byte* tailorPtr = (byte*)tailorData.Scan0;

            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    int sCur = (i + leftH) * sourceData.Stride + (j + leftW) * 3;
                    int nCur = i * tailorData.Stride + j * 3;

                    for (k = 0; k < 3; ++k)
                    {
                        tailorPtr[nCur + k] = sourcePtr[sCur + k];
                    }
                }
            }

            source.UnlockBits(sourceData);
            tailor.UnlockBits(tailorData);

            source.Dispose();
        }

        GC.Collect(2);

        return tailor;
    }
}