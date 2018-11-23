#include <iostream>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

const int MAX_FEATURES = 500;
const float percentage = 0.7;

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

        std::vector<DMatch> matchesORB;
        Ptr<DescriptorMatcher> matcherORB = DescriptorMatcher::create("BruteForce-Hamming");
        matcherORB->match(descriptorsORB1, descriptorsORB2, matchesORB, Mat());


        sort(matchesORB.begin(),matchesORB.end());

        Mat ORBMatches;
        drawMatches(ORB1, keypointsORB1, ORB2, keypointsORB2, matchesORB, ORBMatches);

        drawKeypoints(ORB1, keypointsORB1,ORB1);
        drawKeypoints(ORB2, keypointsORB2, ORB2);

        imshow("ORB_INPUT", ORB1);
        imshow("ORB_OBJECT", ORB2);
        imshow("ALL_ORB_MATCHES", ORBMatches);
        //waitKey(0);

        // Extract location of good matches
        std::vector<Point2f> pointsORB1, pointsORB2;

        for( size_t i = 0; i < matchesORB.size(); i++ )
        {
            pointsORB1.push_back( keypointsORB1[ matchesORB[i].queryIdx ].pt );
            pointsORB2.push_back( keypointsORB2[ matchesORB[i].trainIdx ].pt );
        }

        // Find homography
        Mat h = findHomography( pointsORB1, pointsORB2, RANSAC );
        Mat im1RegORB;
        // Use homography to warp image
        warpPerspective(input, im1RegORB, h, ORB2.size());
        imshow("ORB_RANSAC", im1RegORB);

        /*
         * std::vector<Point2f> obj_corners(4);
         * obj_corners[0] = cvPoint(0,0);obj_corner[1] = cvPoint(img_1.cols,0) 3 = (0,im_1.rows);
         *
         * line(img_matches,scene_corners[0]
         *
         */
        //eventueel todo: kadertjes tekenen



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

        std::vector<DMatch> matchesBRISK;
        Ptr<DescriptorMatcher> matcherBRISK = DescriptorMatcher::create("BruteForce-Hamming");
        matcherBRISK->match(descriptorsBRISK1, descriptorsBRISK2, matchesBRISK, Mat());

        sort(matchesBRISK.begin(),matchesBRISK.end());
        Mat BRISKMatches;
        drawMatches(BRISK1, keypointsBRISK1, BRISK2, keypointsBRISK2, matchesBRISK, BRISKMatches);



        drawKeypoints(BRISK1, keypointsBRISK1,BRISK1);
        drawKeypoints(BRISK2, keypointsBRISK2, BRISK2);

        imshow("BRISK_INPUT", BRISK1);
        imshow("BRISK_OBJECT", BRISK2);
        imshow("BRISK_ALL_MATCHES", BRISKMatches);
        //waitKey(0);

        // Extract location of good matches
        std::vector<Point2f> pointsBRISK1, pointsBRISK2;

        for( size_t i = 0; i < matchesBRISK.size(); i++ )
        {
            pointsBRISK1.push_back( keypointsBRISK1[ matchesBRISK[i].queryIdx ].pt );
            pointsBRISK2.push_back( keypointsBRISK2[ matchesBRISK[i].trainIdx ].pt );
        }

        // Find homography
        Mat h = findHomography( pointsBRISK1, pointsBRISK2, RANSAC );
        Mat im1RegBRISK;
        // Use homography to warp image
        warpPerspective(input, im1RegBRISK, h, BRISK2.size());
        imshow("BRISK_RANSAC", im1RegBRISK);

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

        //BFMatcher matcher(NORM_L2)

        std::vector<DMatch> matchesAKAZE;
        Ptr<DescriptorMatcher> matcherAKAZE = DescriptorMatcher::create("BruteForce-Hamming");
        matcherAKAZE->match(descriptorsAKAZE1, descriptorsAKAZE2, matchesAKAZE, Mat());

        sort(matchesAKAZE.begin(),matchesAKAZE.end());
        Mat AKAZEMatches;
        drawMatches(AKAZE1, keypointsAKAZE1, AKAZE2, keypointsAKAZE2, matchesAKAZE, AKAZEMatches);

        drawKeypoints(AKAZE1, keypointsAKAZE1,AKAZE1);
        drawKeypoints(AKAZE2, keypointsAKAZE2, AKAZE2);

        imshow("AKAZE_INPUT", AKAZE1);
        imshow("AKAZE_OBJECT", AKAZE2);
        imshow("AKAZE_ALL_MATCHES", AKAZEMatches);
        //waitKey(0);

        // Extract location of good matches
        std::vector<Point2f> pointsAKAZE1, pointsAKAZE2;

        for( size_t i = 0; i < matchesAKAZE.size(); i++ )
        {
            pointsAKAZE1.push_back( keypointsAKAZE1[ matchesAKAZE[i].queryIdx ].pt );
            pointsAKAZE2.push_back( keypointsAKAZE2[ matchesAKAZE[i].trainIdx ].pt );
        }

        // Find homography
        Mat h = findHomography( pointsAKAZE1, pointsAKAZE2, RANSAC );
        Mat im1RegAKAZE;
        // Use homography to warp image
        warpPerspective(input, im1RegAKAZE, h, AKAZE2.size());
        imshow("AKAZE_RANSAC", im1RegAKAZE);
    }
    waitKey(0);
    return 0;
}
