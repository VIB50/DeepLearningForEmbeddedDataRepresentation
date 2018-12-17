#include "Motor.h"


/*
CLASS MEANT TO RUN AN EPOS2 MOTOR
use in this order :
initializeMotor()
setVelocity(velocity)
setPosition(position)
stopMotor()
closeDevice()
*/

Motor::Motor()
{
    //ctor
    m_usNodeId = 1;
    m_deviceName = "EPOS2";     //set to "EPOS4" if in possession of such device
    m_protocolStackName = "MAXON SERIAL V2";
    m_interfaceName = "USB";
    m_portName = "USB0";
    m_baudrate = 1000000;
    m_mode = 0;   //0 for velocity and 1 for position
    m_moving = 0; //to know if motor is moving or not
    m_defaultVelocity = 300;
    m_sign = 1; //to know in which direction we are moving, plus goes to the left of device
}

void Motor::logInfo(string msg)
{
    cout << msg << endl;
}

void Motor::separatorLine()
{
    const int lineLength = 65;
    for(int i=0; i<lineLength; i++)
    {
        cout << "-";
    }
    cout << endl;
}

void Motor::openDevice()
{
    char* pDeviceName = new char[255];
    char* pProtocolStackName = new char[255];
    char* pInterfaceName = new char[255];
    char* pPortName = new char[255];

    strcpy(pDeviceName, m_deviceName.c_str());
    strcpy(pProtocolStackName, m_protocolStackName.c_str());
    strcpy(pInterfaceName, m_interfaceName.c_str());
    strcpy(pPortName, m_portName.c_str());

    logInfo("Open device...");

    m_pKeyHandle = VCS_OpenDevice(pDeviceName, pProtocolStackName, pInterfaceName, pPortName, 0);

    if(m_pKeyHandle!=0)
    {
        unsigned int lBaudrate = 0;
        unsigned int lTimeout = 0;

        VCS_GetProtocolStackSettings(m_pKeyHandle, &lBaudrate, &lTimeout, 0);
        VCS_SetProtocolStackSettings(m_pKeyHandle, m_baudrate, lTimeout, 0);
        VCS_GetProtocolStackSettings(m_pKeyHandle, &lBaudrate, &lTimeout, 0);
    }
    else
    {
        m_pKeyHandle = 0;
    }

    delete []pDeviceName;
    delete []pProtocolStackName;
    delete []pInterfaceName;
    delete []pPortName;
}

void Motor::initializeMotor()
{
    unsigned int ulErrorCode = 0;
    openDevice();
    VCS_ClearFault(m_pKeyHandle, m_usNodeId, 0);
    VCS_SetEnableState(m_pKeyHandle, m_usNodeId, 0);
}

//STOPS THE MOTOR
void Motor::stopMotor()
{
    if(m_mode == 0)
    {
        VCS_HaltVelocityMovement(m_pKeyHandle, m_usNodeId, 0);
    }
    if(m_mode == 1)
    {
        VCS_HaltPositionMovement(m_pKeyHandle, m_usNodeId, 0);
    }
    m_moving = 0;
}

//TO CALL WHEN NO LONGER USE MOTOR
void Motor::closeDevice()
{
    logInfo("Close device");
    stopMotor();
    VCS_CloseDevice(m_pKeyHandle, 0);

}

void Motor::setVelocity(int velocity)
{
    m_mode = 0;
    m_moving = 1;
    if (velocity > 0) {m_sign = 1;}
    else {m_sign = -1;}
    VCS_ActivateProfileVelocityMode(m_pKeyHandle, m_usNodeId, 0);
    VCS_MoveWithVelocity(m_pKeyHandle, m_usNodeId, velocity, 0);
}

void Motor::setVelocityToLeft(int velocity)
{
    if (velocity > 0) {setVelocity(velocity);}
    else {setVelocity(-velocity);}
}
void Motor::setVelocityToRight(int velocity)
{
    if (velocity < 0) {setVelocity(velocity);}
    else {setVelocity(-velocity);}
}

void Motor::setPosition(int position)
{
    m_mode = 1;
    VCS_ActivateProfilePositionMode(m_pKeyHandle, m_usNodeId, 0);
    VCS_MoveToPosition(m_pKeyHandle, m_usNodeId, position, 0, 1, 0);
}

bool Motor::isMoving()
{
    if (m_moving == 1){return true;}
    return false;
}

bool Motor::isMovingToLeft()
{
    if (isMoving() == true)
    {
        if (m_sign == 1) { return true;}
    }
    return false;
}

bool Motor::isMovingToRight()
{
    if (isMoving() == true)
    {
        if (m_sign == -1) { return true;}
    }
    return false;
}


// TO TEST
/*
int main(int argc, char *argv[])
{
    Motor motor = Motor();
    motor.initializeMotor();
    motor.setVelocity(1000);
    sleep(2);
    motor.stopMotor();
}
*/
