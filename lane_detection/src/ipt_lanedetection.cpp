/************************************************* NOTES **************************************************************/
// 1.If lanes are having sharp and straight edge then , standard hough transform should be applied over canny of binary image
// 2.If lanes are distorted and edges are not fine , then  probablistic HT should be applied over binary output bypassing canny


/**************************************************************************************************************************/
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
//#include "transform_matrix.h"

#define SMAX 70
#define SMIN 100
#define PMAX 160
#define PMIN 10
using namespace cv;
using namespace std;

int bot_x;
int bot_y;

int d;
int w;
int h;

int x[4];
int y[4];
int temp=0,flag=0;


//  Global variables
int l=50,u=200;
                                      // Input Quadilateral or Image plane coordinates
Point2f inputQuad[4]; 
                                      // Output Quadilateral or World plane coordinates
Point2f outputQuad[4];
         
/*  General variables */
Mat src, edges;
Mat src_gray;
Mat output;
Mat dilated;
Mat eroded;
Mat no_blobs;
Mat warp;        
Mat lambda( 3 , 3 , CV_32FC1 );


       // roi will change according to inputvideo to cut noise
Mat standard_hough, probabilistic_hough;
int min_threshold = 40;                //these are default values after thresholding
int max_trackbar = 150;
int bin_threshold_value = 158;



const char* standard_name = "Standard Hough Lines Demo";
const char* probabilistic_name = "Probabilistic Hough Lines Demo";

int s_trackbar = 78;                     //default values after thresholding
int p_trackbar = max_trackbar;

/// Function Headers
void help();
void Standard_Hough( int, void* , Mat input);
void Standard_Hough_Avg( int, void* , Mat input);
void Probabilistic_Hough( int, void*, Mat input );

/**************************************************************************************/
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

    status =1;
    readFile = fopen("ipt_data.txt", "r");
    for ( int m=0 ; m<4 ; m++) 
    status = status && fscanf(readFile , "%i %i\n" , &x[m] , &y[m] );
   
    for ( int m=0 ; m<4 ; m++) 
    printf("%i %i\n" , x[m] , y[m] );

     

    return;
    if (status == 0) {
        printf("Problem reading bot variable file\n");
    }
} 

/*******************************************************************************/
void inverse_perspective_transform()
{
   inputQuad[0] = Point2f( x[0],y[0]);
   inputQuad[1] = Point2f( x[1],y[1]);
   inputQuad[2] = Point2f( x[2],y[2]);
   inputQuad[3] = Point2f( x[3],y[3]);  

   outputQuad[0] = Point2f( bot_x - w/2 , bot_y-d-h/2);
   outputQuad[1] = Point2f( bot_x + w/2 , bot_y-d-h/2);
   outputQuad[2] = Point2f( bot_x + w/2 , bot_y-d+h/2);
   outputQuad[3] = Point2f( bot_x - w/2 , bot_y-d+h/2);
    
   // Get the Perspective Transform Matrix i.e. lambda 
 lambda = getPerspectiveTransform(inputQuad,outputQuad);
               
}

/*******************************************************************************************************/

Mat remove_blobs(Mat binary_image)
{

double threshold = 400;

vector< vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<int> small_blobs;
double contour_area;
Mat temp_image;

// find all contours in the binary image
binary_image.copyTo(temp_image);
findContours(temp_image, contours, hierarchy, CV_RETR_CCOMP,
                                                  CV_CHAIN_APPROX_SIMPLE);

// Find indices of contours whose area is less than `threshold` 
if ( !contours.empty()) 
{
    for (size_t i=0; i<contours.size(); ++i) 
    {
        contour_area = contourArea(contours[i]) ;
        if ( contour_area < threshold)
            small_blobs.push_back(i);
    }
}

// fill-in all small contours with zeros
for (size_t i=0; i < small_blobs.size(); ++i) 
{
    drawContours(binary_image, contours, small_blobs[i], cv::Scalar(0), 
                                                 CV_FILLED, 8);
}

return binary_image;
}


/******************************************************************************************************/

