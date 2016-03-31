// lambda obtained from calibration getting_perspective_transform.cpp 
//frame reference of both imput mage and output is upper left corner.
//frame refrence of output must be changed to cycle centre through translation of axes.currently not implemented.

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <string>

#define M11 1.2
#define M12 2.1
#define M13 3.3
#define M21 4.2
#define M22 5.5
#define M23 6.7
#define M31 7.8
#define M32 8.8
#define M33 9.9

 
using namespace std;
using namespace cv;

Mat src;
Mat output;
Mat lambda = (Mat_<double>(3,3) << M11 , M12 , M13 , M21 , M22 , M23 , M31 , M32 , M33 );
int main()
{
	VideoCapture v(2);
        namedWindow("Transfrom",WINDOW_AUTOSIZE);

        while(1)
        {
        	v>>src;
                warpPerspective(src,output,lambda,output.size());
                imshow("Transform",output);
                
                char a=waitKey(1);
   	        if (a == 27)
		break;

        }
	return(0);
}
