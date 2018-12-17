#include "MotorCommand.h"


/*
THIS CLASS IS MEANT TO ELABORATE A COMMAND AND RUN THE MOTOR
you just have to run runCommand(vector< Rect_<int> > trackerOutput)

you can set the margin (m_margin) which is a percentage of the WIDTH of the screen
the basic idea is to move if the target is outside the screen minus the margin on each side

since the tracker loses accuracy while moving, we update the command while moving only if the number of persons tracked found now matches the number of persons tracked before moving
*/
MotorCommand::MotorCommand()
{
    //ctor
    m_motor = Motor();
    m_motor.initializeMotor();
    m_extremeLeftPersonID = 0;
    m_extremeRightPersonID = 0;
    m_rightPersonThatFitID = 0;
    m_nbPersonTracked = 0;
    m_SpacingX = 0;
    m_Xcommand = -1;  //-1 means that we should not move the motor
    m_WIDTH = 320;
    m_margin = 0.2*m_WIDTH;
    m_DEFAULT_VELOCITY = 1000;
    timer = Timer();
    m_motorMovingPrevState = 0;
    m_prevNbPersonTracked = 0;
    m_increments = 0;
    m_statSum = 0;
    m_trackerFailed = false;
    m_nbOfFramePassed = 0;
    m_nbOftrackerFails = 0;
}

void MotorCommand::print(int info)
{
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << info << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
}
void MotorCommand::print(string info)
{
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << info << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
    cout << "---------------" << endl;
}
void MotorCommand::updateNbPersonTracked(vector< Rect_<int> > persons)
{
    m_nbPersonTracked = persons.size();
    //keep nb of persons tracked before motor moves, because after occur many tracking errors
    if (m_motor.isMoving() == false)
    {
        m_prevNbPersonTracked = m_nbPersonTracked;
    }
}

int MotorCommand::getX(vector< Rect_<int> > persons, int personNumber)
{
    updateNbPersonTracked(persons);
    if ( personNumber >= 0 && personNumber < m_nbPersonTracked)
    {
        //middle of rectangle
        return (persons[personNumber].tl().x + persons[personNumber].br().x) / 2;
    }
    return -1;  //if personNumber out of range
}

int MotorCommand::getY(vector< Rect_<int> > persons, int personNumber)
{
    updateNbPersonTracked(persons);
    if ( personNumber >= 0 && personNumber < m_nbPersonTracked)
    {
        //middle of rectangle
        return (persons[personNumber].tl().y + persons[personNumber].br().y) / 2;
    }
    return -1;  //if personNumber out of range
}

void MotorCommand::updateExtremePersonID(vector< Rect_<int> > persons)
{
    updateNbPersonTracked(persons);
    int leftX = getX(persons,0);
    int rightX = leftX;
    m_extremeRightPersonID = 0;
    m_extremeLeftPersonID = 0;
    for (int i = 0; i < m_nbPersonTracked; i++) {
        int x = getX(persons, i);
        if (x > rightX)
        {
            //person i more to the right
            rightX = x;
            m_extremeRightPersonID = i;
        }
        if (x < leftX)
        {
            //person i more to the left
            leftX = x;
            m_extremeLeftPersonID = i;
        }
    }
}

int MotorCommand::findNextPersonToLeftID(vector< Rect_<int> > persons, int i)
{
    updateNbPersonTracked(persons);
    int rightX = getX(persons, i);
    int resultX = getX(persons, i);
    int result = i;
    for (int i = 0; i < m_nbPersonTracked; i++) {
        int x = getX(persons, i);
        if (x < rightX) //we only care for people to the left of person i
        {
            if (x > resultX) //we found someone to the left of i, closer than previous one
            {
                resultX = x;
            }
        }
    }
    return result;
}

void MotorCommand::updateRightPersonThatFitID(vector< Rect_<int> > persons)
{
    updateExtremePersonID(persons);
    bool notGood = true;
    int rightPersonID = m_extremeRightPersonID;
    while(notGood)
    {
        int spacing = getX(persons, rightPersonID) - getX(persons, m_extremeLeftPersonID);  //space between left person and currently observed right person
        if (spacing < m_WIDTH - 2*m_margin) //if smaller than widht minus a margin on each side
        {
            notGood = false;    //good then, two people currently selected can fit within the margins
        }
        else
        {
            rightPersonID = findNextPersonToLeftID(persons, rightPersonID);
        }
    }
    m_rightPersonThatFitID = rightPersonID;
}

