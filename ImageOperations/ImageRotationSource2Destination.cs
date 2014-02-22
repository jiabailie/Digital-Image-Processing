using System;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;

public class ImageRotation
{
    public Bitmap DoImageRotationCertainAngle(double angle, string path)
    {
        Bitmap source = GetImage2Bitmap(path);
        return DoImageRotationCertainAngle(angle, source);
    }

    public Bitmap DoImageRotationCertainAngle(double angle, Bitmap source)
    {
        int iw = 0, ih = 0;
        int sW = 0, sH = 0;
        int nW = 0, nH = 0;
        int rposH = 0, rposW = 0;
        int i = 0, cur = 0, pos = 0;
        double CH = 0.0, CW = 0.0;

        int[] validKey = new int[9];
        int[] validValue = new int[9];

        double correct = 0.5;
        double sinA = 0.0, cosA = 0.0;

        while (angle < 0) { angle += 360; }

        angle %= 360;
        if (angle > 180) { angle = angle - 360; }

        double radian = TranslateAngleToRadian(angle);
        sinA = Math.Sin(radian);
        cosA = Math.Cos(radian);

        sW = source.Width;
        sH = source.Height;

        nW = Convert.ToInt32(correct + Math.Abs(sH * sinA) + Math.Abs(sW * cosA));
        nH = Convert.ToInt32(correct + Math.Abs(sH * cosA) + Math.Abs(sW * sinA));

        CH = 0.5 * (nH - sH * cosA - sW * sinA);
        CW = 0.5 * (nW + sH * sinA - sW * cosA);

        while (nW % 4 != 0) { ++nW; }

        Bitmap destin = new Bitmap(nW, nH);

        BitmapData sourceData = source.LockBits(new Rectangle(0, 0, sW, sH), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);
        BitmapData destinData = destin.LockBits(new Rectangle(0, 0, nW, nH), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

        unsafe
        {
            byte* sourcePtr = (byte*)sourceData.Scan0;
            byte* destinPtr = (byte*)destinData.Scan0;

            for (ih = 0; ih < nH; ++ih)
            {
                for (iw = 0; iw < nW; ++iw)
                {
                    cur = ih * destinData.Stride + iw * 3;
                    for (i = 0; i < 3; ++i)
                    {
                        destinPtr[cur + i] = 255;
                    }
                }
            }

            for (ih = 0; ih < sH; ++ih)
            {
                for (iw = 0; iw < sW; ++iw)
                {
                    cur = 0;
                    pos = 0;

                    rposH = Convert.ToInt32(iw * sinA + ih * cosA + CH);
                    rposW = Convert.ToInt32(iw * cosA - ih * sinA + CW);

                    cur = rposH * destinData.Stride + rposW * 3;
                    pos = ih * sourceData.Stride + iw * 3;

                    for (i = 0; i < 3; ++i)
                    {
                        destinPtr[cur + i] = sourcePtr[pos + i];
                    }
                }
            }
            source.UnlockBits(sourceData);
            destin.UnlockBits(destinData);
        }

        GC.Collect(2);

        return destin;
    }

    public Bitmap GetImage2Bitmap(string path)
    {
        if (!File.Exists(path))
        {
            throw new Exception("File is not exist.");
        }

        Bitmap source = new Bitmap(path);

        return source;
    }

    public double TranslateAngleToRadian(double angle)
    {
        return Math.PI * angle / 180.0;
    }
}