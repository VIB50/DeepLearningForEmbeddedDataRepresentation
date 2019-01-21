#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

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



using namespace std;

void* g_pKeyHandle = 0;
unsigned short g_usNodeId = 1;
string g_deviceName = "EPOS2";
string g_protocolStackName;
string g_interfaceName;
string g_portName;
int g_baudrate = 0;

int mode = 0;   //0 for velocity and 1 for position

const string g_programName = "HelloEposCmd";

void  LogError(string functionName, int p_lResult, unsigned int p_ulErrorCode);
void  LogInfo(string message);
void  PrintHeader();
void  PrintSettings();
void  SetDefaultParameters();

void LogError(string functionName, int p_lResult, unsigned int p_ulErrorCode)
{
    cerr << g_programName << ": " << functionName << " failed (result=" << p_lResult << ", errorCode=0x" << std::hex << p_ulErrorCode << ")"<< endl;
}

void LogInfo(string message)
{
    cout << message << endl;
}

void SeparatorLine()
{
    const int lineLength = 65;
    for(int i=0; i<lineLength; i++)
    {
        cout << "-";
    }
    cout << endl;
}

void PrintSettings()
{
    stringstream msg;

    msg << "default settings:" << endl;
    msg << "node id             = " << g_usNodeId << endl;
    msg << "device name         = '" << g_deviceName << "'" << endl;
    msg << "protocal stack name = '" << g_protocolStackName << "'" << endl;
    msg << "interface name      = '" << g_interfaceName << "'" << endl;
    msg << "port name           = '" << g_portName << "'"<< endl;
    msg << "baudrate            = " << g_baudrate;

    LogInfo(msg.str());

    SeparatorLine();
}

void SetDefaultParameters()
{
    //USB
    g_usNodeId = 1;
    g_deviceName = "EPOS2";
    g_protocolStackName = "MAXON SERIAL V2";
    g_interfaceName = "USB";
    g_portName = "USB0";
    g_baudrate = 1000000;
}

void OpenDevice()
{
    char* pDeviceName = new char[255];
    char* pProtocolStackName = new char[255];
    char* pInterfaceName = new char[255];
    char* pPortName = new char[255];

    strcpy(pDeviceName, g_deviceName.c_str());
    strcpy(pProtocolStackName, g_protocolStackName.c_str());
    strcpy(pInterfaceName, g_interfaceName.c_str());
    strcpy(pPortName, g_portName.c_str());

    LogInfo("Open device...");

    g_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, 0);

    if(g_pKeyHandle!=0)
    {
        unsigned int lBaudrate = 0;
        unsigned int lTimeout = 0;

        VCS_GetProtocolStackSettings(g_pKeyHandle, &lBaudrate, &lTimeout, 0);
        VCS_SetProtocolStackSettings(g_pKeyHandle, g_baudrate, lTimeout, 0);
        VCS_GetProtocolStackSettings(g_pKeyHandle, &lBaudrate, &lTimeout, 0);
    }
    else
    {
        g_pKeyHandle = 0;
    }

    delete []pDeviceName;
    delete []pProtocolStackName;
    delete []pInterfaceName;
    delete []pPortName;
}

void PrintHeader()
{
    SeparatorLine();

    LogInfo("Epos Command Library Example Program, (c) maxonmotor ag 2014-2017");

    SeparatorLine();
}

void initialize()
{
    unsigned int ulErrorCode = 0;
    SetDefaultParameters();
    PrintSettings();
    OpenDevice();
}

void setVelocity(int velocity)
{
    mode = 0;
    VCS_ActivateProfileVelocityMode(g_pKeyHandle, g_usNodeId, 0);
    VCS_MoveWithVelocity(g_pKeyHandle, g_usNodeId, velocity, 0);
}

void setPosition(int position)
{
    mode = 1;
    VCS_ActivateProfilePositionMode(g_pKeyHandle, g_usNodeId, 0);
    VCS_MoveToPosition(g_pKeyHandle, g_usNodeId, position, 0, 1, 0);
}

void stopMotor()
{
    if(mode == 0)
    {
        VCS_HaltVelocityMovement(g_pKeyHandle, g_usNodeId, 0);
    }
    if(mode == 1)
    {
        VCS_HaltPositionMovement(g_pKeyHandle, g_usNodeId, 0);
    }
}


void CloseDevice()
{
    LogInfo("Close device");
    stopMotor();
    VCS_CloseDevice(g_pKeyHandle, 0);

}


int main(int argc, char** argv)
{
    initialize();

VCS_GetFaultState(g_pKeyHandle, g_usNodeId, 0, 0 );
VCS_ClearFault(g_pKeyHandle, g_usNodeId, 0);
VCS_GetEnableState(g_pKeyHandle, g_usNodeId, 0, 0);
VCS_SetEnableState(g_pKeyHandle, g_usNodeId, 0);


    setPosition(10000);
    sleep(1);
    setPosition(-5000);
    sleep(1);
setPosition(0);
    sleep(1);
    setVelocity(1000);
    sleep(1);
    stopMotor();


    CloseDevice();


}

