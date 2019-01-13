#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-loop-convert"
/**
 * Author: Thomas Eckert
 * this program reads in an image of some sheet music and will try to convert the notes to a audio file
 * LIBS:
 *      Sound_maker:https://github.com/gemlongman/sound-maker
 *      OPENCV 3.4.3
 * working: processing quarter notes; write to audio file; recreate sheet from data
 * TODO: process different length of notes
 * TODO: 2 notes at the same time
 * TODO: process rests
 * TODO: read in tempo, volume
 * TODO: regions of interest for reading multiple lines of staffs
 * TODO: classify the notes, and give them all a different color before drawing them
 * note: i changed some parts of the sound_maker to make my clang-tidy processor happier :) (mostly conversion warnings, doubles stored in int etc)
 */

//of course we need
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>

//includes for the sound_maker libs
#include <string>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "sound_maker.hpp"
#include <map>

//i know using namespace std kills a fluffy animal everty time someone writes it, but for the convenience we will use it anyway
using namespace std;
using namespace cv;

//variables for the sound_maker lib

constexpr double two_pi = 6.283185307179586476925286766559;
constexpr double hz = 44100.0;

// Function that will convert frequency and time duration into .wav file samples
void generate_data( SoundMaker& S, double freq, double amount_time = 1.0 ) {

    double frequency = freq;
    double seconds = amount_time;

    double chan_1; // channel 1
    double chan_2; // channel 2

    auto amplitude = (double) 32760.0;
    int period;
    frequency == 0.0 ? period = -1 : period = (int) (hz / (2.0 * frequency)) ; // number of samples in a wave length

    double samples = hz * seconds;
    int x;
    double value;

    for ( int n = 0; n < samples; n++ ) {

        x = n % (2 * period);
        value = sin( ((two_pi * n * frequency)  / hz ));

        // Channel 1 has sine wave
        chan_1 = amplitude * value;

        // Channel 2 will have a half circle wave
        chan_2 = sqrt(  pow(2*32760.0, 2) * (1.0 - ( pow(x - (period) ,2) / pow(period,2))) ) - 32760.0;

        if (frequency == 0.0)
            chan_2 = 0.0;

        S.add_sample( (int) (chan_1), (int) (chan_2) ); // Add sample to .wav file

    }
}

//comparator used in the sort which wil compares 2integers
struct myclass {
    bool operator() (int pt1, int pt2) { return (pt1 < pt2);}
} comparator;
//comparator used in the sort which will sort points in vector based on their x-values
struct myclass2 {
    bool operator() (vector<Point> pt1, vector<Point> pt2) {return (pt1.at(0).x < pt2.at(0).x);}
} comparator2;

/**
 * @brief The main function
 * @param argc the amount of arguments given by terminal
 * @param argv array of arguments ;these are auto generated
 * @return 0 if everything is ok; -1 when there is a io-error
 * note: this function can not be called!
 */
