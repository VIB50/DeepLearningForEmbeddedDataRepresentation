#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Timer.h"

#include <librealsense/rs.hpp>
#include <iostream>

using namespace std;
using namespace cv;
using namespace rs;

//for RScamera
context ctx;
device& cam = *ctx.get_device( 0 );
intrinsics  depthIntrin;
intrinsics  colorIntrin;
int FRAMERATE = 60;
Mat depth;
Mat color;

//common values for resolution
//default is 320x240
int INPUT_WIDTH = 320;
int INPUT_HEIGHT = 240;

const char* WINDOW_DEPTH = "Depth Stream";
const char* WINDOW_COLOR = "Color Stream";

//display loop
bool         loop = true;

//to smooth depth mat image
Mat binaryDepth = Mat(INPUT_HEIGHT, INPUT_WIDTH, CV_8UC1);
Mat averageDepth = Mat(INPUT_HEIGHT, INPUT_WIDTH, CV_8UC1);
Mat depthCorrectionResult = Mat(INPUT_HEIGHT, INPUT_WIDTH, CV_8UC1);
Mat contourColor = Mat(INPUT_HEIGHT, INPUT_WIDTH, CV_8UC3);
Mat contourColorAverage = Mat(INPUT_HEIGHT, INPUT_WIDTH, CV_8UC1);
Mat contourColorBinary = Mat(INPUT_HEIGHT, INPUT_WIDTH, CV_8UC1);
int increments = 0;
//timer
Timer timer = Timer();

//When looking for biggest squares
//coordinates and size of the biggest square we can fit
int xCenterOfSquare1 = 0;
int yCenterOfSquare1 = 0;
int squareEdge1 = 0;
int xSquareContour1 = 0;
int ySquareContour1 = 0;

int x1surfaceRectangle = 0;
int y1surfaceRectangle = 0;
int x2surfaceRectangle = 0;
int y2surfaceRectangle = 0;

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

Mat getCamDepthImage()
{
    if( cam.is_streaming( ) )
    {
        cam.wait_for_frames( );
        depthIntrin       = cam.get_stream_intrinsics( rs::stream::depth );
        // Create depth image
        cv::Mat depth16( depthIntrin.height, depthIntrin.width, CV_16U, (uchar *)cam.get_frame_data( rs::stream::depth ) );
        cv::Mat depth8u = depth16;
        depth8u.convertTo( depth8u, CV_8UC1, 255.0/1000 );
        return depth8u;
    }
}

Mat getCamColorImage()
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


void setupWindows( )
{
       cv::namedWindow( WINDOW_DEPTH, 0 );
       cv::namedWindow( WINDOW_COLOR, 0 );
}

//to print info
void print(int i)
{
    cout << i <<endl;
}

//change the value of a mat's pixel
//black and white (1 value)
void setPixelValue(Mat mat, int x, int y, int value)
{
    if (x >= 0 and x < mat.cols)
    {
        if (y >= 0 and y < mat.rows)
        {
            if (value >= 0 and y < 256)
            {
                mat.at<uchar>(y, x) = value;
            }
        }
    }
}

int getPixelValue(Mat mat, int x, int y)
{
    if (x >= 0 and x < mat.cols)
    {
        if (y >= 0 and y < mat.rows)
        {
            return mat.at<uchar>(y, x);
        }
    }
    //if something wrong in data provided
    return -1;
}

void resetMat(Mat mat)
{
    for (int x = 0; x < mat.cols; x++)
    {
        for (int y = 0; y < mat.rows; y++)
        {
            setPixelValue(mat, x, y, 125);
        }
    }
}

void updateBinaryDepth()
{
    for (int x = 0; x < binaryDepth.cols; x++)
    {
        for (int y = 0; y < binaryDepth.rows; y++)
        {
            int criteria = 220;
            int average = getPixelValue(averageDepth, x, y);
            if (average >= criteria)
            {
                setPixelValue(binaryDepth, x, y, 255);
            }
            if (average < criteria)
            {
                setPixelValue(binaryDepth, x, y, 0);
            }
        }
    }
    //try to get rid of remaining noise
    //int threshold = 9;
    //threshold(binaryDepth, depthCorrectionResult, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //medianBlur(depthCorrectionResult, depthCorrectionResult, 1);
    //blur(depthCorrectionResult, depthCorrectionResult, Size(3,3));
    //Canny(binaryDepth, depthCorrectionResult, threshold, 3*threshold, 3);

}

