#include <iostream>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

const int MAX_FEATURES = 500;

int main(int argc, const char** argv) {
    /**
    *     helper function to parse commands
    */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message| in case of MACOS:DO NOT MOVE WINDOW}"
                             "{@input| |(required) path to input image (gray)}"
                             "{@object|  | (required) path to object (gray)}"
                             "{ORB O| |Use ORB-Keypoint type}"
                             "{BRISK B| |Use BRISK-Keypoint type}"
                             "{AKAZE A| |Use AKAZE-Keypoint type}"
    );
    //if help
    if (parser.has("help")) {
        parser.printMessage();
        cerr << "Use absolute path" << endl;
        return 1;
    }
    //check object location
    string object_path(parser.get<string>("@object"));
    if (object_path.empty()) {
        cerr << "no object location given" << endl;
        parser.printMessage();
        return -1;
    }
    //read in object
    Mat object = imread(object_path, IMREAD_GRAYSCALE);      //template1 cause template is in use by c++
    if (object.empty()) {
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
    Mat input = imread(input_path, IMREAD_GRAYSCALE);      //template1 cause template is in use by c++
    if (input.empty()) {
        cerr << "could not read input" << endl;
        parser.printMessage();
        return -1;
    }

    if(parser.has("ORB"))
    {

        Mat ORB1,ORB2;
        input.copyTo(ORB1);
        object.copyTo(ORB2);

        vector<KeyPoint> keypointsORB1, keypointsORB2;
        Mat descriptorsORB1, descriptorsORB2;

        Ptr<Feature2D> orb = ORB::create(MAX_FEATURES);
        orb->detectAndCompute(ORB1, Mat(), keypointsORB1, descriptorsORB1);
        orb->detectAndCompute(ORB2, Mat(), keypointsORB2, descriptorsORB2);

        /*std::vector<DMatch> matches;
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
        matcher->match(descriptors1, descriptors2, matches, Mat());*/

        //Mat imMatches;

        drawKeypoints(ORB1, keypointsORB1,ORB1);
        drawKeypoints(ORB2, keypointsORB2, ORB2);
        //drawMatches(input, keypoints1, object, keypoints2, matches, imMatches);
        imshow("ORB_INPUT", ORB1);
        imshow("ORB_OBJECT", ORB2);
        //waitKey(0);

    }
    if(parser.has("BRISK"))
    {
        Mat BRISK1,BRISK2;
        input.copyTo(BRISK1);
        object.copyTo(BRISK2);

        vector<KeyPoint> keypointsBRISK1, keypointsBRISK2;
        Mat descriptorsBRISK1, descriptorsBRISK2;

        Ptr<Feature2D> brisk = BRISK::create(60,4,0.1f);
        brisk->detectAndCompute(BRISK1, Mat(), keypointsBRISK1, descriptorsBRISK1);
        brisk->detectAndCompute(BRISK2, Mat(), keypointsBRISK2, descriptorsBRISK2);

        /*std::vector<DMatch> matches;
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
        matcher->match(descriptors1, descriptors2, matches, Mat());*/

        //Mat imMatches;

        drawKeypoints(BRISK1, keypointsBRISK1,BRISK1);
        drawKeypoints(BRISK2, keypointsBRISK2, BRISK2);
        //drawMatches(input, keypoints1, object, keypoints2, matches, imMatches);
        imshow("BRISK_INPUT", BRISK1);
        imshow("BRISK_OBJECT", BRISK2);
        //waitKey(0);

    }
    if(parser.has("AKAZE"))
    {
        Mat AKAZE1,AKAZE2;
        input.copyTo(AKAZE1);
        object.copyTo(AKAZE2);

        vector<KeyPoint> keypointsAKAZE1, keypointsAKAZE2;
        Mat descriptorsAKAZE1, descriptorsAKAZE2;

        Ptr<Feature2D> brisk = AKAZE::create();
        brisk->detectAndCompute(AKAZE1, Mat(), keypointsAKAZE1, descriptorsAKAZE1);
        brisk->detectAndCompute(AKAZE2, Mat(), keypointsAKAZE2, descriptorsAKAZE2);

        /*std::vector<DMatch> matches;
        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
        matcher->match(descriptors1, descriptors2, matches, Mat());*/

        //Mat imMatches;

        drawKeypoints(AKAZE1, keypointsAKAZE1,AKAZE1);
        drawKeypoints(AKAZE2, keypointsAKAZE2, AKAZE2);
        //drawMatches(input, keypoints1, object, keypoints2, matches, imMatches);
        imshow("AKAZE_INPUT", AKAZE1);
        imshow("AKAZE_OBJECT", AKAZE2);
        //waitKey(0);
    }
    waitKey(0);
    return 0;
}
