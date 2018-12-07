#include <iostream>
#include <opencv2/opencv.hpp>
#include <string.h>


using namespace std;
using namespace cv;
int main(int argc, const char** argv) {
    /**
     * @brief function that prints the help message, and parses the user input
     */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message \n\t\t left click:add point \n\t\t right click:remove last point \n\t\t middle mouse button print list}"
                             "{@input | | Required: (absolute) path to input video}"
                             "{face f| | do the face part of the course}"
                             "{person p| | do the person part of the course}"


    );
    //if help
    if (parser.has("help")) {
        parser.printMessage();
        cerr << "Use absolute path" << endl;
        return 1;
    }

    //check input location
    string input_path(parser.get<string>("@input"));
    if (input_path.empty()) {
        cerr << "no input location given" << endl;
        parser.printMessage();
        return -1;
    }
    if(parser.has("face"))
    {
        CascadeClassifier haar_cascade = CascadeClassifier("/Users/thomaseckert/Dropbox/PBEII/5de_jaar_(master)/2018_beeldinterpretatie_Eckert_Thomas/eaviseOriginal/sessie_6/haarcascade_frontalface_alt.xml");
        CascadeClassifier lbp_cascade = CascadeClassifier("/Users/thomaseckert/Dropbox/PBEII/5de_jaar_(master)/2018_beeldinterpretatie_Eckert_Thomas/eaviseOriginal/sessie_6/lbpcascade_frontalface_improved.xml");

        vector<Rect> gezichtenHaar, gezichtenLBP;
        vector<int>scoresHaar,scoresLBP;

        VideoCapture input = VideoCapture(input_path);
        while(input.isOpened())
        {
            Mat frame, frame_gray;
            input.read(frame);
            if(frame.empty())
            {
                cerr<< "error reading frame"<<endl;
                break;
            }
            cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
            haar_cascade.detectMultiScale(frame_gray, gezichtenHaar, scoresHaar, 1.05,3);
            lbp_cascade.detectMultiScale(frame_gray,gezichtenLBP,scoresLBP,1.05,3);
            Mat frame_bgr(frame_gray.size(), CV_8UC3);
            cvtColor(frame_gray, frame_bgr, CV_GRAY2BGR);
            for(int i=0;i<gezichtenHaar.size();i++)
            {
                int a = scoresHaar.at(i);
                stringstream ss;
                ss<<a;
                string str = ss.str();

                rectangle(frame_bgr, Point(gezichtenHaar.at(i).x,gezichtenHaar.at(i).y), Point(gezichtenHaar.at(i).x+gezichtenHaar.at(i).width,gezichtenHaar.at(i).y+gezichtenHaar.at(i).height),Scalar(0,0,255));

                putText(frame_bgr,str,Point(gezichtenHaar.at(i).x,gezichtenHaar.at(i).y),FONT_HERSHEY_COMPLEX, 1,Scalar(0,0,255) ,1);

            }
            for(int i=0;i<gezichtenLBP.size();i++)
            {
                int b = scoresLBP.at(i);
                stringstream ss;
                ss << b;
                string str2 = ss.str();
                rectangle(frame_bgr, Point(gezichtenLBP.at(i).x,gezichtenLBP.at(i).y), Point(gezichtenLBP.at(i).x+gezichtenLBP.at(i).width,gezichtenLBP.at(i).y+gezichtenLBP.at(i).height),Scalar(0,255,0));
                putText(frame_bgr,str2,Point(gezichtenLBP.at(i).x,gezichtenLBP.at(i).y),FONT_HERSHEY_COMPLEX, 1,Scalar(0,255,0) ,1);

            }

            imshow("frame", frame_bgr);
            /*if(waitKey(10)  == 'q')
            {
                break;
            }*/
            waitKey(1);


        }
        input.release();
        destroyAllWindows();
    }
    if(parser.has("person"))
    {
        VideoCapture input = VideoCapture(input_path);
        HOGDescriptor hog;
        hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

        vector<Rect> person;
        vector<double>weight;
        vector<Point>tracking;
        while(input.isOpened())
        {

            Mat frame, frame_gray;
            input.read(frame);
            resize(frame, frame, Size(frame.cols*2,frame.rows*2));
            if(frame.empty())
            {
                cerr<< "error reading frame"<<endl;
                break;
            }
            cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

            hog.detectMultiScale(frame, person,weight);

            for(int i=0;i<person.size();i++)
            {
                Rect r = person[i];
                rectangle(frame, r, Scalar(255,0,0));
                stringstream ss;
                ss << weight[i];
                putText(frame,ss.str(),Point(r.x,r.y+50),FONT_HERSHEY_SIMPLEX, 1,Scalar(255,0,0));
                tracking.push_back(Point(r.x+r.width/2,r.y+r.height/2));
            }
            for(int j=0;j<tracking.size();j++)
            {
                line(frame, tracking[j-1], tracking[j], Scalar(255,255,255), 2);
            }


            imshow("frame", frame);
            /*if(waitKey(10)  == 'q')
            {
                break;
            }*/
            waitKey(1);


        }
        input.release();
        destroyAllWindows();
    }






    return 0;
}