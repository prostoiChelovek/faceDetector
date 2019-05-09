#include <iostream>
#include <vector>
#include <map>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include "opencv2/face.hpp"
#include <opencv2/tracking.hpp>

#include "FaceRecognizer.h"

using namespace cv;
using namespace std;
using namespace cv::dnn;
using namespace cv::face;

Size capSize;

const string caffeConfigFile = "../deploy.prototxt";
const string caffeWeightFile = "../res10_300x300_ssd_iter_140000_fp16.caffemodel";

string imgsDir = "../images";
string imgsList = "../faces.csv";
string labelsFile = "../labels.txt";

string modelFile = "../model.yml";


int main(int argc, const char **argv) {
    VideoCapture source;
    if (argc == 1)
        source.open(0);
    else
        source.open(argv[1]);

    capSize = Size(source.get(CAP_PROP_FRAME_WIDTH), source.get(CAP_PROP_FRAME_HEIGHT));

    FaceRecognizer::FaceRecognizer recognizer(capSize);

    if (!recognizer.readNet(caffeConfigFile, caffeWeightFile))
        return EXIT_FAILURE;
    if (!recognizer.readRecognitionModel(modelFile))
        return EXIT_FAILURE;
    if (!recognizer.readLabels(labelsFile))
        return EXIT_FAILURE;
    if (!recognizer.readImageList(imgsList))
        return EXIT_FAILURE;

    Mat img, frame;

    double tt_opencvDNN = 0;
    double fpsOpencvDNN = 0;

    bool shouldPredict = false;
    bool shouldRecDir = false;
    while (source.isOpened()) {
        source >> frame;
        if (frame.empty())
            break;
        frame.copyTo(img);
        double t = getTickCount();

        recognizer.detectFaces(frame);
        recognizer.draw(img);

        tt_opencvDNN = ((double) getTickCount() - t) / getTickFrequency();
        fpsOpencvDNN = 1 / tt_opencvDNN;
        putText(img, format("FPS = %.2f", fpsOpencvDNN), Point(5, 25), FONT_HERSHEY_SIMPLEX, 0.8,
                Scalar(0, 0, 255), 2);
        imshow("Face Detection", img);

        int k = waitKey(5);
        if (k == 'l') {
            string label = recognizer.addLabel();
            printf("Label %s added with index %i\n", label.c_str(), recognizer.labels.size() - 1);
        }
        if (k == 'n') {
            if (recognizer.currentLabel < recognizer.labels.size() - 1)
                recognizer.currentLabel++;
            else
                recognizer.currentLabel = 0;
            printf("Current label is %i - %s\n", recognizer.currentLabel,
                   recognizer.labels[recognizer.currentLabel].c_str());
        }
        if (k == 'p') {
            if (recognizer.currentLabel > 0)
                recognizer.currentLabel--;
            else
                recognizer.currentLabel = recognizer.labels.size() - 1;
            printf("Current label is %i - %s\n", recognizer.currentLabel,
                   recognizer.labels[recognizer.currentLabel].c_str());
        }
        if (k == 's') {
            if (!recognizer.labels.empty()) {
                if (!recognizer.faces.empty()) {
                    Mat f = frame(recognizer.faces[0].rect);
                    string path = recognizer.addTrainImage(imgsDir, f);
                    printf("Recognizer train image %i saved to %s\n",
                           recognizer.imgNum[recognizer.currentLabel], path.c_str());
                } else cerr << "There is no faces" << endl;
            } else cerr << "Labels are empty! Press 'l' to add new" << endl;
        }
        if (k == 't') {
            bool ok = recognizer.trainRecognizer(imgsList, modelFile);
            cout << "Model" << (ok ? "" : " not") << " trained" << endl;
        }
        if (k == 'd') {
            shouldRecDir = !shouldRecDir;
            cout << "Should" << (shouldRecDir ? "" : " not") << " recognize faces` directions" << endl;
        }
        if (k == 27) {
            destroyAllWindows();
            break;
        }
    }
}