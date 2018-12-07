#include <iostream>
#include <opencv2/opencv.hpp>



using namespace std;
using namespace cv;
int main(int argc, const char** argv) {
    /**
     * @brief function that prints the help message, and parses the user input
     */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message \n\t\t left click:add point \n\t\t right click:remove last point \n\t\t middle mouse button print list}"
                             "{@input | | Required: (absolute) path to input video}"
                             "{@haarcascade||Required: (absolute) path to Haar cascade XML}"
                             "{@lbpcascade||Required: (absolute) path to lbp cascade XML}"
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
    //check haarcascade location
    String haar_path(parser.get<string>("@haarcascade"));
    if(haar_path.empty())
    {
        cerr << "no haar cascade location given; aborting" <<endl;
        parser.printMessage();
        return -2;
    }
    //check lbpcascade location
    String lbp_path(parser.get<string>("@lbpcascade"));
    if(lbp_path.empty())
    {
        cerr << "no lbp cascade location given; aborting" <<endl;
        parser.printMessage();
        return -3;
    }
    //run the face part of the assignment
    if(parser.has("face"))
    {
        CascadeClassifier haar_cascade = CascadeClassifier(haar_path);  //create the 2 classifiers
        CascadeClassifier lbp_cascade = CascadeClassifier(lbp_path);

        vector<Rect> gezichtenHaar, gezichtenLBP;           //create a vector to store the faces in (bounding box)
        vector<int>scoresHaar,scoresLBP;                    //create a vector with the scores

        VideoCapture input = VideoCapture(input_path);      //create a videocapture to read in video file
        while(input.isOpened())                             //go through whole video
        {
            Mat frame, frame_gray;                          //create MAtrix for input frame, and converted gray frame (the detectors requires gray)
            input.read(frame);                              //read in frame
            if(frame.empty())                               //test if valid frame
            {
                cerr<< "error reading frame"<<endl;
                break;
            }
            cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
            haar_cascade.detectMultiScale(frame_gray, gezichtenHaar, scoresHaar, 1.05,3);   //detect face with cascade
            lbp_cascade.detectMultiScale(frame_gray,gezichtenLBP,scoresLBP,1.05,3);         //detect face with lbp
            for(unsigned long i=0;i<gezichtenHaar.size();i++)     //for every face in the frame, draw bounding box and score
            {
                Rect r = gezichtenHaar[i];
                int a = scoresHaar.at(i);               //sketchy way of converting int to string
                stringstream ss;
                ss<<a;
                string str = ss.str();
                ellipse(frame, Point(r.x+r.width/2,r.y+r.height/2),Size(r.width/2,r.height/2),0,0,360,Scalar(0,0,255),2);
                //rectangle(frame, Point(gezichtenHaar.at(i).x,gezichtenHaar.at(i).y), Point(gezichtenHaar.at(i).x+gezichtenHaar.at(i).width,gezichtenHaar.at(i).y+gezichtenHaar.at(i).height),Scalar(0,0,255));
                putText(frame,str,Point(gezichtenHaar.at(i).x,gezichtenHaar.at(i).y),FONT_HERSHEY_COMPLEX, 1,Scalar(0,0,255) ,1);
            }
            for(unsigned long i=0;i<gezichtenLBP.size();i++)      //same as above
            {
                Rect r = gezichtenLBP[i];
                int b = scoresLBP.at(i);
                stringstream ss;
                ss << b;
                string str2 = ss.str();
                ellipse(frame, Point(r.x+r.width/2,r.y+r.height/2),Size(r.width/2,r.height/2),0,0,360,Scalar(0,255,0),2);
                //rectangle(frame, Point(gezichtenLBP.at(i).x,gezichtenLBP.at(i).y), Point(gezichtenLBP.at(i).x+gezichtenLBP.at(i).width,gezichtenLBP.at(i).y+gezichtenLBP.at(i).height),Scalar(0,255,0));
                putText(frame,str2,Point(gezichtenLBP.at(i).x,gezichtenLBP.at(i).y+gezichtenLBP.at(i).height+25),FONT_HERSHEY_SIMPLEX, 1,Scalar(0,255,0) ,1);

            }

            imshow("frame", frame);     //show the frame
            auto c=(char)waitKey(25);
            if(c==27 or c=='q') break;

        }
        input.release();
        destroyAllWindows();
    }
    //the person part of the assignment
    if(parser.has("person"))
    {
        VideoCapture input = VideoCapture(input_path);
        HOGDescriptor hog;              //create hog descriptor
        hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());  //make sure we are detecting persons

        vector<Rect> person;        //vector to store results in
        vector<double>weight;       //score
        vector<Point>tracking;      //for the tracking line
        while(input.isOpened())
        {
            Mat frame;
            input.read(frame);
            resize(frame, frame, Size(frame.cols*2,frame.rows*2));      //resize to start detection from frame 1
            if(frame.empty())       //check frame validity
            {
                cerr<< "error reading frame"<<endl;
                break;
            }
            //detect a person in the frame, store in perosn and weight
            hog.detectMultiScale(frame, person, weight);

            for(int i=0;i<person.size();i++)
            {
                Rect r = person[i]; //assign person to rectangle
                rectangle(frame, r, Scalar(255,0,0));   //draw it
                double a = weight[i];       //convert score to str
                stringstream ss;
                ss << a;
                string str = ss.str();
                putText(frame,str,Point(r.x,r.y+50),FONT_HERSHEY_SIMPLEX, 1,Scalar(255,0,0));   //draw str
                tracking.emplace_back(Point(r.x+r.width/2,r.y+r.height/2));        //push middle of rect to tracking list for drawing history of position
            }
            for(int j=1;j<tracking.size();j++)  //j = 1 to avoid out of range
            {
                line(frame, tracking[j-1], tracking[j], Scalar(255,255,255), 2);
            }

            imshow("frame", frame);
            auto c=(char)waitKey(25);
            if(c==27 or c=='q') break;
        }
        input.release();
        destroyAllWindows();
    }
    return 0;
}