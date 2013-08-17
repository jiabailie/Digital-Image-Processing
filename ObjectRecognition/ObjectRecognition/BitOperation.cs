using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ObjectRecognition
{
    public class BitOperation
    {
        /// <summary>
        /// Transform one binary string to an unsigned long type integer.
        /// </summary>
        /// <param name="bitString"></param>
        /// <returns></returns>
        public static ulong TransferStringToUnsignedLongInteger(string bitString)
        {
            ulong ret = 0;

            if (!string.IsNullOrEmpty(bitString) && bitString.Length > 0)
            {
                int i = 0;
                int len = bitString.Length;

                for (i = 0; i < len; i++)
                {
                    ret <<= 1;
                    if (bitString[i] == '1')
                    {
                        ret++;
                    }
                }
            }
            return ret;
        }

        /// <summary>
        /// Transform unsigned long type number to its binary representation
        /// </summary>
        /// <param name="number"></param>
        /// <returns></returns>
        public static string TransferUnsignedLongIntegerToString(ulong number)
        {
            ulong index = ((ulong)1 << 63);
            StringBuilder sbRet = new StringBuilder();

            while (index > 0)
            {
                sbRet.Append(((number & index) == 0 ? "0" : "1"));
                index >>= 1;
            }
            return sbRet.ToString();
        }

        /// <summary>
        /// The subBlock is a matrix size of [height, width].
        /// This function transforms the matrix to a ulong type array.
        /// </summary>
        /// <param name="subBlock"></param>
        /// <returns></returns>
        public static ulong[] TransferOneZeroStringToUlongArray(string subBlock)
        {
            ulong[] ret = null;
            int i = 0, arrayLength = 0;
            int length = subBlock.Length + 1;

            subBlock = "1" + subBlock;

            arrayLength = length / 64;

            if (length % 64 != 0)
            {
                arrayLength++;
                length = (arrayLength << 6);

                subBlock = subBlock.PadRight(length, '0');
            }
            ret = new ulong[arrayLength];
            for (i = 0; i < arrayLength; i++)
            {
                ret[i] = TransferStringToUnsignedLongInteger(subBlock.Substring(i << 6, 64));
            }

            return ret;
        }

        /// <summary>
        /// Judge whether BlockA contains BlockB, A is the template.
        /// </summary>
        /// <param name="AOne"></param>
        /// <param name="BlockA"></param>
        /// <param name="BlockB"></param>
        /// <returns></returns>
        public static bool is_BlockA_Contains_BlockB(int AOne, string BlockA, string BlockB, ref int cntOne)
        {
            bool ret = true;

            cntOne = 0;
            for (int i = 0; i < BlockA.Length; i++)
            {
                if (BlockA[i] == '1' && BlockB[i] == '1') { cntOne++; }
            }
            if ((cntOne * 1.0 / AOne) < Config.ConfigParameters.Image_Include_Threshold)
            {
                ret = false;
            }
            return ret;
        }
    }
}
