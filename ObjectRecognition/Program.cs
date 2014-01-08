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
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                int branch = 0; // Operation branch.

                ConcreteOperations.print_Base_Operation_Branch();

                branch = Convert.ToInt32(Console.ReadLine().Trim());

                switch (branch)
                {
                    case 1:
                        ConcreteOperations.WriteTemplateIntoTextFiles();
                        break;
                    case 2:                        
                        ConcreteOperations.ImageRecognition();
                        break;
                    default:
                        break;
                }           
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
