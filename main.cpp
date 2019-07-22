#include <iostream>
#include <vector>
#include <map>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include "opencv2/face.hpp"
#include <opencv2/tracking.hpp>

#include "Recognizer.h"

using namespace cv;
using namespace std;
using namespace cv::dnn;
using namespace cv::face;

const string configFile = "../models/deploy.prototxt";
const string weightFile = "../models/res10_300x300_ssd_iter_140000_fp16.caffemodel";

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

    Faces::Recognizer recognizer;

    if (!recognizer.readNet(configFile, weightFile)) {
        log(ERROR, "Cannot read face detection network");
        return EXIT_FAILURE;
    }
    if (!recognizer.readRecognitionModel(modelFile)) {
        log(WARNING, "Could not load face recognition model", modelFile);
    }
    if (!recognizer.readLabels(labelsFile)) {
        log(ERROR, "Cannot read labels file");
        return EXIT_FAILURE;
    }
    if (!recognizer.readImageList(imgsList)) {
        log(ERROR, "Cannot read images list");
        return EXIT_FAILURE;
    }

    // Callbacks ->

    recognizer.callbacks.newCallback("faceDetected", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face detected", f->getLabel(), f->confidence);
    }), 5, true);

    recognizer.callbacks.newCallback("faceRecognized", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face recognized", f->getLabel(), f->confidence);
    }), 35, true);

    recognizer.callbacks.newCallback("unknownFace", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Unknown face!", f->getLabel(), f->confidence);
    }), 35, true);

    recognizer.callbacks.newCallback("labelConfirmed", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face label confirmed", f->getLabel(), f->confidence);
    }));

    /*recognizer.callbacks.newCallback("faceMoved", Faces::CallbackFn([](Faces::Face *f) {
        log(INFO, "Face moved", f->getLabel(), f->confidence, f->offset);
    }), 1);*/

    // <- Callbacks

    namedWindow("Face Detection");
    int detectTh = 70;
    auto detectThCb = [](int pos, void *data) {
        static_cast<Faces::Recognizer *>(data)->confidenceThreshold = float(pos) / 100;
    };
    createTrackbar("Detection thresh", "Face Detection", &detectTh, 100, detectThCb, &recognizer);
    int recTh = 70;
    recognizer.model->setThreshold(recTh);
    auto recThCb = [](int pos, void *data) {
        static_cast<Faces::Recognizer *>(data)->model->setThreshold(pos);
    };
    createTrackbar("Recognition thresh", "Face Detection", &recTh, 200, recThCb, &recognizer);

    Mat img, frame;

    double tt_opencvDNN = 0;
    double fpsOpencvDNN = 0;

    bool shouldRecDir = false;
    while (source.isOpened()) {
        source >> frame;
        if (frame.empty())
            break;
        frame.copyTo(img);
        double t = getTickCount();

        recognizer(frame);
        recognizer.draw(img);

        tt_opencvDNN = ((double) getTickCount() - t) / getTickFrequency();
        fpsOpencvDNN = 1 / tt_opencvDNN;
        putText(img, format("FPS = %.2f", fpsOpencvDNN), Point(5, 25), FONT_HERSHEY_SIMPLEX, 0.8,
                Scalar(0, 0, 255), 2);
        imshow("Face Detection", img);

        recognizer.lastFaces = recognizer.faces;

        int k = waitKey(1);
        if (k == 'l') {
            std::cout << "Label: ";
            std::string lbl;
            std::getline(std::cin, lbl);
            recognizer.addLabel(lbl);
            log(INFO, "Label", lbl, "added with index", recognizer.labels.size() - 1);
        }
        if (k == 'n') {
            if (recognizer.currentLabel < recognizer.labels.size() - 1)
                recognizer.currentLabel++;
            else
                recognizer.currentLabel = 0;
            log(INFO, "Current label is", recognizer.currentLabel, "-",
                recognizer.labels[recognizer.currentLabel]);
        }
        if (k == 'p') {
            if (recognizer.currentLabel > 0)
                recognizer.currentLabel--;
            else
                recognizer.currentLabel = recognizer.labels.size() - 1;
            log(INFO, "Current label is", recognizer.currentLabel, "-",
                recognizer.labels[recognizer.currentLabel]);
        }
        if (k == 's') {
            if (!recognizer.labels.empty()) {
                if (!recognizer.faces.empty()) {
                    Mat f = frame(recognizer.faces[0].rect);
                    string path = recognizer.addTrainImage(imgsDir, f);
                    log(INFO, "Recognizer train image", recognizer.imgNum[recognizer.currentLabel],
                        "saved to", path);
                } else log(ERROR, "There is no faces");
            } else log(ERROR, "Labels are empty! Press 'l' to add new");
        }
        if (k == 't') {
            bool ok = recognizer.trainRecognizer(imgsList, modelFile);
            if (ok)
                log(INFO, "Model trained successful");
            else
                log(WARNING, "Model has not been trained");
        }
        if (k == 'd') {
            shouldRecDir = !shouldRecDir;
            log(INFO, "Should", (shouldRecDir ? "" : " not"), "recognize faces` directions");
        }
        if (k == 27) {
            destroyAllWindows();
            break;
        }
    }
}