void updateContour()
{
    //you can change the threshold value to get more details on detouring the color image (more the less precise)
    int threshold = 10;
    cvtColor(color, contourColor, CV_BGR2GRAY);
    blur(contourColor, contourColor, Size(9,9));
    Canny(contourColor, contourColor, threshold, 3*threshold, 3);
}

void updateAverageDepth()
{
    for (int x = 0; x < averageDepth.cols; x++)
    {
        for (int y = 0; y < averageDepth.rows; y++)
        {
            //this way we get the sum before the division (when making an average value
            int sum = getPixelValue(averageDepth, x, y) * increments;
            //add the new occurence to the sum
            sum = sum + getPixelValue(depth, x, y);
            //set the average to its new value
            int average = sum / (increments + 1);
            setPixelValue(averageDepth, x, y, average);
        }
    }
}



//looks if different values in a square around a pixel. i tells how big the square is
//return true if values in the square are all the same as center
bool sameValueInSquare(Mat mat, int x, int y, int i)
{
    int pixelValue = getPixelValue(mat, x, y);
    //left column
    int xSquare = x-i;
    for (int k=-i; k <= i; k++)
    {
        int squarePixelValue = getPixelValue(mat, xSquare, y + k);
        //if different value
        if (pixelValue != squarePixelValue)
        {
            return false;
        }
    }
    //right column
    xSquare = x+i;
    for (int k=-i; k <= i; k++)
    {
        int squarePixelValue = getPixelValue(mat, xSquare, y + k);
        //if different value
        if (pixelValue != squarePixelValue)
        {
            return false;
        }
    }
    //higher part of square left
    int ySquare = y+i;
    for (int k=-i+1; k <= i-1; k++)
    {
        int squarePixelValue = getPixelValue(mat, x + k, ySquare);
        //if different value
        if (pixelValue != squarePixelValue)
        {
            return false;
        }
    }
    //lower part of square left
    ySquare = y-i;
    for (int k=-i+1; k <= i-1; k++)
    {
        int squarePixelValue = getPixelValue(mat, x + k, ySquare);
        //if different value
        if (pixelValue != squarePixelValue)
        {
            return false;
        }
    }
    return true;
}

int UP_SEARCH = 0;
int DOWN_SEARCH = 1;
int LEFT_SEARCH = 2;
int RIGHT_SEARCH = 3;
//returns the number of consecutive black pixels from (x,y), in the searchValue direction
int NbOfBlackPixelsInARow(int x, int y, int searchValue)
{
    if(searchValue == UP_SEARCH)
    {
        int up = 1;
        //while dont find white pixel going up
        while (getPixelValue(contourColor, x, y-up) == 0)
        {
            up = up + 1;
        }
        return (up-1);
    }
    if(searchValue == DOWN_SEARCH)
    {
        int down = 1;
        //while dont find white pixel going up
        while (getPixelValue(contourColor, x, y+down) == 0)
        {
            down = down + 1;
        }
        return (down-1);
    }
    if(searchValue == LEFT_SEARCH)
    {
        int left = 1;
        //while dont find white pixel going up
        while (getPixelValue(contourColor, x-left, y) == 0)
        {
            left = left + 1;
        }
        return (left-1);
    }
    if(searchValue == RIGHT_SEARCH)
    {
        int right = 1;
        //while dont find white pixel going up
        while (getPixelValue(contourColor, x+right, y) == 0)
        {
            right = right + 1;
        }
        return (right-1);
    }
}
/*
//function to look in coutourColor from (x,y), horizontally or vertically (regarding last value given : 0 for vertical, 1 horizontal)
//return the number of black pixel in a row from (x,y)
int NbOfSamePixelsInARow(int x, int y, searchValue)
{
    //first pixel should be black
    if (getPixelValue(contourColor, x, y) == 0)
    {
        //vertical search
        if(searchValue == 0)
        {
            int up = 0;
            //while dont find white pixel going up
            while (getPixelValue(contourColor, x, y-up) == 0)
            {
                up = up + 1;
            }
            int down = 0;
            while (getPixelValue(contourColor, x, y+down) == 0)
            {
                down = down + 1;
            }
            return (down + up + 1);
        }
        //horizontal search
        if(searchValue == 1)
        {
            int left = 0;
            //while dont find white pixel going left
            while (getPixelValue(contourColor, x, y-left) == 0)
            {
                left = left + 1;
            }
            int right = 0;
            while (getPixelValue(contourColor, x, y+right) == 0)
            {
                right = right + 1;
            }
            return (right + left + 1);
        }
    }
}
*/

