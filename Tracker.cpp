#include "Tracker.h"

Tracker::Tracker()
{
    //ctor
    //use any appropriate xml file, should be in same folder
    //obj = DetectionBasedTracker("haarcascade_frontalface.xml",param);
    m_nbPersonsTracked = 0;
    m_nbPersonsTrackedPrev = 0;
}


int Tracker::getXTracked(int personNumber)
{
    if ( personNumber >= 0 && personNumber < m_nbPersonsTracked)
    {
        //middle of rectangle
        return (m_personsTracked[personNumber].tl().x + m_personsTracked[personNumber].br().x) / 2;
    }
    return -1;  //if personNumber out of range
}

int Tracker::getYTracked(int personNumber)
{
    if ( personNumber >= 0 && personNumber < m_nbPersonsTracked)
    {
        //middle of rectangle
        return (m_personsTracked[personNumber].tl().y + m_personsTracked[personNumber].br().y) / 2;
    }
    return -1;  //if personNumber out of range
}

int Tracker::getXTrackedPrev(int personNumber)
{
    if ( personNumber >= 0 && personNumber < m_nbPersonsTrackedPrev)
    {
        //middle of rectangle
        return (m_personsTrackedPrev[personNumber].tl().x + m_personsTrackedPrev[personNumber].br().x) / 2;
    }
    return -1;  //if personNumber out of range
}

int Tracker::getYTrackedPrev(int personNumber)
{
    if ( personNumber >= 0 && personNumber < m_nbPersonsTrackedPrev)
    {
        //middle of rectangle
        return (m_personsTrackedPrev[personNumber].tl().y + m_personsTrackedPrev[personNumber].br().y) / 2;
    }
    return -1;  //if personNumber out of range
}

void Tracker::setPersonsTracked(vector< Rect_<int> > personsTracked)
{
    m_personsTracked = personsTracked;
}
void Tracker::setPersonsTrackedPrev(vector< Rect_<int> > personsTrackedPrev)
{
    m_personsTrackedPrev = personsTrackedPrev;
}

vector< Rect_<int> > Tracker::getPersonsTracked()
{
    return m_personsTracked;
}

vector< Rect_<int> > Tracker::getPersonsTrackedPrev()
{
    return m_personsTrackedPrev;
}

int Tracker::getNbPersonsTracked()
{
    return m_nbPersonsTracked;
}
int Tracker::getNbPersonsTrackedPrev()
{
    return m_nbPersonsTrackedPrev;
}
void Tracker::setNbPersonsTracked(int nbPersonsTracked)
{
    m_nbPersonsTracked = nbPersonsTracked;
}
void Tracker::setNbPersonsTrackedPrev(int nbPersonsTrackedPrev)
{
    m_nbPersonsTrackedPrev = nbPersonsTrackedPrev;
}
void Tracker::setPrev()
{
    setPersonsTrackedPrev(getPersonsTracked());
    setNbPersonsTrackedPrev(getNbPersonsTracked());
}