int main(int argc,char**argv)
{
   static bool file_opened = false;

    if (!file_opened) 
    {
        loadVariable();
        file_opened = true;
    }
   
   /// Read the image
   VideoCapture v(argv[1]);

   /// Create Trackbars for Thresholds
   char thresh_label[50];
   sprintf( thresh_label, "Thres: %d + input", min_threshold );
   
   namedWindow( "original" , WINDOW_NORMAL );
   namedWindow( "input" , WINDOW_NORMAL );
   namedWindow( "final_output" , WINDOW_NORMAL );
   namedWindow( standard_name, WINDOW_NORMAL );
   namedWindow( "transform" , WINDOW_NORMAL );
   namedWindow( probabilistic_name, WINDOW_NORMAL );

   createTrackbar( thresh_label, standard_name, &s_trackbar, max_trackbar);
     

   inverse_perspective_transform();

   cout << "M = "<< endl << " "  << lambda << endl << endl;

   createTrackbar( "binary threshold", "transform", &bin_threshold_value, 255 );
   createTrackbar( thresh_label, probabilistic_name, &p_trackbar, max_trackbar);
  // createTrackbar("upper",standard_name,&u,300);    //delete after thresholding and update u,l in canny func accordingly
  // createTrackbar("lower",standard_name,&l,300);    

   while(1)
{
   v >> src;
   
   //imshow( "original" , src );
   
      
   cvtColor( src, src_gray, COLOR_RGB2GRAY );    //changing src to output

   warpPerspective( src_gray , warp , lambda ,  Size(1000,1000));                      /// Apply Canny edge detector
  
   threshold( warp, edges, bin_threshold_value, 255 , 0 );

   //dilate(edges, dilated, Mat(), Point(-1, -1), 3, 1, 1);   //remove_blobs is more effective without erode/dilate
  
   //erode(dilated , eroded , Mat(),Point(-1 ,-1), 3,1 ,1);

   no_blobs = remove_blobs(edges);   //remove_blobs function modifies image input.So input is modified

   Canny( edges, output, l, u, 3 ,true);  //l=50 ; u = 200 ;
   
   imshow( "transform" , no_blobs);
   imshow("warped" , warp);
   imshow("input" , src);
   imshow("output",output);
 
   Standard_Hough_Avg( 0, 0,output );  //output is passed to this function

   
   Probabilistic_Hough(0, 0 ,output);
   char a=waitKey(1);
   	if (a == 27)
		break;    
}
	return(0);
}

