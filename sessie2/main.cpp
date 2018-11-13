#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void callback(int, void*)
{

}
int main(int argc, const char** argv) {
    /**
    *     helper function to parse commands
    */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message}"
                             "{@image|  | (required) path to image}"
                             "{segBGR segmentBGR | | opdracht1:segmenteer in BGR}"
                             "{segHSV segmentHSV | | opdracht2:segmenteer in HSV}"
                             "{clean c | | opdracht3:segmenteer in hsv, dan proper maken}"
                             "{sliders| |pas de waarden van de threshold aan door sliders}"

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
    if(parser.has("segmentBGR"))
    {
        namedWindow("original image", WINDOW_AUTOSIZE);
        imshow("original image", image);
        waitKey(0);
        int B_LOW=0;
        int B_HIGH = 255;
        int G_LOW = 0;
        int G_HIGH = 255;
        int R_LOW = 150;
        int R_HIGH = 255;
        vector<Mat>channels;
        split(image, channels);
        Mat B = channels[0];
        Mat G = channels[1];
        Mat R = channels[2];
        Mat output;
        Mat B_th,G_th,R_th;
        //threshold(Rood, rood_th, 150, 255, THRESH_BINARY);
        //alternatief Mat Rood_th = Rood>150
        //alt2 inRange(Rood,150,255,Rood_Th)
        inRange(R,R_LOW,R_HIGH,R_th);
        Mat finaal(image.rows, image.cols, CV_8UC3);
        Mat blauw = channels[0] & R_th;
        Mat groen = channels[1] & R_th;
        Mat rood = channels[2] & R_th;

        Mat in[] = {blauw,groen,rood};
        int from_to[] = {0,0,1,1,2,2};
        mixChannels(in,3,&finaal,1,from_to,3);

        namedWindow("after segmentation", WINDOW_AUTOSIZE);
        imshow("after segmentation skin", finaal);
        waitKey(0);



        //CVTcolor
        //COLORBGR2HSV
        //INRANGE

    }
    if(parser.has("segmentHSV"))
    {
        namedWindow("original image", WINDOW_AUTOSIZE);
        imshow("original image", image);
        waitKey(0);
        int H_LOW1=0;
        int H_HIGH1 = 5;
        int S_LOW1 = 115;
        int S_HIGH1 = 255;
        int V_LOW1 = 145;
        int V_HIGH1 = 255;
        int H_LOW2=165;
        int H_HIGH2 = 180;
        int S_LOW2 = 115;
        int S_HIGH2 = 255;
        int V_LOW2 = 115;
        int V_HIGH2 = 255;
        Mat output(image.rows, image.cols, CV_8UC3);
        cvtColor(image, output, COLOR_BGR2HSV);
        namedWindow("converion2HSV", WINDOW_AUTOSIZE);
        imshow("conversion2HSV", output);
        waitKey(0);
        Mat threshold,threshold2, thresholdout;
        inRange(output, Scalar(H_LOW1,S_LOW1,V_LOW1), Scalar(H_HIGH1,S_HIGH1,V_HIGH1), threshold);
        inRange(output, Scalar(H_LOW2,S_LOW2,V_LOW2), Scalar(H_HIGH2,S_HIGH2,V_HIGH2), threshold2);
        addWeighted(threshold, 1.0, threshold2,1.0,0.0, thresholdout);

        namedWindow("HSVTHRESH", WINDOW_AUTOSIZE);
        imshow("HSVTHRESH", thresholdout);
        waitKey(0);

        //Mat finaal(image.rows,image.cols,CV_8UC3);
        Mat finaal = Mat::zeros(image.rows, image.cols, CV_8UC3);
        image.copyTo(finaal,thresholdout);

        namedWindow("HSVoutput", WINDOW_AUTOSIZE);
        imshow("HSVoutput", finaal);
        waitKey(0);
    }
    if(parser.has("clean"))
    {
        namedWindow("original image", WINDOW_AUTOSIZE);
        imshow("original image", image);
        waitKey(0);
        int H_LOW1=0;
        int H_HIGH1 = 5;
        int S_LOW1 = 115;
        int S_HIGH1 = 255;
        int V_LOW1 = 145;
        int V_HIGH1 = 255;
        int H_LOW2=165;
        int H_HIGH2 = 180;
        int S_LOW2 = 115;
        int S_HIGH2 = 255;
        int V_LOW2 = 115;
        int V_HIGH2 = 255;
        Mat output(image.rows, image.cols, CV_8UC3);
        cvtColor(image, output, COLOR_BGR2HSV);
        namedWindow("converion2HSV", WINDOW_AUTOSIZE);
        imshow("conversion2HSV", output);
        waitKey(0);
        Mat threshold,threshold2, thresholdout;
        inRange(output, Scalar(H_LOW1,S_LOW1,V_LOW1), Scalar(H_HIGH1,S_HIGH1,V_HIGH1), threshold);
        inRange(output, Scalar(H_LOW2,S_LOW2,V_LOW2), Scalar(H_HIGH2,S_HIGH2,V_HIGH2), threshold2);
        addWeighted(threshold, 1.0, threshold2,1.0,0.0, thresholdout);

        namedWindow("HSVTHRESH", WINDOW_AUTOSIZE);
        imshow("HSVTHRESH", thresholdout);
        waitKey(0);

        //Mat finaal(image.rows,image.cols,CV_8UC3);
        Mat finaal = Mat::zeros(image.rows, image.cols, CV_8UC3);
        image.copyTo(finaal,thresholdout);

        namedWindow("HSVoutput", WINDOW_AUTOSIZE);
        imshow("HSVoutput", finaal);
        waitKey(0);

        dilate(thresholdout, thresholdout, Mat(),Point(-1,1) ,5);
        erode(thresholdout, thresholdout, Mat(),Point(-1,1) ,5);

        namedWindow("cleaned up mask", WINDOW_AUTOSIZE);
        imshow("cleaned up mask", thresholdout);
        waitKey(0);

        vector<vector<Point>> contouren;
        findContours(thresholdout.clone(), contouren, RETR_EXTERNAL,CHAIN_APPROX_NONE);
        vector <vector <Point>> hulls;
        for(size_t i=0;i<contouren.size();i++)
        {
            vector<Point> hull;
            convexHull(contouren[i],hull);
            hulls.push_back(hull);
        }
        drawContours(thresholdout,hulls, -1,255,-1);
        namedWindow("masker met hulls", WINDOW_AUTOSIZE);
        imshow("masker met hulls", thresholdout);
        waitKey(0);

        finaal = Mat::zeros(image.rows, image.cols, CV_8UC3);
        image.copyTo(finaal,thresholdout);
        namedWindow("verkeersbord", WINDOW_AUTOSIZE);
        imshow("verkeersbord", finaal);
        waitKey(0);

    }
    if(parser.has("sliders"))
    {
        int hueValL, hueValH, satValL, satValH, valValL, valValH;
        namedWindow("Sliders");
        hueValL = 34;
        hueValH = 159;
        satValL = 17;
        satValH = 92;
        valValL = 127;
        valValH = 181;
        createTrackbar("HUE-LOW", "Sliders",&hueValL, 180, callback);
        createTrackbar("HUE-HIGH", "Sliders",&hueValH, 180, callback);
        createTrackbar("SAT-LOW", "Sliders",&satValL, 255, callback);
        createTrackbar("SAT-HIGH", "Sliders",&satValH, 255, callback);
        createTrackbar("VAL-LOW", "Sliders",&valValL, 250, callback);
        createTrackbar("VAL-HIGH", "Sliders",&valValH, 250, callback);

        Mat output(image.rows, image.cols, CV_8UC3);
        cvtColor(image, output, COLOR_BGR2HSV);
        while(true)
        {
            Mat threshold;
            inRange(output, Scalar(hueValL,satValL,valValL),Scalar(hueValH,satValH,valValH),threshold);
            imshow("sliders", threshold);

            Mat finaal = Mat::zeros(image.rows, image.cols, CV_8UC3);
            image.copyTo(finaal,threshold);
            dilate(threshold, threshold, Mat(),Point(-1,1) ,5);
            erode(threshold, threshold, Mat(),Point(-1,1) ,5);

            vector<vector<Point>> contouren;
            findContours(threshold.clone(), contouren, RETR_EXTERNAL,CHAIN_APPROX_NONE);
            vector <vector <Point>> hulls;
            for(size_t i=0;i<contouren.size();i++)
            {
                vector<Point> hull;
                convexHull(contouren[i],hull);
                hulls.push_back(hull);
            }
            drawContours(threshold,hulls, -1,255,-1);
            finaal = Mat::zeros(image.rows, image.cols, CV_8UC3);
            image.copyTo(finaal,threshold);

            namedWindow("verkeersbord", WINDOW_AUTOSIZE);
            imshow("verkeersbord", finaal);



            char key = (char) waitKey(1);
            if (key == 'q' || key == 27)
            {
                break;
            }
        }
    }
    return 0;
}

