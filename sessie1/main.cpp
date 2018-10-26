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
                             "{help h usage ?|       | show this message}"
                             "{@image|  | (required) path to image}"
                             "{thresholding t |       | thresholding part(skin filtering)}"
                             "{bimodal b|   | thresholding part(scanned document)}"
                             "{clean c| |use erosion and dilation to clean up the image}"
    );
    //if help
    if(parser.has("help"))
    {
        parser.printMessage();
        cerr<<"Use absolute path"<<endl;
        return 1;
    }
    //check image location
    string image_path(parser.get<string>("@image"));
    if(image_path.empty())
    {
        cerr<<"no image location given"<<endl;
        parser.printMessage();
        return -1;
    }
    //read in image
    Mat image = imread(image_path);
    if(image.empty())
    {
        cerr<<"could not read image"<<endl;
        parser.printMessage();
        return -1;
    }
    if(parser.has("thresholding"))
    {
        vector<Mat>channels;
        split(image, channels);
        Mat B = channels[0];
        Mat G = channels[1];
        Mat R = channels[2];

        Mat mask_1 = Mat::zeros(image.rows,image.cols, CV_8UC1);
        Mat mask_2 = mask_1.clone();
        for(int row=0;row<image.rows;row++)
        {
            for(int col=0;col<image.cols;col++)
            {
                if(
                        (R.at<uchar>(row,col)>95) && (G.at<uchar>(row,col)>40) && (B.at<uchar>(row,col)>20) &&
                        ((max(R.at<uchar>(row,col),max(G.at<uchar>(row,col),B.at<uchar>(row,col))) -
                        min(R.at<uchar>(row,col), min(G.at<uchar>(row,col),B.at<uchar>(row,col))))>15) &&
                        (abs(R.at<uchar>(row,col)-G.at<uchar>(row,col))>15) && (R.at<uchar>(row,col)>G.at<uchar>(row,col)) && (R.at<uchar>(row,col)>B.at<uchar>(row,col)))
                {
                    //cerr<<"true"<<endl;
                    mask_1.at<uchar>(row,col)= 255;
                }
                else
                {
                    //cerr<<"false"<<endl;
                }
            }
        }
        namedWindow("masker met for loop", WINDOW_AUTOSIZE);
        imshow("masker met for loop", mask_1);
        waitKey(0);

        mask_2 = (R>95) & (G>40) & (B>20) & ((max(R, max(G,B)) - min(R,min(G,B)))>15) & (abs(R-G)>15) & (R>G) & (R>B);
        mask_2 = mask_2*255;
        namedWindow("masker met matrix operaties", WINDOW_AUTOSIZE);
        imshow("masker met matrix operaties", mask_2);
        waitKey(0);


        Mat finaal(image.rows, image.cols, CV_8UC3);
        Mat blauw = channels[0] & mask_2;
        Mat groen = channels[1] & mask_2;
        Mat rood = channels[2] & mask_2;

        Mat in[] = {blauw,groen,rood};
        int from_to[] = {0,0,1,1,2,2};
        mixChannels(in,3,&finaal,1,from_to,3);

        namedWindow("segmented skin", WINDOW_AUTOSIZE);
        imshow("segmented skin", finaal);
        waitKey(0);


    }
    else if(parser.has("bimodal"))
    {
        Mat gray_image;
        cvtColor(image,gray_image,COLOR_BGR2GRAY);
        namedWindow("zwartwit kassaticket", WINDOW_AUTOSIZE);
        imshow("zwartwit kassaticket", gray_image);
        waitKey(0);

        Mat OTSU;
        threshold(gray_image, OTSU, 0 , 255, THRESH_OTSU | THRESH_BINARY);
        namedWindow("OTSU kassaticket", WINDOW_AUTOSIZE);
        imshow("OTSU kassaticket", OTSU);
        waitKey(0);

        Mat equal;
        equalizeHist(gray_image.clone(),equal);
        namedWindow("Equalised kassaticket", WINDOW_AUTOSIZE);
        imshow("Equialised kassaticket", equal);
        waitKey(0);

    }
    return 0;
}