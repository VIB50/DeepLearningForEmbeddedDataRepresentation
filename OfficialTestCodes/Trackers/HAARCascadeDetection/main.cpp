#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include <librealsense/rs.hpp>

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace rs;

//variables

//HAAR cascade

//uncomment desired file to use
//String pedestrianCascade = "haarcascade_pedestrian.xml";
//String pedestrianCascade = "haarcascade_pedestrians.xml";
//String pedestrianCascade = "haarcascade_upperbody.xml"; //best working
String pedestrianCascade = "haarcascade_frontalface.xml";   //for tests

CascadeClassifier pedestrian;
String windowName = "Pedestrian Detection";

// Window size and frame rate
int const INPUT_WIDTH      = 320;
int const INPUT_HEIGHT     = 240;
int const FRAMERATE        = 60;
char* const WINDOW_RGB     = "RGB Image";

//RealSense camera
context      _rs_ctx;
device&      _rs_camera = *_rs_ctx.get_device( 0 );
intrinsics  _color_intrin;
bool         _loop = true;


//functions

// Initialize the application state. Upon success will return the static app_state vars address
bool initialize_streaming( )
{
       bool success = false;
       if( _rs_ctx.get_device_count( ) > 0 )
       {
             _rs_camera.enable_stream( rs::stream::color, INPUT_WIDTH, INPUT_HEIGHT, rs::format::rgb8, FRAMERATE );
             _rs_camera.start( );
             success = true;
       }
       return success;
}

Mat getMatImage()
{
    _color_intrin       = _rs_camera.get_stream_intrinsics( rs::stream::color );

       // Create color image
       cv::Mat rgb( _color_intrin.height,
                            _color_intrin.width,
                            CV_8UC3,
                            (uchar *)_rs_camera.get_frame_data( rs::stream::color ) );

       cv::cvtColor( rgb, rgb, cv::COLOR_BGR2RGB );
       return rgb;
}

void detectAndDisplay(Mat frame)
{
    std::vector<Rect> pedestrians;
    Mat grayFrame;

    cvtColor(frame, grayFrame, CV_BGR2GRAY);
    equalizeHist(grayFrame, grayFrame);

    //detect pedestrians
    pedestrian.load(pedestrianCascade);
    pedestrian.detectMultiScale(grayFrame, pedestrians, 1.1,2,0|CV_HAAR_SCALE_IMAGE, Size(30,30));
    //cout << pedestrians.size() << endl;
    for(size_t i = 0; i < pedestrians.size(); i++)
    {
        Point center(pedestrians[i].x + pedestrians[i].width*0.5, pedestrians[i].y + pedestrians[i].height*0.5);
        rectangle( frame, Point(pedestrians[i].x, pedestrians[i].y), Point(pedestrians[i].x + pedestrians[i].width, pedestrians[i].y + pedestrians[i].height),Scalar(255,0,255),2,1);
    }
    imshow( WINDOW_RGB, frame );
    cvWaitKey( 1 );
}

static void onMouse( int event, int x, int y, int, void* window_name )
{
       if( event == cv::EVENT_LBUTTONDOWN )
       {
             _loop = false;
       }
}

void initializeRS()
{
    rs::log_to_console( rs::log_severity::warn );
    if( !initialize_streaming( ) )
    {
        std::cout << "Unable to locate a camera" << std::endl;
        rs::log_to_console( rs::log_severity::fatal );
    }
}

int main (int argc, const char** argv) try
{
    //initialize camera
    initializeRS();
    cv::namedWindow( WINDOW_RGB, 0 );
    cv::setMouseCallback( WINDOW_RGB, onMouse, WINDOW_RGB );

    Mat img = Mat();

    //detect and display loop
    while( _loop )
       {
             if( _rs_camera.is_streaming( ) )
             {
                    _rs_camera.wait_for_frames( );
                    img = getMatImage();
                    detectAndDisplay(img);
             }
       }
       //stops camera
    _rs_camera.stop( );
    cv::destroyAllWindows( );
    return EXIT_SUCCESS;
}
catch( const rs::error & e )
{
       std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
       return EXIT_FAILURE;
}
catch( const std::exception & e )
{
       std::cerr << e.what() << std::endl;
       return EXIT_FAILURE;
}
