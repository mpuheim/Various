// Michal Puheim 2012

//Configuration
#define WND_NAME            "Square Detection"  //name of output window
#define WEBCAM_ID           0                   //webcam id
#define AD_THRESH_CONST     8                   //constant subtracted from the mean in adaptive thresholding
#define AD_THRESH_BLOCK     13                  //pixel neighborhood size used in adaptive thresholding
#define CANNY_THRESH_LOW    50                  //first threshold level in canny algorithm
#define CANNY_THRESH_HIGH   200                 //second threshold level in canny algorithm
#define CANNY_THRESH_APP    3                   //aperture size for the Sobel() operator in canny algorithm
#define SQUARE_AREA         400                 //minimum size of square area
#define SQUARE_TOLERANCE    6                   //tolerance to different side lengths of square
#define FILTERING_WINDOW    10                  //threshold used to filter out duplicitly detected squares
