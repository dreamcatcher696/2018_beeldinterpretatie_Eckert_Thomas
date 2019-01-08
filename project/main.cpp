#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

int main(int argc, const char** argv) {
    /**
     * @brief function that prints the help message, and parses the user input
     */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message}"
                             "{@input | | Required: (absolute) path to sheet}"
                             "{debug d | | Use this option to show all calculated frames}"
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

    Mat input = imread(input_path);
    if (input.empty()) {
        cerr << "could not read input; check parameters" << endl;
        parser.printMessage();
        return -1;
    }
    if (parser.has("debug")) {
        namedWindow("input image", WINDOW_AUTOSIZE);
        imshow("input image", input);
        waitKey(0);
    }

    Mat image_gray;
    cvtColor(input, image_gray, COLOR_BGR2GRAY);


    if (parser.has("debug")) {
        namedWindow("gray image", WINDOW_AUTOSIZE);
        imshow("gray image", image_gray);
        waitKey(0);



    }
    Mat bin(image_gray.size(), image_gray.type());
    image_gray = ~image_gray;
    adaptiveThreshold(image_gray, bin, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,15, -2);

    imshow("test", bin);
    waitKey(0);



    Mat notenbalk = bin.clone();
    int horizontalsize = notenbalk.cols/25;

    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));
    erode(notenbalk,notenbalk,horizontalStructure);
    dilate(notenbalk, notenbalk, horizontalStructure);
    imshow("notenbalk", notenbalk);
    waitKey(0);


    Mat edges(image_gray.size(), image_gray.type());
    Mat dst(image_gray.size(), input.type());
    //Canny(notenbalk, edges, 100, 255);
    //imshow("edges", edges);
    waitKey(0);


    vector<Vec2f> lines;
    HoughLines(notenbalk, lines, 1, CV_PI/180, 1000,0,0);
    //vector<Vec4i> lines;
    //HoughLinesP(edges, lines, 1, CV_PI/180, 1, 50, 10 );
    cout << lines.size() << endl;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line( dst, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
    }

    imshow("lijnen", dst);
    waitKey(0);


    return 0;

}