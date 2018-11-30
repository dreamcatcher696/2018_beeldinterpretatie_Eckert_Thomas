#include <iostream>
#include <opencv2/opencv.hpp>
#define STRAWBERRY 0
#define BACKGROUND 1



using namespace std;
using namespace cv;

void redrawPuntjes();
void redrawAll();
vector<Point> strawberryList;
vector<Point> backgroundList;
int modus;
Mat kloon;
Mat input;

//TODO implement puttext()

void mouseCallBack(int event, int x, int y, int flags, void* userdata)
{
    if(event==EVENT_LBUTTONDOWN)
    {
        if(modus==STRAWBERRY)
        {
            strawberryList.push_back(Point(x,y));
            cout << "Add " << x << " , " << y << " to Strawberry" <<endl;   //TODO ADD POINT WHERE CLICKED
            circle(kloon, Point(x,y),2, Scalar(0,0,255), -1);

        }
        else
        {
            backgroundList.push_back(Point(x,y));
            cout << "Add " << x << " , " << y << " to background" <<endl;   //TODO ADD POINT WHERE CLICKED
            circle(kloon, Point(x,y),2, Scalar(255,0,0), -1);

        }
    }
    else if(event==EVENT_RBUTTONDOWN)
    {
        if(modus==STRAWBERRY)
        {
            if(strawberryList.empty())
            {
                cout << "Strawberry list empty" <<endl;
            } else{
                cout << "remove " << strawberryList.back().x << " , " << strawberryList.back().y << " from strawberry" <<endl;   //TODO ADD POINT WHERE CLICKED
                strawberryList.pop_back();
                redrawAll();
            }
        }
        else
        {
            if(backgroundList.empty())
            {
                cout << "Background list empty" << endl;
            } else{
                cout << "remove " << backgroundList.back().x << " , " << backgroundList.back().y << " from background" <<endl;   //TODO ADD POINT WHERE CLICKED
                backgroundList.pop_back();
                redrawAll();
            }
        }
    }
    else if(event==EVENT_MBUTTONDOWN)
    {
        cout << "********* STRAWBERRIES *********"<<endl;
        for(int i=0;i<strawberryList.size();i++)
        {
            cout << "Strawberry :"<< strawberryList.at(i).x << " , " << strawberryList.at(i).y << endl;
        }
        cout << "********* BACKGROUND *********"<<endl;
        for(int i=0;i<backgroundList.size();i++)
        {
            cout << "Background :"<< backgroundList.at(i).x << " , " << backgroundList.at(i).y << endl;

        }
    }
    imshow("Input image", kloon);
}
void trackbarCallBack(int, void*)
{
    redrawAll();
}

int main(int argc, const char** argv) {
    /**
    *     helper function to parse commands
    */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message \n\t\t left click:add point \n\t\t right click:remove last point \n\t\t middle mouse button print list}"
                             "{@input | | Required: (absolute) path to input image}"
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
    //read in template
    input = imread(input_path);      //template1 cause template is in use by c++
    if (input.empty()) {
        cerr << "could not read input" << endl;
        parser.printMessage();
        return -1;
    }
    namedWindow("Input image", WINDOW_AUTOSIZE);
    setMouseCallback("Input image", mouseCallBack,NULL);
    createTrackbar("STRAWBERRY-BACKGROUND", "Input image", &modus,1,trackbarCallBack);

    redrawAll();
    waitKey(0);
    return 0;
}
void redrawPuntjes()
{
    for(int i=0;i<strawberryList.size();i++)
    {
        circle(kloon, Point(strawberryList.at(i).x,strawberryList.at(i).y),2, Scalar(0,0,255), -1);
    }
}

void redrawAll()
{
    input.copyTo(kloon);
    redrawPuntjes();
    if(modus==STRAWBERRY)
    {
        putText(kloon, "Strawberries", Point(32,508), FONT_HERSHEY_COMPLEX, 2,Scalar(255,255,255) ,4);
    } else{
        putText(kloon, "Background", Point(32,508), FONT_HERSHEY_COMPLEX, 2,Scalar(255,255,255) ,4);
    }
    imshow("Input image", kloon);
    waitKey(1);

}