//check if can fit a square around a point of pixels of all the same value
bool allSamePixelsInSquare(int x, int y, int edgeSize)
{
    for (int i = 1; i <= edgeSize; i++)
    {
        if (sameValueInSquare(contourColor, x, y, i) == false)
        {
            return false;
        }
    }
    return true;
}
//on each white pixel in contourColor; looks down and right to next white pixel, then from middle of those pixels finds how many black consecutive pixels can be found perpendicularly
//makes the average of first and second value, keeps the center of bigger average
void updateCenterSquareContourSurface()
{
    int averageResult = 0;
    //go to every pixel
    for (int x = 0; x < contourColor.cols; x++)
    {
        for (int y = 0; y < contourColor.rows; y++)
        {
            //check if pixel is white or if the point belong to top edge of left edge
            //since we only look down or right from a white pixel, we won't check anything if top left quarter is black,reason why stands second condition
            if (getPixelValue(contourColor, x, y) == 255 or x == 0 or y == 0)
            {
                //get number of consecutive black pixels, looking down and to right
                int down = NbOfBlackPixelsInARow(x, y, DOWN_SEARCH);
                int right = NbOfBlackPixelsInARow(x, y, RIGHT_SEARCH);
                //look perpendicularly from middle of those lines for number of consecutive black pixels
                int yMiddleDown = y + down / 2;
                int xMiddleRight = x + right / 2;
                int horizontalNbOfBlackDots = NbOfBlackPixelsInARow(x, yMiddleDown, LEFT_SEARCH) + NbOfBlackPixelsInARow(x, yMiddleDown, RIGHT_SEARCH);
                int verticalNbOfBlackDots = NbOfBlackPixelsInARow(xMiddleRight, y , UP_SEARCH) + NbOfBlackPixelsInARow(xMiddleRight, y, DOWN_SEARCH);
                //make average
                int averageToRight = ( right + verticalNbOfBlackDots ) / 2;
                int averageToDown = ( down + horizontalNbOfBlackDots ) / 2;
                //update if average bigger than previous one
                int arbitrarySquareSize = 20;
                if (averageToDown > averageResult and averageToDown >= averageToRight)
                {
                    int newX = x;
                    int newY = yMiddleDown;
                    //first prevent some errors due to narrow areas (want to fit a square, not a cross, but checked for a cross)
                    if (allSamePixelsInSquare(newX, newY, arbitrarySquareSize) == true)
                    {
                        averageResult = averageToDown;
                        //update coordinates of middle of flat surface found
                        xSquareContour1 = newX;
                        ySquareContour1 = newY;
                    }
                }
                if (averageToRight > averageResult and averageToRight >= averageToDown)
                {
                    int newX = xMiddleRight;
                    int newY = y;
                    //first prevent some errors due to narrow areas (want to fit a square, not a cross, but checked for a cross)
                    if (allSamePixelsInSquare(newX, newY, arbitrarySquareSize) == true)
                    {
                        averageResult = averageToRight;
                        //update coordinates of middle of flat surface found
                        xSquareContour1 = newX;
                        ySquareContour1 = newY;
                    }
                }
            }
        }
    }
    //if the value of averageResult did not change, then we never found an area where we could fit a square
    if (averageResult == 0)
    {
        //means couldnt find a flat area
        print(9999);
        xSquareContour1 = -1;
        ySquareContour1 = -1;
    }
}

//finds the biggest rectangle around a point
//returns the edgeSize
int biggestSquareCentered(Mat mat, int x, int y)
{
    int edgeSize = 1;
    bool fit = true;
    while (fit == true)
    {
        //check if all same value in square
        if (sameValueInSquare(mat, x, y, edgeSize) == false)
        {
            fit = false;
        }
        edgeSize = edgeSize + 1;
    }
    return edgeSize - 1;
}

//returns true if all pixel between two points are of same value
//points have to be on same column or same line
bool allSameValuesInLine(Mat mat, int x1, int y1, int x2, int y2)
{
    //check values given
    if (x1 > x2 or y1 > y2 or x1 < 0 or x2 > INPUT_WIDTH or y1 < 0 or y2 > INPUT_HEIGHT)
    {
        return false;
    }
    //check if 2 points on same line or command
    if (x1 != x2 and y1 != y2)
    {
        return false;
    }

    int pixelValue = getPixelValue(mat, x1, y1);
    //same line
    if (y1 == y2)
    {
        for ( int x = x1 ; x <= x2; x++)
        {
            if (getPixelValue(mat, x, y1) != pixelValue)
            {
                return false;
            }
        }
    }
    //same column
    if (x1 == x2)
    {
        for ( int y = y1 ; y <= y2; y++)
        {
            if (getPixelValue(mat, x1, y) != pixelValue)
            {
                return false;
            }
        }
    }
    return true;
}


