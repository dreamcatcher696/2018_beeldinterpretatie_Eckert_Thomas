#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace cv;


RNG rng(12345);
struct myclass {
    bool operator() (int pt1, int pt2) { return (pt1 < pt2);}
} comparator;

struct myclass2 {
    bool operator() (vector<Point> pt1, vector<Point> pt2) {return (pt1.at(0).x < pt2.at(0).x);}
} comparator2;

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
    Mat finaal = Mat::zeros(input.size(),input.type());
    cvtColor(input, image_gray, COLOR_BGR2GRAY);


    if (parser.has("debug")) {
        namedWindow("gray image", WINDOW_AUTOSIZE);
        imshow("gray image", image_gray);
        waitKey(0);
    }
    Mat bin(image_gray.size(), image_gray.type());
    image_gray = ~image_gray;
    adaptiveThreshold(image_gray, bin, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,15, -2);

    if(parser.has("debug"))
    {
        imshow("binary image", bin);
        waitKey(0);
    }


    Mat notenbalk = bin.clone();
    int horizontalsize = notenbalk.cols/25;

    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));
    erode(notenbalk,notenbalk,horizontalStructure);
    dilate(notenbalk, notenbalk, horizontalStructure);
    if(parser.has("debug"))
    {
        imshow("notenbalk", notenbalk);
        waitKey(0);
    }



    Mat dst(image_gray.size(), input.type());
    Mat notenbalk_lijnen = Mat::zeros(image_gray.size(), CV_32SC3);

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
        if(parser.has("debug"))
        {
            cout << "x0: " << x0 << ", y0: " << y0 <<endl;
            cout << "rho: " << rho << "theta " << theta <<endl;
        }

        pt1.x = 0;
        pt1.y = y0;
        pt2.x = dst.cols;
        pt2.y = y0;
        line( dst, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
        line( notenbalk_lijnen, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
        line(finaal, pt1, pt2, Scalar(255, 255, 255), 1, CV_AA);

        lijnen.push_back(y0);
        //cout << "p1: "<<pt1.x<< ',' << pt1.y << ";pt2" << pt2.x <<"," <<pt2.y<<endl;
    }
    if(parser.has("debug"))
    {
        namedWindow("lijnen", WINDOW_AUTOSIZE);   //create a window to display everything
        setMouseCallback("lijnen", mouseCallBack,NULL);    //enable the mousecallback
        imshow("lijnen", dst);
        waitKey(0);
    }


    sort(lijnen.begin(),lijnen.end(),comparator);
    for(int i=0;i<lijnen.size();i++)
    {
        //cout << lijnen.at(i) << endl;
    }

    int fa_lijn = lijnen.at(0);
    int re_lijn = lijnen.at(1);
    int si_lijn = lijnen.at(2);
    int sol_lijn = lijnen.at(3);
    int mi_lijn = lijnen.at(4);
    int helft = (fa_lijn-re_lijn)/2;

    int C3 = mi_lijn + 2*helft;
    int D3 = mi_lijn + helft;
    int E3 = mi_lijn;
    int F3 = mi_lijn - helft;
    int G3 = sol_lijn;
    int A3 = sol_lijn - helft;
    int B3 = si_lijn;
    int C4 = si_lijn-helft;
    int D4 = re_lijn;
    int E4 = re_lijn-helft;
    int F4 = fa_lijn;
    int G4 = fa_lijn - helft;
    int A4 = fa_lijn -2*helft;
    cout << "C3: " << C3 << endl;
    cout << "D3: " << D3 << endl;
    cout << "E3: " << F3 << endl;
    cout << "F3: " << F3 << endl;
    cout << "G3: " << G3 << endl;
    cout << "A3: " << A3 << endl;
    cout << "B3: " << B3 << endl;
    cout << "C4: " << C4 << endl;
    cout << "D4: " << D4 << endl;
    cout << "E4: " << E4 << endl;
    cout << "F4: " << F4 << endl;
    cout << "G4: " << G4 << endl;
    cout << "A4: " << A4 << endl;





    Mat noten = bin.clone();
    int verticalsize = noten.rows /200;     //was 200

    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));


    erode(noten, noten, verticalStructure, Point(-1, -1));
    dilate(noten, noten, verticalStructure, Point(-1, -1));

    if(parser.has("debug"))
    {
        imshow("vertical", noten);
        waitKey(0);
    }


    vector<vector<Point>> contouren;
    findContours(noten.clone(), contouren, RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    vector <vector <Point>> hulls;
    for(size_t i=0;i<contouren.size();i++)
    {
       // cout << contouren[i] <<endl;
        vector<Point> hull;
        convexHull(contouren[i],hull);
        hulls.push_back(hull);
    }

    Mat voor=dst.clone();
    Mat na = dst.clone();
    drawContours(voor, contouren, -1, Scalar(255,255,255),-1);
    if(parser.has("debug"))
    {
        imshow("noten voor erase", voor);
        waitKey(0);
    }

    //cout << "voor:" << contouren.size() <<endl;
    vector<vector<Point>> naaa;
    for(int i=0;i<contouren.size();i++)
    {
        vector<Point> pointlist;
       // cout << "contour "<< i  << "=" <<contouren.at(i) <<endl;
        //cout << "first y value = " << contouren.at(i).at(0).y <<endl;
        if(contouren.at(i).at(0).y<200)
        {
            contouren.erase(contouren.begin()+i);
            //cout << "erased" <<endl<<endl;
        }
        else{
            naaa.push_back(contouren.at(i));
        }
    }

    sort(naaa.begin(),naaa.end(),comparator2);
    for(int i=0;i<naaa.size();i++)
    {
        //cout << "contour " << i << ":" << naaa.at(i) <<endl<<endl;
    }

    /////////
    Mat noten_bollen = Mat::zeros(image_gray.size(), CV_8UC1);
    drawContours(noten_bollen, naaa, -1, Scalar(255,0,0),-1);

    Mat elipsStructure = getStructuringElement(MORPH_ELLIPSE, Size(8,4));
    erode(noten_bollen, noten_bollen, elipsStructure, Point(-1, -1));
    dilate(noten_bollen, noten_bollen, elipsStructure, Point(-1, -1));
    imshow("noten_bollen", noten_bollen);
    waitKey(0);



    vector<vector<Point>> noten_bollen_contouren;
    findContours(noten_bollen.clone(), noten_bollen_contouren, RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

    //cout << "aantal noten: " << noten_bollen_contouren.size() << endl;
    sort(noten_bollen_contouren.begin(),noten_bollen_contouren.end(),comparator2);
    drawContours(finaal, noten_bollen_contouren, -1, Scalar(255,0,0),-1);


    for(int i=0;i<noten_bollen_contouren.size();i++)
    {
        cout << "noot " << i <<" : "  << boundingRect(noten_bollen_contouren.at(i)).x<<","<< boundingRect(noten_bollen_contouren.at(i)).y<<endl;
        cout << "midden: " << boundingRect(noten_bollen_contouren.at(i)).x+boundingRect(noten_bollen_contouren.at(i)).width/2<<" , "<<boundingRect(noten_bollen_contouren.at(i)).y+boundingRect(noten_bollen_contouren.at(i)).height/2 << endl;
    }










    Mat noten_lijnen = Mat::zeros(image_gray.size(), CV_8UC1);
    drawContours(noten_lijnen, naaa, -1, Scalar(255,0,0),-1);
    int verticalsize2 = noten_lijnen.rows /50;

    Mat verticalStructure2 = getStructuringElement(MORPH_RECT, Size( 1,verticalsize2));


    erode(noten_lijnen, noten_lijnen, verticalStructure2, Point(-1, -1));
    dilate(noten_lijnen, noten_lijnen, verticalStructure2, Point(-1, -1));

    vector<vector<Point>> noten_lijnen_contouren;
    findContours(noten_lijnen.clone(), noten_lijnen_contouren, RETR_EXTERNAL,CHAIN_APPROX_NONE);
    sort(noten_lijnen_contouren.begin(),noten_lijnen_contouren.end(),comparator2);

    vector<vector<Point>> maatstrepen;
    vector<vector<Point>> nootlijnen;
    for(int i=0;i<noten_lijnen_contouren.size();i++)
    {

        //cout << "contour " << i << endl;
        //cout << "width: " << boundingRect(noten_lijnen_contouren.at(i)).width <<"; height: " << boundingRect(noten_lijnen_contouren.at(i)).height << endl;
        if(boundingRect(noten_lijnen_contouren.at(i)).height==(mi_lijn- fa_lijn) or boundingRect(noten_lijnen_contouren.at(i)).height==((mi_lijn-fa_lijn)+1) or boundingRect(noten_lijnen_contouren.at(i)).height==(mi_lijn-fa_lijn+2))
        {
            maatstrepen.push_back(noten_lijnen_contouren.at(i));
            //noten_lijnen_contouren.erase(noten_lijnen_contouren.begin()+i);
            //cout << "maatstreep!" <<endl<<endl;
        } else{
            nootlijnen.push_back(noten_lijnen_contouren.at(i));
            //cout << "nootlijn" <<endl<<endl;
        }
    }
    //cout << nootlijnen.size() << " noten gedetcteerd" << endl;
    //cout << maatstrepen.size() << " maatstrepen gedetecteerd" << endl;
    drawContours(finaal, nootlijnen, -1, Scalar(255,0,255),-1);

    drawContours(finaal, maatstrepen, -1, Scalar(255,255,255),-1);
    namedWindow("finaal", WINDOW_AUTOSIZE);   //create a window to display everything
    setMouseCallback("finaal", mouseCallBack,NULL);    //enable the mousecallback
    imshow("finaal", finaal);
    waitKey(0);



    //imshow("noten_lijnen", noten_lijnen);
    //waitKey(0);
    ////////
    /*vector<vector<Point> > contours_poly( naaa.size() );
    vector<Rect> boundRect( naaa.size() );
    vector<Point2f>center( naaa.size() );
    vector<float>radius( naaa.size() );
    for( size_t i = 0; i < naaa.size(); i++ )
    { approxPolyDP( Mat(naaa[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );

    }
    for( size_t i = 0; i< naaa.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );

        rectangle( na, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );

    }*/


    //drawContours(na, naaa, -1, Scalar(255,0,0),-1);
    /*if(parser.has("debug"))
    {
        imshow("noten?", na);
        waitKey(0);
    }*/


   //notenbalk_lijnen.convertTo(noten_bollen, CV_32FC1);

    Mat final = Mat::zeros(image_gray.size(), CV_32FC1);
    Mat temp = Mat::zeros(image_gray.size(),CV_32FC1);

    //addWeighted(noten_bollen,1,noten_lijnen,1,0.0,temp);
    //addWeighted(temp,1, notenbalk_lijnen,1,0.0,final);
    //imshow("final", temp);
   // waitKey(0);




    return 0;

}