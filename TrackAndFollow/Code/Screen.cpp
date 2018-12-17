#include "Screen.h"

Screen::Screen()
{
    //ctor
    //resolution of the display (by default 320 and 240)
    m_width = 320;
    m_height = 240;
    //for drawing a cross
    m_thickness = 2;
    m_crossHeight = 12;
    m_eraseColor = Scalar(0,0,0);    //black by default
    m_drawColor = Scalar(250,250,250);     //blank by default
    //create image to display
    m_img = cvCreateImage(cvSize(m_width, m_height), 8, 3);
    cvSet(m_img, m_eraseColor); //make it black
    //fullscreen or not
    m_fullscreen = 0;       //not by default
}


Screen::Screen(int width, int height, int fullscreen)
{
    //ctor
    //resolution of the display (by default 320 and 240)
    m_width = width;
    m_height = height;
    //for drawing a cross
    m_thickness = 2;
    m_crossHeight = 12;
    m_eraseColor = Scalar(0,0,0);    //black by default
    m_drawColor = Scalar(250,250,250);     //blank by default
    //create image to display
    m_img = cvCreateImage(cvSize(m_width, m_height), 8, 3);
    cvSet(m_img, m_eraseColor); //make it black
    //fullscreen or not
    m_fullscreen = fullscreen;
}

Screen::Screen(int width, int height)
{
    //ctor
    //resolution of the display (by default 320 and 240)
    m_width = width;
    m_height = height;
    //for drawing a cross
    m_thickness = 2;
    m_crossHeight = 12;
    m_eraseColor = Scalar(0,0,0);    //black by default
    m_drawColor = Scalar(250,250,250);     //blank by default
    //create image to display
    m_img = cvCreateImage(cvSize(m_width, m_height), 8, 3);
    cvSet(m_img, m_eraseColor); //make it black
    //fullscreen or not
    m_fullscreen = 0;   //not by default
}



void Screen::setThickness(int a)
{m_thickness = a;}

void Screen::setCrossHeight(int a)
{m_crossHeight = a;}

void Screen::setEraseColor(Scalar s)
{m_eraseColor = s;}

void Screen::setDrawColor(Scalar s)
{m_drawColor = s;}

//draws a line on m_img
void Screen::drawLine(int x1, int y1, int x2, int y2)
{
    Point pt1 = Point(x1,y1);
    Point pt2 = Point(x2,y2);
    cvLine(m_img, pt1, pt2, m_drawColor, m_thickness);
}

void Screen::eraseLine(int x1, int y1, int x2, int y2)
{
    Point pt1 = Point(x1,y1);
    Point pt2 = Point(x2,y2);
    cvLine(m_img, pt1, pt2, m_eraseColor, m_thickness);
}

void Screen::updateCross(Tracker tracker)
{
    //first erase previous cross
    for (int i = 0; i < tracker.getNbPersonsTrackedPrev(); i++) {
        //we know xTracker and yTracker and we want to draw the cross
        int xTrackerPrev = tracker.getXTrackedPrev(i);
        int yTrackerPrev = tracker.getYTrackedPrev(i);
        //diagonal cross
        eraseLine(xTrackerPrev - m_crossHeight, yTrackerPrev - m_crossHeight, xTrackerPrev + m_crossHeight, yTrackerPrev + m_crossHeight);
        eraseLine(xTrackerPrev + m_crossHeight, yTrackerPrev - m_crossHeight, xTrackerPrev - m_crossHeight, yTrackerPrev + m_crossHeight);
    }
    for (int i = 0; i < tracker.getNbPersonsTracked(); i++) {
        //draw new cross
        int xTracker = tracker.getXTracked(i);
        int yTracker = tracker.getYTracked(i);
        drawLine(xTracker - m_crossHeight, yTracker - m_crossHeight, xTracker + m_crossHeight, yTracker + m_crossHeight);
        drawLine(xTracker + m_crossHeight, yTracker - m_crossHeight, xTracker - m_crossHeight, yTracker + m_crossHeight);
    }
}


void Screen::makeWindow(const char* windowName)
{
    cvNamedWindow(windowName, CV_WINDOW_NORMAL);
    //make it full screen
    if (m_fullscreen == 1){cvSetWindowProperty(windowName, CV_WND_PROP_FULLSCREEN, 1);}
}

void Screen::updateDisplay(const char* windowName)
{
    cvShowImage(windowName, m_img);
}

void Screen::show(const char* windowName, Mat mat)
{
    imshow(windowName, mat);
}
