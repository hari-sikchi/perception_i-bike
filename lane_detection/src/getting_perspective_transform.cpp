//Input checker board image and original image with the camera setting onboard cycle.
//remember to give any inputs in clockwise direction

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <string>
//#include "transform_matrix.h"

using namespace std;
using namespace cv;


int bot_x;
int bot_y;

int d;
int w;
int h;

int x[4];
int y[4];
int temp=0,flag=0;
int X[4];
int Y[4];
Mat src;
Mat output;
Mat lambda( 3 , 3 , CV_32FC1 );
    // Input Quadilateral or Image plane coordinates
    Point2f inputQuad[4]; 
    // Output Quadilateral or World plane coordinates
    Point2f outputQuad[4];
         
    // Lambda Matrix
  //  Mat lambda( 3, 3, CV_32FC1 ); 

void loadVariable() {
    int status = 1;
    FILE *readFile;
    readFile = fopen("ipt_offsets1.txt", "r");
    status = status && fscanf(readFile, "bot_x = %d\n", &bot_x);
    status = status && fscanf(readFile, "bot_y = %d\n", &bot_y);
    status = status && fscanf(readFile, "d = %d\n", &d);
    status = status && fscanf(readFile, "w = %d\n", &w);
    status = status && fscanf(readFile, "h = %d\n", &h);
    printf("%d %d %d %d %d\n", bot_x, bot_y, d, w, h);
    return;
    if (status == 0) {
        printf("load in reading bot variable file\n");
    }
} 

void CallBackFunc(int event, int c, int r, int flags, void* userdata)
{
     if(temp<4)
     {
     	if ( event == (EVENT_LBUTTONDOWN) )
     	{
        	cout << "Left mouse button is clicked while pressing CTRL key - position (" << c << ", " << r << ")" << endl;
     
        	x[temp]=c; y[temp]=r;
        	temp++;
     	}
     }
     else 
        {
               inputQuad[0] = Point2f( x[0],y[0]);
               inputQuad[1] = Point2f( x[1],y[1]);
               inputQuad[2] = Point2f( x[2],y[2]);
               inputQuad[3] = Point2f( x[3],y[3]);  
    
              /* outputQuad[0] = Point2f( X[0],Y[0]);
               outputQuad[1] = Point2f( X[1],Y[1]);
               outputQuad[2] = Point2f( X[2],Y[2]);
               outputQuad[3] = Point2f( X[3],Y[3]);   */
 
               outputQuad[0] = Point2f( bot_x - w/2 , bot_y-d-h/2);
               outputQuad[1] = Point2f( bot_x + w/2 , bot_y-d-h/2);
               outputQuad[2] = Point2f( bot_x + w/2 , bot_y-d+h/2);
               outputQuad[3] = Point2f( bot_x - w/2 , bot_y-d+h/2);
                
               // Get the Perspective Transform Matrix i.e. lambda 
   
    
    	       lambda = getPerspectiveTransform(inputQuad,outputQuad);
               cout << "M = "<< endl << " "  << lambda << endl << endl;

               int status = 1;
               FILE *writeFile;
               writeFile = fopen("ipt_data.txt", "w");
               for( int m = 0 ; m < 4 ; m++)
               {   
                  status = status && fprintf(writeFile, "%i %i\n",x[m],y[m]);           
               }
            if (status == 0) 
            {
            printf("Couldn't write to file ");
            }

           fclose(writeFile);
                
   
               warpPerspective( src , output , lambda ,  Size(1000,1000));
               namedWindow( "transform" , WINDOW_AUTOSIZE );
               imshow( "transform" , output);
               
    
    
        }
     
}

void help()
{
  printf("\t Hough Transform to detect lines \n ");
  printf("\t---------------------------------\n ");
  printf(" Usage: ./HoughLines_Demo <image_name> \n");
}

int main( int argc, char**argv )
{
      /// Read the image
   src = imread( argv[1], 1 );

   if( src.empty() )
     { 
       help();
       return -1;
     }
     //Create a window
    namedWindow("My Window", WINDOW_AUTOSIZE);
    imshow("My Window",src);


    static bool file_opened = false;

    if (!file_opened) {
        loadVariable();
        file_opened = true;
    }

     //set the callback function for any mouse event
   
    setMouseCallback("My Window", CallBackFunc, NULL);
    
    

/*    for(int i=0;i<4;i++)
    {
     cout<<"enter the value of X["<< i <<"]";
     cin>>X[i];
     cout<<"enter the value of Y["<< i <<"]";
     cin>>Y[i];

    }   */
  
    waitKey(0);
    return(0);
   
 }

