#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace cv;

struct myclass {
    bool operator() (int pt1, int pt2) { return (pt1 < pt2);}
} comparator;

void mouseCallBack(int event, int x, int y, int flags, void* userdata)
{
    if(event==EVENT_LBUTTONDOWN)    //add point
    {

        cout << "Point " << x << " , " << y <<  endl;

    }
}



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


    Mat dst(image_gray.size(), input.type());








    vector<Vec2f> lines;
    vector<int> lijnen;
    HoughLines(notenbalk, lines, 1, CV_PI/180, 1000,0,0);
    cout << lines.size() << endl;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        cout << "x0: " << x0 << ", y0: " << y0 <<endl;
        cout << "rho: " << rho << "theta " << theta <<endl;
        pt1.x = 0;
        pt1.y = y0;
        pt2.x = dst.cols;
        pt2.y = y0;
        line( dst, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
        lijnen.push_back(y0);
        //cout << "p1: "<<pt1.x<< ',' << pt1.y << ";pt2" << pt2.x <<"," <<pt2.y<<endl;
    }

    namedWindow("lijnen", WINDOW_AUTOSIZE);   //create a window to display everything
    setMouseCallback("lijnen", mouseCallBack,NULL);    //enable the mousecallback
    imshow("lijnen", dst);
    waitKey(0);

    sort(lijnen.begin(),lijnen.end(),comparator);
    for(int i=0;i<lijnen.size();i++)
    {
        cout << lijnen.at(i) << endl;
    }
    int mi_lijn = lijnen.at(0);
    int sol_lijn = lijnen.at(1);
    int si_lijn = lijnen.at(2);
    int re_lijn = lijnen.at(3);
    int fa_lijn = lijnen.at(4);


    Mat noten = bin.clone();
    // Specify size on vertical axis
    int verticalsize = noten.rows /200;

    // Create structure element for extracting vertical lines through morphology operations
    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));

    // Apply morphology operations
    erode(noten, noten, verticalStructure, Point(-1, -1));
    dilate(noten, noten, verticalStructure, Point(-1, -1));


    imshow("vertical", noten);
    waitKey(0);

    vector<Vec2f> bolletjes;





    return 0;

}