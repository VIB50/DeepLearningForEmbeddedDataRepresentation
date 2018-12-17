#ifndef MOTORCOMMAND_H
#define MOTORCOMMAND_H

#include "Motor.h"
#include "Timer.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

/*
THIS CLASS IS MEANT TO ELABORATE A COMMAND AND RUN THE MOTOR
you just have to run runCommand(vector< Rect_<int> > trackerOutput)

you can set the margin (m_margin) which is a percentage of the WIDTH of the screen
the basic idea is to move if the target is outside the screen minus the margin on each side

since the tracker loses accuracy while moving, we update the command while moving only if the number of persons tracked found now matches the number of persons tracked before moving
*/

using namespace cv;
using namespace std;

class MotorCommand
{
    public:
        MotorCommand();
        void runMotor(vector< Rect_<int> > persons);
        void updateNbPersonTracked(vector< Rect_<int> > persons);
        void updateExtremePersonID(vector< Rect_<int> > persons);
        void updateRightPersonThatFitID(vector< Rect_<int> > persons);
        void print(int info);
        void print(string info);
    protected:
    private:
        void updateXcommand(vector< Rect_<int> > persons);
        int findNextPersonToLeftID(vector< Rect_<int> > persons, int i);
        int getX(vector< Rect_<int> > persons, int personID);
        int getY(vector< Rect_<int> > persons, int personID);
        Motor m_motor;
        int m_extremeLeftPersonID;
        int m_extremeRightPersonID;
        int m_rightPersonThatFitID;
        int m_nbPersonTracked;
        int m_SpacingX;
        int m_Xcommand;
        int m_WIDTH;
        int m_margin;
        int m_DEFAULT_VELOCITY;
        Timer timer;
        int m_motorMovingPrevState;
        int m_prevNbPersonTracked;
        void updateStatNbPersonsTracked();
        int m_increments;
        int m_statSum;
        bool m_trackerFailed;
        int m_nbOfFramePassed;
        int m_nbOftrackerFails;
        void updateTrackerFailed();

};

#endif // MOTORCOMMAND_H