/****************************************************************************************************/
void Standard_Hough(int, void* , Mat input )
{
  vector<Vec2f> s_lines;
  cvtColor( input ,standard_hough, COLOR_GRAY2BGR );

  /// 1. Use Standard Hough Transform
  HoughLines( input, s_lines, 1, CV_PI/180, min_threshold + s_trackbar, 0, 0 );

  /// Show the result
  for( size_t i = 0; i < s_lines.size(); i++ )
     {
      float r = s_lines[i][0], t = s_lines[i][1];
      double cos_t = cos(t), sin_t = sin(t);
      double x0 = r*cos_t, y0 = r*sin_t;
      double alpha = 1000;

       Point pt1( cvRound(x0 + alpha*(-sin_t)), cvRound(y0 + alpha*cos_t) );
       Point pt2( cvRound(x0 - alpha*(-sin_t)), cvRound(y0 - alpha*cos_t) );
       line( standard_hough, pt1, pt2, Scalar(255,0,0), 3, CV_AA);
     }

   imshow( standard_name, standard_hough );
}
/************************************************************************************/
void Standard_Hough_Avg( int , void* , Mat input)
{
  vector<Vec2f> s_lines;
  //Canny( src_gray, edges, l, u, 3 );
  cvtColor( input, standard_hough, COLOR_GRAY2BGR );
  float r_sum_p=0.0,t_sum_p=0.0;
  float r_avg_p=0.0,t_avg_p=0.0;
  float r_sum_n=0.0,t_sum_n=0.0;
  float r_avg_n=0.0,t_avg_n=0.0;
 
  float x_i,y_i;

  int count_p=0;
  int count_n=0;

  /// 1. Use Standard Hough Transform
  HoughLines( input, s_lines, 1, CV_PI/180, min_threshold + s_trackbar, 0, 0 );  //edges instead of output

  /// Show the result
  for( size_t i = 0; i < s_lines.size(); i++ )
     {
      float r = s_lines[i][0], t = s_lines[i][1];
      
      //cout << "theta is "<< t*180/3.14 << endl;
      double cos_t = cos(t)  ;  double sin_t = sin(t);
      double x0 = r*cos_t, y0 = r*sin_t;
      double alpha = 1000;

       Point pt1( cvRound(x0 + alpha*(-sin_t)), cvRound(y0 + alpha*cos_t) );
       Point pt2( cvRound(x0 - alpha*(-sin_t)), cvRound(y0 - alpha*cos_t) );
       
       //cout << "slope is " << slope <<endl; 
       //if ((t*180/3.14 > SMIN ) || ( t*180/3.14 < SMAX) )
       
       		if (s_lines[i][1]>0 && s_lines[i][1]<1.57)
       		{
       		r_sum_p+=s_lines[i][0] ; t_sum_p+=s_lines[i][1]; //cout << "theta is " << s_lines[i][1] <<endl;
       		count_p++;
       		}
       		if (s_lines[i][1]<0 || s_lines[i][1]>1.57) 
       		{
       		r_sum_n+=s_lines[i][0] ; t_sum_n+=s_lines[i][1]; //cout << "theta is " << s_lines[i][1] <<endl;
       		count_n++;
       		}
       		line(  standard_hough, pt1, pt2, Scalar(255,0,0), 2, CV_AA);
       
       }
      
      if (count_p>0)         //theta is in polar form of line 
      {
      t_avg_p=(float)t_sum_p/count_p;r_avg_p=(float)r_sum_p/count_p;
      //cout << "avg is " << t_avg <<endl; 
      double cos_t_avg = cos(t_avg_p)  ;  double sin_t_avg = sin(t_avg_p);
      double x0a = r_avg_p*cos_t_avg, y0a = r_avg_p*sin_t_avg;
      double alpha = 1000;

       Point pt3( cvRound(x0a + alpha*(-sin_t_avg)), cvRound(y0a + alpha*cos_t_avg) );
       Point pt4( cvRound(x0a - alpha*(-sin_t_avg)), cvRound(y0a - alpha*cos_t_avg) );
       
       line( standard_hough, pt3, pt4, Scalar(0,255,0), 2, CV_AA);
        
       //line( ROI, pt3, pt4, Scalar(0,255,0), 2, CV_AA);
       }


       if (count_n>0)
      {
      t_avg_n=(float)t_sum_n/count_n;r_avg_n=(float)r_sum_n/count_n;
      //cout << "avg is " << t_avg <<endl; 
      double cos_t_avg = cos(t_avg_n)  ;  double sin_t_avg = sin(t_avg_n);
      double x0a = r_avg_n*cos_t_avg, y0a = r_avg_n*sin_t_avg;
      double alpha = 1000;

       Point pt3( cvRound(x0a + alpha*(-sin_t_avg)), cvRound(y0a + alpha*cos_t_avg) );
       Point pt4( cvRound(x0a - alpha*(-sin_t_avg)), cvRound(y0a - alpha*cos_t_avg) );
       
       line( standard_hough, pt3, pt4, Scalar(0,255,0), 2, CV_AA);
       
       //line( ROI, pt3, pt4, Scalar(0,255,0), 2, CV_AA);
      }
      
      if (count_p >0 && count_n ==0)
      {
          Point pt0( cvRound(500+500*(sin(t_avg_p))) , cvRound(1000-500*(cos(t_avg_p))) );
          line( standard_hough , Point(500,1000) , pt0 , Scalar(0,0,255), 2 , CV_AA );
      }
      
      else if (count_p==0 && count_n > 0)
      {
          Point pt0( cvRound(500-500*(sin(t_avg_n))) , cvRound(1000+500*(cos(t_avg_n))) );
          line( standard_hough , Point(500,1000) , pt0 , Scalar(0,0,255), 2 ,CV_AA);
    
      }
  
     /* else if( count_p >0 && count_n>0)
      {
      }  */
      else
      {
          line( standard_hough , Point(500,1000) , Point(500,500) , Scalar(0,0,255), 2 , CV_AA );
      }
   
   
     

     // x_i=((r_avg_n)*(sin(t_avg_p))-(r_avg_p)*(sin(t_avg_n)))/( - cos(t_avg_p)*sin(t_avg_n) + cos(t_avg_n)*sin(t_avg_p));
     // y_i=(r_avg_p*cos(t_avg_n)-r_avg_n*cos(r_avg_p))/( -cos(t_avg_p)*sin(t_avg_n) + cos(t_avg_n)*sin(t_avg_p)); 
      
      //cout << "Intersection is ("<< x_i<< "," << y_i << ")" << endl;

   imshow( standard_name, standard_hough );
  // imshow( "original" , src );
}

/*****************************************************************************************************/
void Probabilistic_Hough( int, void* ,Mat input )
{
  
  vector<Vec4i> p_lines;
  cvtColor( input, probabilistic_hough, COLOR_GRAY2BGR );

  /// 2. Use Probabilistic Hough Transform
  HoughLinesP( input, p_lines, 1, CV_PI/180, min_threshold + p_trackbar, 30, 10 );

  /// Show the result
  for( size_t i = 0; i < p_lines.size(); i++ )
     {
       Vec4i l = p_lines[i];
       line( probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
     }

   imshow( probabilistic_name, probabilistic_hough );
}


