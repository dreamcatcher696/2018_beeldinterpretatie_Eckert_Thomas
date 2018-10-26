#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
   /**
    *     helper function to parse commands
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
    /**
     *  The following part executes hen the thresholding or t parameter is given to the function.
     *  Requires: Imagecolor.png (https://github.com/EAVISE/2018_labo_beeldinterpretatie/tree/master/sessie_1/Imagecolor.jpg)
     */
    if(parser.has("thresholding"))
    {
        vector<Mat>channels;
        split(image, channels);
        Mat B = channels[0];
        Mat G = channels[1];
        Mat R = channels[2];

        //create 2 masks, same size as input image
        Mat mask_1 = Mat::zeros(image.rows,image.cols, CV_8UC1);
        Mat mask_2 = mask_1.clone();
        //use the long, difficult and incredibly annoying way of implementing the filter
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
                    //if all the above is true, set the mask at that pixel to true (or255 in this case)
                    //cerr<<"true"<<endl;
                    mask_1.at<uchar>(row,col)= 255;
                }
                else
                {
                    //do nothing
                    //cerr<<"false"<<endl;
                }
            }
        }
        //show the mask we created
        namedWindow("masker met for loop", WINDOW_AUTOSIZE);
        imshow("masker met for loop", mask_1);
        waitKey(0);

        //more elegant way of implementing, using matrix operations
        mask_2 = (R>95) & (G>40) & (B>20) & ((max(R, max(G,B)) - min(R,min(G,B)))>15) & (abs(R-G)>15) & (R>G) & (R>B);
        mask_2 = mask_2*255;    //the matrix only sets the bit to 1, zo we need to multiply with 255
        //show mask with matrix operations, naturally this is the same result as the previous window
        namedWindow("masker met matrix operaties", WINDOW_AUTOSIZE);
        imshow("masker met matrix operaties", mask_2);
        waitKey(0);

        //stitch the mask to the original, so we can see the colors
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
    /**
     *  This function tests the OTSU and CHANE part to see which one performes best. both are types of thresholding, but the way we used in Thresholding is not accurate
     *  Requires: ImageBimodal.png (https://github.com/EAVISE/2018_labo_beeldinterpretatie/tree/master/sessie_1/ImageBimodal.jpg)
     */
    else if(parser.has("bimodal"))
    {
        //convert the image to gray, because OTSU expects gray image
        Mat gray_image;
        cvtColor(image,gray_image,COLOR_BGR2GRAY);
        namedWindow("zwartwit kassaticket", WINDOW_AUTOSIZE);
        imshow("zwartwit kassaticket", gray_image);
        waitKey(0);

        //apply OTSU
        Mat OTSU;
        threshold(gray_image, OTSU, 0 , 255, THRESH_OTSU | THRESH_BINARY);
        namedWindow("OTSU kassaticket", WINDOW_AUTOSIZE);
        imshow("OTSU kassaticket", OTSU);
        waitKey(0);

        //use histogram equilisation
        Mat equal;
        equalizeHist(gray_image.clone(),equal);
        namedWindow("Equalised kassaticket", WINDOW_AUTOSIZE);
        imshow("Equialised kassaticket", equal);
        waitKey(0);

        //use CLAHE
        Mat CLAHEMAT;
        Ptr<CLAHE> claheptr = createCLAHE();
        claheptr->setTilesGridSize(Size(15,15));
        claheptr->setClipLimit(1);
        claheptr->apply(gray_image.clone(), CLAHEMAT);
        namedWindow("CLAHE kassaticket", WINDOW_AUTOSIZE);
        imshow("CLAHE kassaticket", CLAHEMAT);
        waitKey(0);
    }
    /**
     * 1.2; Erosion and dilation, how can we 'clean up' the input image, and find the features of the image
     *  Requires: ImagecolorAdapted.png (https://github.com/EAVISE/2018_labo_beeldinterpretatie/tree/master/sessie_1/ImagecolorAdapted.jpg)
     */
    else if(parser.has("clean"))
    {
        //split the imput image in 3 channels to filter the same way we did in Thresholding
        vector<Mat>channels;
        split(image, channels);
        Mat B = channels[0];
        Mat G = channels[1];
        Mat R = channels[2];

        Mat mask = Mat::zeros(image.rows,image.cols, CV_8UC1);
        mask = (R>95) & (G>40) & (B>20) & ((max(R, max(G,B)) - min(R,min(G,B)))>15) & (abs(R-G)>15) & (R>G) & (R>B);
        mask = mask*255;
        namedWindow("masker", WINDOW_AUTOSIZE);
        imshow("masker", mask);
        waitKey(0);
        //filter out the small dots
        erode(mask,mask,Mat(), Point(-1,-1), 2);
        dilate(mask,mask, Mat(), Point(-1,-1), 2);

        namedWindow("masker na eerste erode+dilate", WINDOW_AUTOSIZE);
        imshow("masker na eerste erode+dilate", mask);
        waitKey(0);
        //find big blobs
        dilate(mask,mask, Mat(), Point(-1,-1), 5);
        erode(mask,mask,Mat(), Point(-1,-1), 5);

        namedWindow("masker na dilate+erode", WINDOW_AUTOSIZE);
        imshow("masker na dilate+erode", mask);
        waitKey(0);

        //find outer contours
        vector< vector <Point>> contouren;
        findContours(mask.clone(), contouren, RETR_EXTERNAL, CHAIN_APPROX_NONE);
        vector <vector <Point>> hulls;
        for(size_t i=0;i<contouren.size();i++)
        {
            vector<Point> hull;
            convexHull(contouren[i],hull);
            hulls.push_back(hull);
        }
        //color in the hulls by using -1 as thickness
        drawContours(mask,hulls, -1,255,-1);
        namedWindow("masker met hulls", WINDOW_AUTOSIZE);
        imshow("masker met hulls", mask);
        waitKey(0);

        //yet again, apply the mask on the original image to see the colors
        Mat finaal(image.rows, image.cols, CV_8UC3);
        Mat blauw=channels[0]&mask;
        Mat groen = channels[1]&mask;
        Mat rood = channels[2]&mask;

        Mat in[] = {blauw, groen, rood};
        int from_to[] = {0,0,1,1,2,2};
        mixChannels(in, 3, &finaal, 1, from_to, 3);

        namedWindow("resultaat", WINDOW_AUTOSIZE);
        imshow("resultaat", finaal);
        waitKey(0);
    }
    return 0;
}