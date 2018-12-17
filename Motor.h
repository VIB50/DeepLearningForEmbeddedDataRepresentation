#ifndef MOTOR_H
#define MOTOR_H

#include <iostream>
#include "Definitions.h"
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class Motor
{
    public:
        Motor();
        void openDevice();
        void initializeMotor();
        void stopMotor();
        void closeDevice();
        void setVelocity(int velocity);
        void setPosition(int position);
        void setVelocityToLeft(int velocity);
        void setVelocityToRight(int velocity);
        bool isMoving();
        bool isMovingToLeft();
        bool isMovingToRight();
    protected:
    private:
        void* m_pKeyHandle;
        unsigned short m_usNodeId;
        string m_deviceName;
        string m_protocolStackName;
        string m_interfaceName;
        string m_portName;
        int m_baudrate;
        int m_mode;   //0 for velocity and 1 for position
        int m_moving; //to know if motor is moving or not
        int m_defaultVelocity;
        int m_sign;       //to know in which direction we are rotating
        //const string m_programName;
        void logInfo(string msg);
        void separatorLine();
};

#endif // MOTOR_H
