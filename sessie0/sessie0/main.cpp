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
        "{image_color ic|       |(required) path to color image}"

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
    cerr<<"check parameters, no gray location given" <<endl;
        parser.printMessage();
        return -1;
    }

    string image_color_location(parser.get<string>("image_color"));
    if(image_gray_location.empty()){
        cerr<<"check parameters, no color location given" <<endl;
        parser.printMessage();
        return -1;
    }

    //create imagematrix for gray image
    Mat grayimage;
    grayimage = imread(image_gray_location);
    //image found?
    if(grayimage.empty())
    {
        cerr<<"no gray image found, please try again with correct location"<<endl;
        return -1;
    }

    Mat colorimage;
    colorimage = imread(image_color_location);
    //image found?
    if(colorimage.empty())
    {
        cerr<<"no color image found, please try again with correct location"<<endl;
        return -1;
    }
    //show image
    imshow("foto", grayimage);
    //wait till key is pressed
    waitKey(0);
    imshow("foto2",colorimage);
    waitKey(0);
    vector <Mat>channels;
    split(colorimage,channels);
    //3 kanalen showen

    imshow("red", channels[2]);
    waitKey(0);
    imshow("green", channels[1]);
    waitKey(0);

    imshow("blue", channels[0]);
    waitKey(0);


    //foto in grijswaarden tonen
    Mat image_grayscale;
    cvtColor(colorimage,image_grayscale, COLOR_BGR2GRAY);
    imshow("grijswaarden", image_grayscale);
    waitKey(0);

    for(int row = 0; row < grayimage.rows;row++)
    {
        for(int col = 0;col < grayimage.cols;col++)
        {
            int value = grayimage.at<uchar>(row,col);
            cerr<<value<< " ";
            cerr << (int)colorimage.at<uchar>(row,col) << " ";
        }
    }
    cerr<<endl;
    //lege canvas maken
    Mat canvas = Mat::zeros(250, 250, CV_8UC3);
    //rechthoek tekenen
    rectangle(canvas, Point(50,50), Point(200,200), Scalar(255,0,0),1);
    circle(canvas, Point(125,125),50,Scalar(0,255,0), 2);
    imshow("Canvas", canvas);
    waitKey(0);
    return 0;
}





