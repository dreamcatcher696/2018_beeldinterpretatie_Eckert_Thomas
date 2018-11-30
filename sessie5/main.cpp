#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char** argv) {
    /**
    *     helper function to parse commands
    */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message| in case of MACOS:DO NOT MOVE WINDOW}"
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
    Mat input = imread(input_path);      //template1 cause template is in use by c++
    if (input.empty()) {
        cerr << "could not read input" << endl;
        parser.printMessage();
        return -1;
    }
    return 0;
}