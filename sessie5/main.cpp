#include <iostream>
#include <opencv2/opencv.hpp>
#define STRAWBERRY 0


using namespace std;
using namespace cv;
using namespace ml;

void redrawPuntjes();
void redrawAll();
vector<Point> strawberryList;
vector<Point> backgroundList;
int modus;              //background or strawberry selection
Mat kloon;              //to copy the original into before drawing all over it
Mat input;              //global input mat to use in the function


/**
 * @brief Function that gets called when a mouse event occurs
 * @param event the type of event; which button is pressed/released
 * @param x  the x value of the current location of the mouse
 * @param y  the y value of the current location of the mouse
 * @param flags j'ai aucun idee
 * @param userdata iets vreemd
 */
void mouseCallBack(int event, int x, int y, int flags, void* userdata)
{
    if(event==EVENT_LBUTTONDOWN)    //add point
    {
        if(modus==STRAWBERRY)
        {   //add to list+ draw red circle on location
            strawberryList.push_back(Point(x,y));
            cout << "Add " << x << " , " << y << " to Strawberry" <<endl;
            circle(kloon, Point(x,y),2, Scalar(0,0,255), -1);

        }
        else
        {   //add to list+draw green circle
            backgroundList.push_back(Point(x,y));
            cout << "Add " << x << " , " << y << " to background" <<endl;
            circle(kloon, Point(x,y),2, Scalar(0,255,0), -1);

        }
    }
    else if(event==EVENT_RBUTTONDOWN)   //delete last point in correct modus
    {
        if(modus==STRAWBERRY)
        {
            if(strawberryList.empty())
            {
                cout << "Strawberry list empty" <<endl;
            } else{
                cout << "remove " << strawberryList.back().x << " , " << strawberryList.back().y << " from strawberry" <<endl;
                strawberryList.pop_back();
                redrawAll();
            }
        }
        else    //make sure we can't delete something that isn't there, such as my will to live
        {
            if(backgroundList.empty())
            {
                cout << "Background list empty" << endl;
            } else{
                cout << "remove " << backgroundList.back().x << " , " << backgroundList.back().y << " from background" <<endl;
                backgroundList.pop_back();
                redrawAll();
            }
        }
    }
    else if(event==EVENT_MBUTTONDOWN)
    {   //print out the lists with points
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
/**
 * @brief function that gets called when the value of the slider changes
 * @param value
 * @param userdata
 */
void trackbarCallBack(int value, void* userdata)
{
    redrawAll();
}
/**
 * @brief main routine
 * @param argc amount of parameter inputs
 * @param argv array of parameter inputs
 * @return returns 0 when correct
 */
int main(int argc, const char** argv) {
    /**
     * @brief function that prints the help message, and parses the user input
     */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message \n\t\t left click:add point \n\t\t right click:remove last point \n\t\t middle mouse button print list}"
                             "{@input | | Required: (absolute) path to input image}"
                             "{knearest k||use the Nearest neighbour algorithm}"
                             "{bayes b | |use the bayes algorithm}"
                             "{svm s | | use the support vector machine algorithm}"
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

    namedWindow("Input image", WINDOW_AUTOSIZE);   //create a window to display everything
    setMouseCallback("Input image", mouseCallBack,NULL);    //enable the mousecallback
    createTrackbar("STRAWBERRY-BACKGROUND", "Input image", &modus,1,trackbarCallBack);  //use a trackbar to select modi


    redrawAll();    //make sure the correct text is printed on the screen
    waitKey(0);     //wait for the user to select all points
    Mat input_hsv;
    cvtColor(input, input_hsv, COLOR_BGR2HSV);  //create and convert the input image to hsv colorspace to make everything easier

    Mat trainingStrawberries(strawberryList.size(), 3, CV_32FC1);
    Mat labelsStrawberries = Mat::ones(strawberryList.size(),1,CV_32SC1);

    Mat trainingBackground(backgroundList.size(), 3, CV_32FC1);
    Mat labelBackground = Mat::zeros(backgroundList.size(),1,CV_32SC1);     //create matrixes for the trainingdata and labels for fore- and background

    for(int i=0;i<strawberryList.size();i++)    //loop over the foreground list
    {
        Vec3b descriptor = input_hsv.at<Vec3b>(strawberryList[i].y, strawberryList[i].x);   //create a descriptor for the training
        trainingStrawberries.at<float>(i,0) = descriptor[0];
        trainingStrawberries.at<float>(i,1) = descriptor[1];
        trainingStrawberries.at<float>(i,2) = descriptor[2];
    }
    cerr << "Done training foreground"<< endl;
    for(int i=0;i<backgroundList.size();i++)
    {
        Vec3b descriptor = input_hsv.at<Vec3b>(backgroundList[i].y, backgroundList[i].x);
        trainingBackground.at<float>(i,0) = descriptor[0];
        trainingBackground.at<float>(i,1) = descriptor[1];
        trainingBackground.at<float>(i,2) = descriptor[2];
    }

    cerr << "Done training background"<< endl;

    Mat trainingData;
    Mat labels;
    vconcat(trainingStrawberries,trainingBackground,trainingData);
    vconcat(labelsStrawberries,labelBackground,labels);



    cerr<<"Training kNearest" << endl;

    Ptr<KNearest> kNN = KNearest::create();
    Ptr<TrainData> trainingDatakNN = TrainData::create(trainingData, ROW_SAMPLE, labels);
    kNN->setIsClassifier(true);
    kNN->setAlgorithmType(KNearest::BRUTE_FORCE);
    kNN->setDefaultK(3);
    kNN->train(trainingDatakNN);

    cerr << "Done with kNearest"<<endl;



    cerr << "Training Bayes" << endl;
    Ptr<NormalBayesClassifier> nB = NormalBayesClassifier::create();
    nB->train(trainingData, ROW_SAMPLE, labels);
    cerr << "Done with Bayes" << endl;





    cerr << "Training SVM" << endl;
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    svm->train(trainingData, ROW_SAMPLE, labels);

    cerr << "Done woth SVM" << endl;


    Mat labels_kNN, labels_nB, labels_svm;
    Mat mask_kNN = Mat::zeros(input.rows,input.cols,CV_8UC1);
    Mat mask_nB = Mat::zeros(input.rows,input.cols,CV_8UC1);
    Mat mask_svm = Mat::zeros(input.rows,input.cols,CV_8UC1);

    for(int i=0;i<input.rows;i++)
    {
        for(int j=0;j<input.cols;j++)
        {
            Vec3b pixval = input_hsv.at<Vec3b>(i,j);
            Mat data_test(1,3,CV_32FC1);
            data_test.at<float>(0,0) = pixval[0];
            data_test.at<float>(0,1) = pixval[1];
            data_test.at<float>(0,2) = pixval[2];

            kNN->findNearest(data_test, kNN->getDefaultK(), labels_kNN);
            nB->predict(data_test, labels_nB);
            svm->predict(data_test,labels_svm);


            mask_kNN.at<uchar>(i,j) = labels_kNN.at<float>(0,0);
            mask_nB.at<uchar>(i,j) = labels_nB.at<int>(0,0);
            mask_svm.at<uchar>(i,j) = labels_svm.at<float>(0,0);

        }
    }


    //mask the input to show results
    Mat result_kNN, result_nB, result_svm;
    bitwise_and(input, input, result_kNN,mask_kNN);
    bitwise_and(input, input, result_nB,mask_nB);
    bitwise_and(input, input, result_svm,mask_svm);
    if(parser.has("knearest"))
    {
        imshow("seg kNearest", mask_kNN*255);
        imshow("res kNearest", result_kNN);
    }
    if(parser.has("bayes"))
    {
        imshow("seg normal_Bayes", mask_nB*255);
        imshow("res Bayes", result_nB);
    }
    if(parser.has("svm"))
    {

        imshow("res svm", result_svm);
        imshow("seg svm", mask_svm*255);
    }
    waitKey(0);
    return 0;
}
/**
 * @brief this function copys the original to a clone, before messing the picture up with circles and text
 */
void redrawPuntjes(void)
{
    for(int i=0;i<strawberryList.size();i++)
    {
        circle(kloon, Point(strawberryList.at(i).x,strawberryList.at(i).y),2, Scalar(0,0,255), -1);
    }
    for(int i=0;i<backgroundList.size();i++)
    {
        circle(kloon, Point(backgroundList.at(i).x,backgroundList.at(i).y),2, Scalar(0,255,0), -1);
    }
}
/**
 * @brief As the name suggests, this function is called on deletion of a point, or switching of a modus, and redraws the whole image
 */
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