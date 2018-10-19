#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{



/*------------------------------------------------
    helper function to parse commands
*/
    CommandLineParser parser(argc, argv,
        "{help h usage ?|       |show this message}"
        "{image_gray ig |       |(required) path to image}"

    );
    //if help
    if(parser.has("help"))
    {
        parser.printMessage();
        cerr<<"Use absolute path"<<endl;
        return 1;
    }
    // collecting data from parameters
    string image_gray_location(parser.get<string>("image_gray"));
    if(image_gray_location.empty()){
    cerr<<"check parameters, no location given" <<endl;
        parser.printMessage();
        return -1;
    }

    //create imagematrix
    Mat image;
    image = imread(image_gray_location);
    //image found?
    if(image.empty())
    {
        cerr<<"no image found, please try again with correct location"<<endl;
        return -1;
    }
    //show image
    imshow("foto", image);
    //wait till key is pressed
    waitKey(0);
    vector <Mat>channels;
    split(image,channels);
    //3 kanalen showen


    Mat image_gryscale
    cvtcolor(image,image_grayscale, COLOR_BGR2GRAY);
    imshow();

    for(int row = 0; row < image.rows;row++)
    {
        for(int col = 0;col < image.cols;col++)
        {
            int value = image.at<uchar>(row,col);
            cerr<<value<< " ";
            cerr << (int)image.at<uchar>(row,col) << " ";
        }
    }
    cerr<<endl;

    Mat canvas = Mat::zeros(Size(250));
    return 0;
}





