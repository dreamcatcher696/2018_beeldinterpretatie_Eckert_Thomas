#include <iostream>
#include <opencv2/opencv.hpp>
#include <queue>

using namespace std;
using namespace cv;

int match_method;
int amount_matches = 0;
int threshval;
Mat grayInput;
Mat grayTemplate;
Mat input;
const char* image_window = "source Image";
const char* result_window = "result window";

void MatchingMethod(int, void*);


int main(int argc, const char** argv) {
    /**
    *     helper function to parse commands
    */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message| in case of MACOS:DO NOT MOVE WINDOW}"
                             "{@template|  | (required) path to template}"
                             "{@input| |(required) path to input inmage}"
                             "{(f)indall f | | use f to find all matches instead of best match}"
    );
    //if help
    if (parser.has("help")) {
        parser.printMessage();
        cerr << "Use absolute path" << endl;
        return 1;
    }
    //check template location
    string template_path(parser.get<string>("@template"));
    if (template_path.empty()) {
        cerr << "no template location given" << endl;
        parser.printMessage();
        return -1;
    }
    //read in template
    Mat template1 = imread(template_path);      //template1 cause template is in use by c++
    if (template1.empty()) {
        cerr << "could not read template image" << endl;
        parser.printMessage();
        return -1;
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

    cvtColor(template1, grayTemplate, COLOR_BGR2GRAY);
    cvtColor(input, grayInput, COLOR_BGR2GRAY);

    namedWindow(image_window,WINDOW_AUTOSIZE);
    namedWindow(result_window,WINDOW_AUTOSIZE);
    const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM_CCORR \n 3: TM CCORR NORMED \n 4:TM COEFF\n 5: TM COEFF NORMED";
    const char* amount_of_matches = "0: only single match \n 1: all matches";

    createTrackbar(trackbar_label, result_window, &match_method, 5, MatchingMethod);
    if(parser.has("f"))
    {
        createTrackbar(amount_of_matches, result_window, &amount_matches,1, MatchingMethod);
        createTrackbar("Threshold", result_window, &threshval, 255, MatchingMethod);
    }


    MatchingMethod(0,0);
    waitKey(0);

    return 0;
}
void MatchingMethod(int, void*)
{
    Mat result;

    matchTemplate(grayInput, grayTemplate, result, match_method);
    normalize(result, result, 0,1,NORM_MINMAX, -1, Mat());
    if(amount_matches==0)
    {
        double minVal,maxVal;
        Point minLoc, maxLoc;
        Point matchLoc;

        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
        if(match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
        {
            matchLoc = minLoc;

        }
        else{
            matchLoc = maxLoc;
        }

        Mat temp = grayInput.clone();
        rectangle(temp,matchLoc,Point(matchLoc.x + grayTemplate.cols, matchLoc.y + grayTemplate.rows), Scalar::all(0),2,8,0);
        imshow(result_window, temp);
        waitKey(0);
    }
    else
    {
        Mat mask;
        inRange(result, ((float)threshval/255),1,mask);
        imshow("Threshold", mask);
        cout<<threshval<<endl;
        Mat temp;
        input.copyTo(temp);
        vector<vector<Point>> contours;
        findContours(mask,contours,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for(int i=0;i<contours.size();i++)
        {
            Rect region = boundingRect(contours[i]);
            Mat temp2 = result(region);
            Point maxLoc;

            minMaxLoc(temp2,nullptr,nullptr,nullptr,&maxLoc);
            rectangle(temp, Point(region.x+maxLoc.x, region.y+maxLoc.y), Point(maxLoc.x+region.x+grayTemplate.cols, maxLoc.y+region.y+grayTemplate.rows),Scalar(0,255,0), 2,8,0);
        }
        imshow(result_window, temp);
        waitKey(0);
    }

    return;
}