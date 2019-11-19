// Square detector
// Michal Puheim 2012
//
// Compile by using command:
// g++ `pkg-config opencv --cflags` `pkg-config opencv --libs` square_detect.cpp -o square_detect


//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//cpp libraries
#include <iostream>
//other headers
#include "config.h"

using namespace cv;
using namespace std;

// draw all the squares in the image
void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
	    //draw borders
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 2, CV_AA);
        //draw crosslines
        line(image, p[0], p[2], cvScalar(0, 255, 0), 2);
	    line(image, p[1], p[3], cvScalar(0, 255, 0), 2);
        //draw corners
        circle(image, p[0], 2, cvScalar(255, 0, 0), 2);
	    circle(image, p[1], 2, cvScalar(255, 0, 0), 2);
	    circle(image, p[2], 2, cvScalar(255, 0, 0), 2);
	    circle(image, p[3], 2, cvScalar(255, 0, 0), 2);
    }

    imshow(WND_NAME, image);
}

// find a cosine of angle between point vectors
// from pt0->pt1 and from pt0->pt2
double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// find distance between 2 points
double ptDistance(Point p1, Point p2)
{
    return sqrt((double)((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y)));
}

// test if contour is a square
// square contours should have:
//     - 4 vertices after approximation,
//     - relatively large area (to filter out noisy contours)
//     - and be convex.
// note: absolute value of an area is used because area may be positive or negative
//       in accordance with the contour orientation
bool squareTest(const vector<Point>& contour)
{
    if( contour.size() == 4 &&
        fabs(contourArea(Mat(contour))) > SQUARE_AREA &&
        isContourConvex(Mat(contour)) )
    {
        // find the maximum cosine of the angle between joint edges
        double maxCosine = 0;
        for( int i = 2; i < 5; i++ )
        {
            double cosine = fabs(angle(contour[i%4], contour[i-2], contour[i-1]));
            maxCosine = MAX(maxCosine, cosine);
        }
        // if cosines of all angles are small
        // (all angles are ~90 degree) then contour is square
        if( maxCosine > 0.3 )
            return false;
        // decide if all sides of square have similar size
        double compare = ptDistance(contour[1],contour[0]);
        for( int i = 2; i < 5; i++ )
        {
            double dst = ptDistance(contour[i%4],contour[i-1]);
            if (fabs(compare-dst) > SQUARE_TOLERANCE)
                return false;
        }
        //if all conditions are met, then the contour is a square
        return true;
    }
    return false;
}

// get center coordinates of square
Point getSquareCenter(const vector<Point>& squares)
{
    Point center;
    int size = (int)squares.size();
    for (int i = 0; i < size; i++)
    {
        center.x += squares[i].x;
        center.y += squares[i].y;
    }
    center.x = center.x/size;
    center.y = center.y/size;
    return center;
}

// filtering of duplicit squares
void filterDuplicitSquares( vector<vector<Point> >& squares )
{
    for (int i = 0; i < (int)squares.size()-1; i++)
    {
        for (int j = i+1; j < (int)squares.size(); j++)
        {
            if (fabs((double)(getSquareCenter(squares[i]).x - getSquareCenter(squares[j]).x)) < FILTERING_WINDOW)
            {
                if (fabs((double)(getSquareCenter(squares[i]).y - getSquareCenter(squares[j]).y)) < FILTERING_WINDOW)
                {
                    squares.erase(squares.begin() + j);
                    j = j-1;
                }
            }
        }
    }
}

// return sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    //clear square vector
    squares.clear();
    //convert image to grayscale
    Mat gray;
    cvtColor(image, gray, CV_RGB2GRAY);
    //blur image
    Mat blurred;
    blur(gray, blurred, Size(3,3), Point(-1,-1), BORDER_DEFAULT);
    //find contours:
    Mat binary;
    //1. using canny edge detector
    vector<vector<Point> > contours1;
    Canny(blurred, binary, CANNY_THRESH_LOW, CANNY_THRESH_HIGH, CANNY_THRESH_APP);
    dilate(binary, binary, Mat(), Point(-1,-1),3); //Mat r;resize(binary, r, Size(), 0.5, 0.5);imshow("dbg_canny", r);
    findContours(binary, contours1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    //2. using adaptive thresholding
    vector<vector<Point> > contours2;
    adaptiveThreshold(blurred, binary, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, AD_THRESH_BLOCK, AD_THRESH_CONST);
    dilate(binary, binary, Mat(), Point(-1,-1),2); //resize(binary, r, Size(), 0.5, 0.5);imshow("dbg_AT", r);
    findContours(binary, contours2, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    //merge results
    vector<vector<Point> > contours;
    contours.insert(contours.end(), contours1.begin(), contours1.end());
    contours.insert(contours.end(), contours2.begin(), contours2.end());
    //test each contour
    vector<Point> approx;
    for( size_t i = 0; i < contours.size(); i++ )
    {
        // approximate contour with accuracy proportional to the contour perimeter
        approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.1, true);
        // test if contour is a square
        if (squareTest(approx) == true)
            squares.push_back(approx);
    }
    //filter duplicit squares (eq. when found both by canny and adaptive thresholding approaches)
    filterDuplicitSquares(squares);
}

// initialize camera
VideoCapture initializeCamera()
{
    //Object for video capturing from video files or cameras
    VideoCapture cap;
    //Start streaming from the camera specified by its id
    cap.open(WEBCAM_ID);
    //Continue only if the camera has been initialized succesfully
    if( !cap.isOpened() )
    {
        cout << "Error. Could not initialize camera...\n";
        exit(-1);
    }
    return cap;
}

int main(int /*argc*/, char** /*argv*/)
{
    namedWindow(WND_NAME,1);
    vector<vector<Point> > squares;
    VideoCapture cap = initializeCamera();
    Mat image;
    
    while(1)
    {
        //capture image
        cap >> image;
        if( image.empty() )
        {
            cout << "Couldn't capture image " << endl;
            continue;
        }
        //find and draw squares
        findSquares(image, squares);
        drawSquares(image, squares);
        //exit if escape is pressed
        int c = waitKey(60);
        if((char)c==27 )
            return 0;
    }

    return 0;
}