//averageDepth contains white dots where possible flat surfaces are
//here we look for the biggest square that can fit in averageDepth with only white dots
void searchBiggestWhiteSquare()
{
    for (int x = 0; x < binaryDepth.cols; x++)
    {
        for (int y = 0; y < binaryDepth.rows; y++)
        {
            //look if pixel is white
            if (getPixelValue(binaryDepth, x, y) == 255)
            {
                //search for first size of square that won't fit
                int i = 1;
                while (i < binaryDepth.rows)
                {
                    //if there is a black dot in the square centered on (x,y) of size i
                    if (sameValueInSquare(binaryDepth, x, y, i) == false)
                    {
                        //take into account if square bigger than previous found
                        if (i-1 > squareEdge1)
                        {
                            xCenterOfSquare1 = x;
                            yCenterOfSquare1 = y;
                            squareEdge1 = i - 1; //fits when edge is i-1
                        }
                        //break from while loop
                        i = binaryDepth.rows;
                    }
                    i = i + 1;
                }
            }
        }
    }
}

//shows a square in the mat, for test purpose
void drawSquare(Mat mat, int x, int y, int i)
{
    int drawValue = 100;
    //left column
    int xSquare = x-i;
    for (int k=-i; k <= i; k++)
    {
        setPixelValue(mat, xSquare, y + k, drawValue);
    }
    //right column
    xSquare = x+i;
    for (int k=-i; k <= i; k++)
    {
        setPixelValue(mat, xSquare, y + k, drawValue);
    }
    //lower part of square left
    int ySquare = y+i;
    for (int k=-i+1; k <= i-1; k++)
    {
        setPixelValue(mat, x + k, ySquare, drawValue);
    }
    //higher part of square left
    ySquare = y-i;
    for (int k=-i+1; k <= i-1; k++)
    {
        setPixelValue(mat, x + k, ySquare, drawValue);
    }
}


//draw a rectangle by giving 2 points
//give top left first
void drawRectangle(Mat mat, int x1, int y1, int x2, int y2)
{
    int drawValue = 100;
    for (int y = y1; y <= y2; y ++)
    {
        setPixelValue(mat, x1, y, drawValue);
        setPixelValue(mat, x2, y, drawValue);
    }
    for (int x = x1; x <= x2; x ++)
    {
        setPixelValue(mat, x, y1, drawValue);
        setPixelValue(mat, x, y2, drawValue);
    }
}


