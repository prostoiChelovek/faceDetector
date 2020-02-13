#include <iostream>
#include <vector>
#include <map>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include "opencv2/face.hpp"
#include <opencv2/tracking.hpp>

#include "Faces.h"

using namespace cv;
using namespace std;
using namespace cv::dnn;
using namespace cv::face;

const string data_dir = "../data";

const string configFile = data_dir + "/models/deploy.prototxt";
const string weightFile = data_dir + "/models/res10_300x300_ssd_iter_140000_fp16.caffemodel";

string samplesDir = data_dir + "/samples";
string labelsFile = data_dir + "/labels.txt";

string modelFile = data_dir + "/model.yml";

string lmsPredictorFile = data_dir + "/models/shape_predictor_5_face_landmarks.dat";
string descriptorsNetFIle = data_dir + "/models/dlib_face_recognition_resnet_model_v1.dat";
string faceClassifiersFile = data_dir + "/classifiers.dat";

string histValidator = data_dir + "/faceHistCHecker.dat";

int main(int argc, const char **argv) {
    VideoCapture source;
    if (argc == 1)
        source.open(0);
    else
        source.open(stoi(argv[1]));

    Faces::Faces faces(configFile, weightFile, lmsPredictorFile, "",
                       descriptorsNetFIle, faceClassifiersFile, histValidator,
                       labelsFile);

    faces.detectFreq = 3;
    faces.recognition->minLabelNotChanged = 6;
    faces.recognizeFreq = 9;

    // Callbacks ->

    faces.callbacks.newCallback("faceDetected", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face detected", f->getLabel(), f->confidence);
    }), 5, true);

    faces.callbacks.newCallback("faceRecognized", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face recognized", f->getLabel(), f->confidence);
    }), 35, true);

    faces.callbacks.newCallback("unknownFace", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Unknown face!", f->getLabel(), f->confidence);
    }), 35, true);

    faces.callbacks.newCallback("labelConfirmed", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face label confirmed", f->getLabel(), f->confidence);
    }));

    /*faces.callbacks.newCallback("faceMoved", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face moved", f->getLabel(), f->confidence, f->offset);
    }), 1);*/

    // <- Callbacks

    namedWindow("Face Detection");
    int detectTh = 70;
    auto detectThCb = [](int pos, void *data) {
        static_cast<Faces::Detection *>(data)->confidenceThreshold = float(pos) / 100;
    };
    createTrackbar("Detection thresh", "Face Detection", &detectTh, 100, detectThCb, &faces.detector);
    int recTh = 30;
    faces.recognition->setThreshold(double(recTh) / 100.0);
    auto recThCb = [](int pos, void *data) {
        static_cast<Faces::Recognition::recognition *>(data)->setThreshold(double(pos) / 100.0);
    };
    createTrackbar("recognition thresh", "Face Detection", &recTh, 200, recThCb, faces.recognition);

    Mat img, frame;

    int nFrames = 0;
    int fps = 0;
    thread([&]() {
        while (true) {
            fps = nFrames;
            nFrames = 0;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }).detach();

    bool shouldRecDir = true;
    while (source.isOpened()) {
        source >> frame;
        if (frame.empty())
            break;
        frame.copyTo(img);

        faces(img);

        faces.draw(img);
        putText(img, format("FPS = %i", fps), Point(5, 25), FONT_HERSHEY_SIMPLEX, 0.8,
                Scalar(0, 0, 255), 2);
        imshow("Face Detection", img);

        faces.update();
        nFrames++;

        int k = waitKey(1);
        if (k == 'l') {
            std::cout << "Label: ";
            std::string lbl;
            std::getline(std::cin, lbl);
            faces.recognition->addLabel(lbl);
            log(INFO, "Label", lbl, "added with index", faces.recognition->labels.size() - 1);
        }
        if (k == 'n') {
            if (faces.recognition->currentLabel < faces.recognition->labels.size() - 1)
                faces.recognition->currentLabel++;
            else
                faces.recognition->currentLabel = 0;
            log(INFO, "Current label is", faces.recognition->currentLabel, "-",
                faces.recognition->labels[faces.recognition->currentLabel]);
        }
        if (k == 'p') {
            if (faces.recognition->currentLabel > 0)
                faces.recognition->currentLabel--;
            else
                faces.recognition->currentLabel = faces.recognition->labels.size() - 1;
            log(INFO, "Current label is", faces.recognition->currentLabel, "-",
                faces.recognition->labels[faces.recognition->currentLabel]);
        }
        if (k == 's') {
            if (!faces.recognition->labels.empty()) {
                if (!faces.detector.faces.empty()) {
                    string path = faces.recognition->addSample(samplesDir, faces.detector.faces[0]);
                    log(INFO, "Detection train image", faces.recognition->imgNum[faces.recognition->currentLabel],
                        "saved to", path);
                } else log(ERROR, "There is no faces");
            } else log(ERROR, "Labels are empty! Press 'l' to add new");
        }
        if (k == 't') {
            faces.recognition->train(samplesDir);
            faces.recognition->save(faceClassifiersFile);
            log(INFO, "recognition model trained");
        }
        if (k == 'd') {
            shouldRecDir = !shouldRecDir;
            log(INFO, "Should", (shouldRecDir ? "" : " not"), "recognize faces` directions");
        }
        if (k == 'f') {
            Mat disp = imread(data_dir + "/faceDisp.png", IMREAD_GRAYSCALE);
            Mat disp2 = imread(data_dir + "/faceDisp2.png", IMREAD_GRAYSCALE);
            faces.checker.train(samplesDir);
            faces.checker.save();
            faces.checker.check(disp);
            faces.checker.check(disp2);
        }
        if (k == 27) {
            destroyAllWindows();
            break;
        }
    }
}