int main(int argc, const char** argv) {

    /**
     * @brief function that prints the help message, and parses the user input
     */
    CommandLineParser parser(argc, argv,
                             "{help h usage ?|       | show this message}"
                             "{@input | | Required: (absolute) path to sheet}"
                             "{@audio | | name for the audio output (do not add wav!!)}"
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
    //check if their is a audio name given
    string audio_path(parser.get<string>("@audio"));
    if (audio_path.empty()) {
        cerr << "no audio name given" << endl;
        cerr << "using default \" output_sound \"" <<endl;
        audio_path="output_sound";
    }
    //the original input image
    Mat input = imread(input_path);
    if (input.empty()) {
        cerr << "could not read input; check parameters" << endl;
        parser.printMessage();
        return -1;
    }
    //show the input image
    if (parser.has("debug")) {
        namedWindow("input image", WINDOW_AUTOSIZE);
        imshow("input image", input);
        waitKey(0);
    }
    //create the final image, i chose to do this early because we will write to it during the program
    Mat finaal = Mat::zeros(input.size(),input.type());
    //convert the input image to grayscale
    Mat image_gray;
    cvtColor(input, image_gray, COLOR_BGR2GRAY);

    ////show the gray image
    if (parser.has("debug")) {
        namedWindow("gray image", WINDOW_AUTOSIZE);
        imshow("gray image", image_gray);
        waitKey(0);
    }
    //convert the image to binary (0 or 1 instead of 0 to 255)
    Mat bin(image_gray.size(), image_gray.type());
    //invert the image (we are looking for white notes on black background instead of the opposite)
    image_gray = ~image_gray;
    //threshold the image
    adaptiveThreshold(image_gray, bin, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,15, -2);   //255 as max pixel value, use mean thresholding, blocksize15 and add 2 to mean
    //show the binary image
    if(parser.has("debug"))
    {
        imshow("binary image", bin);
        waitKey(0);
    }
    //here we will try to find the horizontal lines, indicating the staff lines
    Mat notenbalk = bin.clone();    //clone the binary image
    int horizontalsize = notenbalk.cols/25;

    //make a structure containing the 'recangles' width a heigt of 1 (ergo a line)
    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1));
    //morhplogically change the image using the given structure (filtering the lines ot of the image)
    erode(notenbalk,notenbalk,horizontalStructure);
    dilate(notenbalk, notenbalk, horizontalStructure);
    //we now have an image containing the lines
    if(parser.has("debug"))
    {
        imshow("notenbalk", notenbalk);
        waitKey(0);
    }

   //create image to store the horizontals on
    Mat found_horizontals(image_gray.size(), input.type());
    //create a vector for storing the coordinates of the lines
    vector<Vec2f> lines;
    //create a vector for storing the y coordinate of the lines
    vector<int> lijnen;
    //use houghlines to find the lines, use 1000 points to identify a line
    HoughLines(notenbalk, lines, 1, CV_PI/180, 1000,0,0);
    if(parser.has("debug"))
    {
        cout << lines.size() << endl;
    }
    //calculate the coordinates
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
        pt1.y = (int)(y0);
        pt2.x = found_horizontals.cols;
        pt2.y = (int)y0;
        //draw on the final image the found lines
        line( found_horizontals, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
        line(finaal, pt1, pt2, Scalar(255, 255, 255), 1, CV_AA);

        lijnen.push_back((int)y0);
    }
    //show the found lines
    if(parser.has("debug"))
    {
        namedWindow("found_horizontal", WINDOW_AUTOSIZE);   //create a window to display everything
        imshow("found_horizontal", found_horizontals);
        waitKey(0);
    }

    //sort the lines, so we know which line is on top
    sort(lijnen.begin(),lijnen.end(),comparator);
    if(parser.has("debug"))
    {
        for(unsigned long i=0;i<lijnen.size();i++)
        {
            cout << lijnen.at(i) << endl;
        }
    }

    //give a name to the corresponding line to make it easier to understand
    int fa_lijn = lijnen.at(0);
    int re_lijn = lijnen.at(1);
    int si_lijn = lijnen.at(2);
    int sol_lijn = lijnen.at(3);
    int mi_lijn = lijnen.at(4);
    int helft = (fa_lijn-re_lijn)/2;

    //identify individual notes
    int B2 = mi_lijn - 3*helft;
    int C3 = mi_lijn - 2*helft;
    int D3 = mi_lijn - helft;
    int E3 = mi_lijn;
    int F3 = mi_lijn + helft;
    int G3 = sol_lijn;
    int A3 = sol_lijn + helft;
    int B3 = si_lijn;
    int C4 = si_lijn + helft;
    int D4 = re_lijn;
    int E4 = re_lijn + helft;
    int F4 = fa_lijn;
    int G4 = fa_lijn + helft;
    int A4 = fa_lijn + 2*helft;
    if(parser.has("debug"))
    {
        cout << "C3: " << C3 << endl;
        cout << "D3: " << D3 << endl;
        cout << "E3: " << E3 << endl;
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
    }

    //create a copy of the binary image
    Mat noten = bin.clone();
    //we will now strip the vertical lines from the image
    int verticalsize = noten.rows /200;     //200 found by trial and error
    //create a structure containing the recatngles/lines with vertical notes
    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));
    //make sure we see them by first eroding, and then dilating given the structure
    erode(noten, noten, verticalStructure, Point(-1, -1));
    dilate(noten, noten, verticalStructure, Point(-1, -1));
    //show the notes
    if(parser.has("debug"))
    {
        imshow("vertical", noten);
        waitKey(0);
    }

    //find contours in the image

    vector<vector<Point>> contouren;
    //use the external contours, and use only important coordinates
    findContours(noten.clone(), contouren, RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    //there is a lot of jitter in the image because of the title and so on, we will strip the image of the unnessecary info, therefor creating the 2 images
    Mat voor = found_horizontals.clone();
    Mat na = found_horizontals.clone();
    //draw all contours
    drawContours(voor, contouren, -1, Scalar(255,255,255),-1);
    if(parser.has("debug"))
    {
        imshow("noten voor erase", voor);
        waitKey(0);
    }
    //vector of vector of points to store the final contours in
    vector<vector<Point>> after;
    //loop all contours
    for(unsigned long i=0;i<contouren.size();i++)
    {
        //check if the current contour contains an y coordinate not in the region of interest
        if(contouren.at(i).at(0).y<(fa_lijn-30))
        {
            contouren.erase(contouren.begin()+i);   //delete the contour
        }
        else{
            after.push_back(contouren.at(i));   //add teh contour to the after vector
        }
    }
    //sort the contours by x coordinate
    sort(after.begin(),after.end(),comparator2);


    //create the image containg the circles of the notes aka bollen and draw the contours in the region of interest on it
    Mat noten_bollen = Mat::zeros(image_gray.size(), CV_8UC1);
    drawContours(noten_bollen, after, -1, Scalar(255,0,0),-1);
    //create an elipsstructure to store the elipses (bollen) in
    Mat elipsStructure = getStructuringElement(MORPH_ELLIPSE, Size(8,4));   //the bollen are mostly the size of 8 by 4, we will extract them from the image
    erode(noten_bollen, noten_bollen, elipsStructure, Point(-1, -1));
    dilate(noten_bollen, noten_bollen, elipsStructure, Point(-1, -1));
    if(parser.has("debug"))
    {
        imshow("noten_bollen", noten_bollen);
        waitKey(0);
    }


    /**
     * this part is the audio_maker lib
     */
     //generate random number
    srand(static_cast<unsigned int>(time(nullptr))); // NOLINT(cert-msc32-c,cert-msc51-cpp) //this is to supress the clang-tidy check

    // music_map[ note_letter ][ octave 0 - 8 ] == frequency
    std::map< char, double* > music_map;

    // https://pages.mtu.edu/~suits/notefreqs.html
    // Upper-case letters denote sharp notes
    //frequency rqnges of notes
    double c_notes[] = {16.35, 32.70, 65.41, 130.81, 261.63, 523.25, 1046.50, 2093.00, 4186.01};
    music_map['c'] =  c_notes;
    double C_notes[] = {17.32, 34.65, 69.30, 138.59, 277.18, 554.37, 1108.73, 2217.46, 2217.46};
    music_map['C'] = C_notes;
    double d_notes[] = {18.35, 36.71, 73.42, 146.83, 293.66, 587.33, 1174.66, 2349.32, 4698.63};
    music_map['d'] = d_notes;
    double D_notes[] = {19.45, 38.89, 77.78, 155.56, 311.13, 622.25, 1244.51, 2489.02,  4978.03};
    music_map['D'] = D_notes;
    double e_notes[] = {20.60, 41.20, 82.41, 164.81, 329.63, 659.25, 1318.51, 2637.02, 5274.04};
    music_map['e'] = e_notes;
    double f_notes[] = {21.83, 43.65, 87.31, 174.61, 349.23, 698.46, 1396.91, 2793.83, 5587.65};
    music_map['f'] = f_notes;
    double F_notes[] = {23.12, 46.25, 92.50, 185.00, 369.99, 739.99, 1479.98, 2959.96, 5919.91};
    music_map['F'] = F_notes;
    double g_notes[] = {24.50, 49.00, 98.00, 196.00, 392.00, 783.99, 1567.98, 3135.96, 6271.93};
    music_map['g'] = g_notes;
    double G_notes[] = {25.96, 51.91, 103.83, 207.65, 415.30, 830.61, 1661.22, 3322.44, 6644.88};
    music_map['G'] = G_notes;
    double a_notes[] = {27.50, 55.00, 110.00, 220.00, 440.00, 880.00, 1760.00, 3520.00, 7040.00};
    music_map['a'] = a_notes;
    double A_notes[] = {29.14, 58.27, 116.54, 233.08, 466.16, 932.33, 1864.66, 3729.31, 7458.62};
    music_map['A'] = A_notes;
    double b_notes[] = {30.87, 61.74, 123.47, 246.94, 493.88, 987.77, 1975.53, 3951.07, 7902.13};
    music_map['b'] = b_notes;

    // Create file and initialize .wav file headers
    //add .wav to the audio_path
    audio_path+= ".wav";
    SoundMaker S(audio_path);


    //find the coordinates of the noten_bollen
    vector<vector<Point>> noten_bollen_contouren;
    findContours(noten_bollen.clone(), noten_bollen_contouren, RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

    //sort them by x value
    sort(noten_bollen_contouren.begin(),noten_bollen_contouren.end(),comparator2);
    //draw them on the final image
    drawContours(finaal, noten_bollen_contouren, -1, Scalar(255,0,0),-1);


    //make first "sound" with no frequency (needed by soundmaker)
    generate_data(S, 0.0, 0.1);
    /**
     * the following loop will check the contour, and identify which note it is. it will then generate the appropriate sound.
     * if no note is found, it will alter the position and look again (+3 and -3)
     * if then no note is found, it will be printed
     * note: if the distance between the staff lines is greater than 5, you will need to alter the +3 and -3 offzet. this is something i need to implement
     */
    for(unsigned long i=0;i<noten_bollen_contouren.size();i++)
    {
        //calc note
        int pos = (boundingRect(noten_bollen_contouren.at(i)).y + (boundingRect(noten_bollen_contouren.at(i)).y+boundingRect(noten_bollen_contouren.at(i)).height/2))/2;
        int nootgevonden=0;
        int loopcounter=0;
        while(nootgevonden==0)
        {
            if(pos==B2)
            {
                cout << "noot " << i << " B2" <<endl;
                generate_data(S, music_map[ 'b'][2], 1.0); generate_data(S, 0.0, 0.1);
                nootgevonden=1;
            }
            if(pos==C3)
            {
                cout << "noot " << i << " C3" <<endl;
                generate_data(S, music_map[ 'c'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==D3)
            {
                cout << "noot " << i << " D3" <<endl;
                generate_data(S, music_map[ 'd'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==E3)
            {
                cout << "noot " << i << " E3" <<endl;
                generate_data(S, music_map[ 'e'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==F3)
            {
                cout << "noot " << i << " F3" <<endl;
                generate_data(S, music_map[ 'f'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==G3)
            {
                cout << "noot " << i << " G3" <<endl;
                generate_data(S, music_map[ 'g'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==A3)
            {
                cout << "noot " << i << " A3" <<endl;
                generate_data(S, music_map[ 'a'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==B3)
            {
                cout << "noot " << i << " B3" <<endl;
                generate_data(S, music_map[ 'b'][3], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==C4)
            {
                cout << "noot " << i << " C4" <<endl;
                generate_data(S, music_map[ 'c'][4], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==D4)
            {
                cout << "noot " << i << " D4" <<endl;
                generate_data(S, music_map[ 'd'][4], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==E4)
            {
                cout << "noot " << i << " E4" <<endl;
                generate_data(S, music_map[ 'e'][4], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==F4)
            {
                cout << "noot " << i << " F4" <<endl;
                generate_data(S, music_map[ 'f'][4], 1.0); generate_data(S, 0.0, 0.1);


                nootgevonden=1;
            }
            else if(pos==G4)
            {
                cout << "noot " << i << " G4" <<endl;
                generate_data(S, music_map[ 'g'][4], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else if(pos==A4)
            {
                cout << "noot " << i << " A4" <<endl;
                generate_data(S, music_map[ 'a'][4], 1.0); generate_data(S, 0.0, 0.1);

                nootgevonden=1;
            }
            else{
                loopcounter++;
                if(loopcounter<3)
                {
                    pos--;
                } else if(loopcounter==3)
                {
                    pos+=3;
                } else
                {
                    pos++;
                }
                if(loopcounter==5)
                {
                    cerr << "noot " << i << " Niet gevonden" <<endl;
                    nootgevonden=1;
                }
            }


        }

    }
    //generate final 'sound'
    generate_data(S, 0.0, 0.1);

    S.done(); // Final header adjustments and close file
    cout << "audio file written" << endl;


    //extract the lines (do determine the length of the note) this is not implemented yet
    Mat noten_lijnen = Mat::zeros(image_gray.size(), CV_8UC1);
    //do the same thing as always to extract the lines from the note
    drawContours(noten_lijnen, after, -1, Scalar(255,0,0),-1);
    int verticalsize2 = noten_lijnen.rows /50;

    Mat verticalStructure2 = getStructuringElement(MORPH_RECT, Size( 1,verticalsize2));


    erode(noten_lijnen, noten_lijnen, verticalStructure2, Point(-1, -1));
    dilate(noten_lijnen, noten_lijnen, verticalStructure2, Point(-1, -1));

    //store the lines
    vector<vector<Point>> noten_lijnen_contouren;
    findContours(noten_lijnen.clone(), noten_lijnen_contouren, RETR_EXTERNAL,CHAIN_APPROX_NONE);
    //sort the lines
    sort(noten_lijnen_contouren.begin(),noten_lijnen_contouren.end(),comparator2);

    //diffenetiate the end of a maat and a line of a note
    vector<vector<Point>> maatstrepen;
    vector<vector<Point>> nootlijnen;
    for(unsigned long i=0;i<noten_lijnen_contouren.size();i++)
    {
        //if the line goes from the lower line to the top line (with a margin of 2) it is a 'maatstreep'
        if(boundingRect(noten_lijnen_contouren.at(i)).height==(mi_lijn- fa_lijn) or boundingRect(noten_lijnen_contouren.at(i)).height==((mi_lijn-fa_lijn)+1) or boundingRect(noten_lijnen_contouren.at(i)).height==(mi_lijn-fa_lijn+2))
        {
            maatstrepen.push_back(noten_lijnen_contouren.at(i));
        } else{
            //else it is a nootlijn
            nootlijnen.push_back(noten_lijnen_contouren.at(i));
        }
    }
    //draw the nootlijnen en maatstrepen on the final picture
    drawContours(finaal, nootlijnen, -1, Scalar(255,0,255),-1);
    drawContours(finaal, maatstrepen, -1, Scalar(255,255,255),-1);
    if(parser.has("debug"))
    {
        namedWindow("finaal", WINDOW_AUTOSIZE);   //create a window to display everything
        imshow("finaal", finaal);
        waitKey(0);
    }

    return 0;

}


#pragma clang diagnostic pop