void biggestSquareAround(Mat mat, int x, int y)
{
    //first make it grow every side at the same time
    //minus one to be sure not to get out of window
    int edge = biggestSquareCentered(mat, x, y) - 1;
    //transfer to two points of a rectangle
    int x1 = x - edge;
    int y1 = y - edge;
    int x2 = x + edge;
    int y2 = y + edge;

    //now grow two sides by two sides in order to remain a square
    //is true to enter the while loop
    bool alreadyGrown = true;

    while(alreadyGrown == true)
    {
        alreadyGrown = false;
        //check if can grow up
        if (allSameValuesInLine(mat, x1, y1-1, x2, y1-1) == true)
        {
            //check if can grow to the left
            if (allSameValuesInLine(mat, x1-1, y1-1, x1-1, y2) == true and alreadyGrown == false)
            {
                alreadyGrown = true;
                x1 = x1-1;
                y1 = y1-1;
            }
            //check if can grow to the right, if not been to left
            if (allSameValuesInLine(mat, x2+1, y1-1, x2+1, y2) == true and alreadyGrown == false)
            {
                alreadyGrown = true;
                x2 = x2+1;
                y1 = y1-1;
            }
        }
        //check if can grow down and that did not grow before
        if (allSameValuesInLine(mat, x1, y2+1, x2, y2+1) == true and alreadyGrown == false)
        {
            //check if can grow to the left
            if (allSameValuesInLine(mat, x1-1, y1, x1-1, y2+1) == true)
            {
                alreadyGrown = true;
                x1 = x1-1;
                y2 = y2+1;
            }
            //check if can grow to the right, if not been to left
            if (allSameValuesInLine(mat, x2+1, y1, x2+1, y2+1) == true and alreadyGrown == false)
            {
                alreadyGrown = true;
                x2 = x2+1;
                y2 = y2+1;
            }
        }
    }

    //drawRectangle(mat, x1, y1, x2, y2);

    //helpful values for next step
    int UP = 0;
    int RIGHT = 1;
    int DOWN = 2;
    int LEFT = 3;
    //now find a direction where we can grow the most into a rectangle
    int maxi = 0;
    int directionResult = -1;
    //4 directions
    for (int k = 0; k < 4; k++)
    {
        //what direction looking at
        if (k == UP)
        {
            bool fit = true;
            int i = 1;
            while (fit == true)
            {
                //search for blank spot in upper layer of pixel
                if (allSameValuesInLine(mat, x1, y1 - i, x2, y1 - i ) == false)
                {
                    //blank pixel found
                    fit = false;
                    //replace maxi if found higher
                    if (i - 1 > maxi)
                    {
                        maxi = i - 1;
                        directionResult = UP;
                    }
                }
                i = i + 1;
            }
        }
        if (k == RIGHT)
        {
            bool fit = true;
            int i = 1;
            while (fit == true)
            {
                //search for blank spot in right layer of pixel
                if (allSameValuesInLine(mat, x2 + i, y1, x2 + i, y2 ) == false)
                {
                    //blank pixel found
                    fit = false;
                    //replace maxi if found higher
                    if (i - 1 > maxi)
                    {
                        maxi = i - 1;
                        directionResult = RIGHT;
                    }
                }
                i = i + 1;
            }
        }
        if (k == DOWN)
        {
            bool fit = true;
            int i = 1;
            while (fit == true)
            {
                //search for blank spot in down layer of pixel
                if (allSameValuesInLine(mat, x1, y2 + i, x2, y2 + i ) == false)
                {
                    //blank pixel found
                    fit = false;
                    //replace maxi if found higher
                    if (i - 1 > maxi)
                    {
                        maxi = i - 1;
                        directionResult = DOWN;
                    }
                }
                i = i + 1;
            }
        }
        if (k == LEFT)
        {
            bool fit = true;
            int i = 1;
            while (fit == true)
            {
                //search for blank spot in left layer of pixel
                if (allSameValuesInLine(mat, x1 - i, y1, x1 - i, y2 ) == false)
                {
                    //blank pixel found
                    fit = false;
                    //replace maxi if found higher
                    if (i - 1 > maxi)
                    {
                        maxi = i - 1;
                        directionResult = LEFT;
                    }
                }
                i = i + 1;
            }
        }
        // update x1... where found biggest rectangle
        if (directionResult == UP)
        {
            y1 = y1 - maxi;
        }
        if (directionResult == DOWN)
        {
            y2 = y2 + maxi;
        }
        if (directionResult == LEFT)
        {
            x1 = x1 - maxi;
        }
        if (directionResult == RIGHT)
        {
            x2 = x2 + maxi;
        }
    }

    //draw rectangle
    drawRectangle(mat, x1, y1, x2, y2);
}
//
////////
//MAIN//
////////
int main(int argc, char *argv[])
{
    initializeCamera();
    setupWindows();
    while( loop )
    {
        depth = getCamDepthImage();
        color = getCamColorImage();
        //every said second, we perform those actions
        //includes the reset of the averageDepth and the look for a flat surface
        if (timer.secondsPast())
        {
            //BINARY AND DEPTH PART
            updateBinaryDepth();
            //searchBiggestWhiteSquare();
            //drawSquare(binaryDepth, xCenterOfSquare1, yCenterOfSquare1, squareEdge1);

            //CONTOUR AND COLOR PART
            updateContour();
            updateCenterSquareContourSurface();
            //drawSquare(contourColor, xSquareContour1, ySquareContour1, biggestSquareCentered(contourColor, xSquareContour1, ySquareContour1));
            biggestSquareAround(contourColor, xSquareContour1, ySquareContour1);

            //RESET AVERAGE PART
            resetMat(averageDepth);
            squareEdge1 = 0;
            increments = 0;
        }
        depth = getCamDepthImage();
        color = getCamColorImage();

        updateAverageDepth();

        imshow(WINDOW_DEPTH, depth);
        imshow(WINDOW_COLOR, color);

        imshow("binary", binaryDepth);
        //imshow("correction result", depthCorrectionResult);
        imshow("contours", contourColor);

        //needed for average mats
        increments = increments + 1;

        cvWaitKey(1);
    }
}
