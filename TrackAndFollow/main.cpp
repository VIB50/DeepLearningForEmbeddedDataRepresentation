#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Screen.h"
#include "Motor.h"
#include "Tracker.h"
#include <librealsense/rs.hpp>
#include <iostream>
#include <stdio.h>
#include "MotorCommand.h"

using namespace std;
using namespace cv;
using namespace rs;

vector< Rect_<int> > personsTracked;
/////////////
//VARIABLES//
/////////////

const char* WINDOW_NAME = "Display";
const char* WINDOW_NAME_CAMERA = "Camera";
//for RScamera
context ctx;
device& cam = *ctx.get_device( 0 );
intrinsics  colorIntrin;
int FRAMERATE = 60;
//common values for resolution
//default is 320x240
int INPUT_WIDTH = 320;
int INPUT_HEIGHT = 240;

/////////////
//FUNCTIONS//
/////////////

//for RS camera
// Initialize the application state. Upon success will return the static app_state vars address
bool initializeStreaming( )
{
       bool success = false;
       if( ctx.get_device_count( ) > 0 )
       {
             cam.enable_stream( rs::stream::color, INPUT_WIDTH, INPUT_HEIGHT, rs::format::rgb8, FRAMERATE );
             cam.enable_stream( rs::stream::depth, INPUT_WIDTH, INPUT_HEIGHT, rs::format::z16, FRAMERATE );
             cam.start( );
             success = true;
       }
       return success;
}

void initializeCamera()
{
    rs::log_to_console( rs::log_severity::warn );
    if( !initializeStreaming() )
    {
        std::cout << "Unable to locate a camera" << std::endl;
        rs::log_to_console( rs::log_severity::fatal );
    }
}

Mat getCamImage()
{
    if( cam.is_streaming( ) )
    {
        cam.wait_for_frames( );
        colorIntrin       = cam.get_stream_intrinsics( rs::stream::color );
        // Create color image
        cv::Mat rgb( colorIntrin.height, colorIntrin.width, CV_8UC3, (uchar *)cam.get_frame_data( rs::stream::color ) );
        cv::cvtColor( rgb, rgb, cv::COLOR_BGR2RGB );
        return rgb;
    }

}


int main(int argc, char *argv[])
{
    //create objects
    Screen screen = Screen();
    Screen screen2 = Screen(INPUT_WIDTH, INPUT_HEIGHT, 0);
    MotorCommand cmd = MotorCommand();
    Tracker tracker = Tracker();

    //create DetectionBasedTracker instance here because does not work in a class
    DetectionBasedTracker::Parameters param;
    param.maxObjectSize = 400;
    param.maxTrackLifetime = 20;
    param.minDetectionPeriod = 7;
    param.minNeighbors = 3;
    param.minObjectSize = 20;
    param.scaleFactor = 1.1;
    DetectionBasedTracker obj = DetectionBasedTracker("haarcascade_frontalface.xml",param);
    obj.run();

    //TEST FOR SCREEN CLASS
    /*
    screen.makeWindow(WINDOW_NAME_CAMERA);
    screen.updateCross(20,20,20,20);
    screen.updateDisplay(WINDOW_NAME_CAMERA);
    cvWaitKey(0);
    */

    //INITIALIZE RSCAMERA AND SCREEN
    initializeCamera();
    screen.makeWindow(WINDOW_NAME_CAMERA);
    screen2.makeWindow(WINDOW_NAME);
    Mat mat;
    //loop to display video
    while(1)
    {
        mat = getCamImage();
        Mat gray;
        cv::cvtColor(mat,gray,CV_RGB2GRAY);
        //dont know why, but if done in tracker class does not work
        // The class object is run on a gray image.
        obj.process(gray);
        // The results are a vector of Rect that enclose the object being tracked
        obj.getObjects(personsTracked);
        //give it to the Tracker class
        tracker.setPersonsTracked(personsTracked);
        tracker.setNbPersonsTracked(personsTracked.size());
        //draw cross and update display
        screen2.updateCross(tracker);
        screen2.updateDisplay(WINDOW_NAME);
        screen.show(WINDOW_NAME_CAMERA, mat);
        cvWaitKey(10);

        //give the command to the motor
        cmd.runMotor(tracker.getPersonsTracked());

        //set previous values to be able to erase previous cross
        tracker.setPrev();
    }


    //TEST FOR MOTOR CLASS
    /*
    Motor motor = Motor();
    motor.initializeMotor();
    */
}



