#ifndef SCREEN_H
#define SCREEN_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Tracker.h"

#include <iostream>
using namespace std;
using namespace cv;

/*
THIS CLASS IS MADE TO CREATE A FULL SCREEN WINDOW THAT IS PROJECTED VIA A PROJECTOR
another class tracks a person and finds a rectangle aroung it
we give it two points of this rectangle and it draws a cross inside it
this is supposed to be projected in real time on the person tracked

use as follow
makeWindow(name)
updateCross(int xTracker, int yTracker, int xTrackerPrev, int yTrackerPrev) //for tracking
updateDisplay(name)
*/


class Screen
{
    public:
        Screen();
        Screen(int width, int height, int fullscreen);
        Screen(int width, int height);
        void setThickness(int a);
        void setCrossHeight(int a);
        void setEraseColor(Scalar s);
        void setDrawColor(Scalar s);
        void drawLine(int x1, int y1, int x2, int y2);
        void eraseLine(int x1, int y1, int x2, int y2);
        void updateCross(Tracker tracker);
        void makeWindow(const char* windowName);
        void updateDisplay(const char* windowName);
        void show(const char* windowName, Mat mat);
    protected:
    private:
        //the image to show
        IplImage* m_img;
        //resolution of the display (by default 320 and 240)
        int m_width;
        int m_height;
        //for drawing a cross
        int m_thickness;
        int m_crossHeight;
        Scalar m_eraseColor;    //black by default
        Scalar m_drawColor;     //blank by default
        //0 for window, 1 for full screen
        int m_fullscreen;


};

#endif // SCREEN_H
