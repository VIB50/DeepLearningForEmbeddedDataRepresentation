#ifndef TRACKER_H
#define TRACKER_H

#include "opencv2/contrib/detection_based_tracker.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

class Tracker
{
    public:
        Tracker();
        //where detected people s coordinates are stored
        vector< Rect_<int> > m_personsTracked;
        //store previous tracked persons to erase cross
        vector< Rect_<int> > m_personsTrackedPrev;
        int m_nbPersonsTracked;
        int m_nbPersonsTrackedPrev;
        //coordinates of the person tracked, and previous position
        //DetectionBasedTracker::Parameters param;
        //DetectionBasedTracker obj = DetectionBasedTracker("haarcascade_frontalface.xml",param);
        int getXTracked(int personNumber);
        int getYTracked(int personNumber);
        int getXTrackedPrev(int personNumber);
        int getYTrackedPrev(int personNumber);
        void setPersonsTracked(vector< Rect_<int> > personsTracked);
        void setPersonsTrackedPrev(vector< Rect_<int> > personsTrackedPrev);
        int getNbPersonsTracked();
        int getNbPersonsTrackedPrev();
        vector< Rect_<int> > getPersonsTracked();
        vector< Rect_<int> > getPersonsTrackedPrev();
        void setNbPersonsTracked(int nbPersonsTracked);
        void setNbPersonsTrackedPrev(int nbPersonsTrackedPrev);
        void setPrev();
    protected:
    private:
};

#endif // TRACKER_H