void MotorCommand::updateTrackerFailed()
{
    //only important if motor moving, where might make errors
    if (m_motor.isMoving() == true)
    {
        //if does not find same number of persons than before moving
        if (m_nbPersonTracked != m_prevNbPersonTracked)
        {
            m_nbOftrackerFails = m_nbOftrackerFails + 1;
        }
        m_nbOfFramePassed = m_nbOfFramePassed + 1;

        //if it failed too many times, then declare it failed
        //adjust the value if wanted
        if ( m_nbOftrackerFails > 0.2 * m_nbOfFramePassed)
        {
            m_trackerFailed = true;
        }
    }
    else
    {
        //reset if not moving
        m_nbOfFramePassed = 0;
        m_nbOftrackerFails = 0;
        m_trackerFailed = false;
    }
}

//m_Xcommand has the value -1 if we want the motor to stop
// if not, it has the value of the X coordinate we want in the center of the view
// it does not update if the motor is moving and the number of persons found is not the number of persons found before it moved
void MotorCommand::updateXcommand(vector< Rect_<int> > persons)
{
    //return value of exception if no one to be tracked, and not caused to tracking errors due to motor moving
    if (m_nbPersonTracked == 0 and m_motor.isMoving() == false) {m_Xcommand = -1;}
    //if motor does not move, elaborate command
    if (m_motor.isMoving() == false)
    {
        //find the middle of the two more spaced persons that fit
        m_Xcommand = ( getX(persons, m_extremeLeftPersonID) + getX(persons, m_rightPersonThatFitID) ) / 2;
        m_motorMovingPrevState = 0;
    }
    //conditions to stop motor
    if (m_motor.isMoving() == true)
    {
        //begin timer to stop motor after said time if can no longer find persons it tracked before moving
        if (m_motorMovingPrevState == 0)    //was not moving just before
        {
            timer.setCurrentTime(); //time where begin to move
        }
        // update m_Xcommand only if number of people seen when motor did not move matches current nb of people seen
        // prevents some tracking errors due to motion
        if (m_nbPersonTracked == m_prevNbPersonTracked)
        {
            //same as when not moving, assume found same persons
            m_Xcommand = ( getX(persons, m_extremeLeftPersonID) + getX(persons, m_rightPersonThatFitID) ) / 2;
        }
        //stop motor if for more than  a TIME_CST not a single frame matched the number of persons seen before
        if ( timer.overtime() == true and m_trackerFailed == true )
        {
            //timer.setCurrentTime();
            m_Xcommand = -1;    //stop the motor
        }
        m_motorMovingPrevState = 1;
    }
}


//updates everything and runs or stop the motor according to the m_Xcommand value
void MotorCommand::runMotor(vector< Rect_<int> > persons)
{
    //update all data
    updateNbPersonTracked(persons);
    updateExtremePersonID(persons);
    updateRightPersonThatFitID(persons);
    updateXcommand(persons);
    updateTrackerFailed();
    //print(m_Xcommand);
    //we now know the IDs of the more spaced two persons that can fit within the margins
    //the condition in updateRightPersonThatFitID() implies that if one peron is out of the margins, the other is not
    if (m_Xcommand == -1)   //asks to stop
    {
        m_motor.stopMotor();
    }
    else
    {
        //just to make sure dont make multiple orders
        bool alreadyDidSomething = false;
        //conditions to set motor to move
        if (m_motor.isMoving() == false)
        {
            if (getX(persons, m_extremeLeftPersonID and alreadyDidSomething == false) < m_margin)   //left person outside of left margin
            {
                m_motor.setVelocityToLeft(m_DEFAULT_VELOCITY);
                alreadyDidSomething = true;
            }
            if (getX(persons, m_rightPersonThatFitID) > m_WIDTH - m_margin and alreadyDidSomething == false)   //right person outside of right margin
            {
                m_motor.setVelocityToRight(m_DEFAULT_VELOCITY);
                alreadyDidSomething = true;
            }
        }
        //conditions to stop motor
        if (m_motor.isMovingToRight() == true and alreadyDidSomething == false)
        {
            if (m_Xcommand < m_WIDTH/2){m_motor.stopMotor(); alreadyDidSomething = true;} //stop if X command reached the center of the screen
        }
        if (m_motor.isMovingToLeft() == true and alreadyDidSomething == false)  //moving to left
        {
            if (m_Xcommand > m_WIDTH/2){m_motor.stopMotor(); alreadyDidSomething = true;} //stop if X command reached the center of the screen
        }
    }
}
