#include <iostream>
#include <opencv2/opencv.hpp>
#include <queue>

using namespace std;
using namespace cv;

int match_method;
Mat grayInput;
Mat grayTemplate;
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
    Mat input = imread(input_path);      //template1 cause template is in use by c++
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
    createTrackbar(trackbar_label, result_window, &match_method, 5, MatchingMethod);

    MatchingMethod(0,0);
    waitKey(0);

    return 0;
}
void MatchingMethod(int, void*)
{
    Mat result;



    matchTemplate(grayInput, grayTemplate, result, match_method);
    normalize(result, result, 0,1,NORM_MINMAX, -1, Mat());

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
    //rectangle(result,matchLoc,Point(matchLoc.x + grayTemplate.cols, matchLoc.y + grayTemplate.rows), Scalar::all(0),2,8,0);
    //imshow("template", grayTemplate);
    //imshow("normalised", result);
    imshow(result_window, temp);

    Mat mask, temp2;
    threshold(result, mask,0.9,1,THRESH_BINARY);
    mask.convertTo(mask,CV_8UC1);
    mask *= 255;
    imshow("thresholded mask", mask);

    temp2 = grayInput.clone();




    imshow(result_window, temp);
    waitKey(0);
    return;
}