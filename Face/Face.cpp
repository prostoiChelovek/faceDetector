//
// Created by prostoichelovek on 21.07.19.
//

#include "Face.h"

namespace Faces {

    Face::Face(cv::Point p1, cv::Point p2, int label) : label(label) {
        rect = cv::Rect(p1, p2);
    }

    bool Face::checkBounds(const cv::Size &imgSize) {
        if (rect.width <= 0 || rect.height <= 0)
            return false;
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        if (rect.x >= imgSize.width)
            rect.x = imgSize.width - rect.width;
        if (rect.y >= imgSize.height)
            rect.y = imgSize.height - rect.height;
        if (rect.x + rect.width > imgSize.width)
            rect.width = imgSize.width - rect.x;
        if (rect.y + rect.height > imgSize.height)
            rect.height = imgSize.height - rect.y;
        if (rect.width > imgSize.width)
            rect.width = imgSize.width;
        if (rect.height > imgSize.height)
            rect.height = imgSize.height;
        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        return true;
    }

    void Face::setLast(Face *f) {
        last = f;
        if (last != nullptr) {
            label = last->label;
            labelNotChanged = last->labelNotChanged;
            executedCallbacks = last->executedCallbacks;
            offset.x = rect.x - last->rect.x;
            offset.y = rect.y - last->rect.y;
            descriptor = last->descriptor;
        }
    }

    void Face::setLabel(int lbl) {
        label = lbl;
        if (last != nullptr) {
            if (label == last->label)
                labelNotChanged = last->labelNotChanged + 1;
            else
                labelNotChanged = 0;
        }
    }

    int Face::getLabel() const {
        if (label == -3)
            return label;
        if (labelNotChanged >= minLabelNotChanged)
            return label;
        else
            return -2;
    }

    bool Face::hasMoved() const {
        return offset.x >= minOffset || offset.y >= minOffset
               || -offset.x >= minOffset || -offset.y >= minOffset;
    }

    cv::Scalar Face::draw(cv::Mat &img, const std::vector<std::string> *labels,
                          const cv::Scalar &color) const {
        // Color ->
        cv::Scalar clr = cv::Scalar(0, 255, 0);
        if (color == cv::Scalar(-1)) {
            if (getLabel() == -1)
                clr = cv::Scalar(0, 0, 255);
            if (getLabel() == -2)
                clr = cv::Scalar(0, 125, 255);
            if (getLabel() == -3)
                clr = cv::Scalar(255, 0, 0);
            if (getLabel() == -4)
                clr = cv::Scalar(255, 0, 255);
        } else {
            clr = color;
        }
        // <- Color

        cv::rectangle(img, rect, clr, 2, 4);

        // Label ->
        std::string text = std::to_string(confidence);
        if (labels) {
            if (getLabel() > -1) {
                std::string label = getLabel() < labels->size()
                                    ? labels->at(getLabel()) : "WTF";
                text += " - " + label;
            }
            std::for_each(text.begin(), text.end(),
                          [](char &ch) { ch = ::toupper(static_cast<unsigned char>(ch)); });
        }

        cv::rectangle(img, cv::Rect(rect.x, rect.y - 20, rect.width, 25),
                      clr, cv::FILLED, 4);
        cv::putText(img, text, rect.tl(),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        // <- Label

        // Moving direction ->
        auto drawCrn = [&](int x, int y, int width, int height) {
            cv::Point start(x, y);
            cv::Point end = start;
            if (width != 0)
                end.x += width;
            if (height != 0)
                end.y += height;
            cv::line(img, start, end, cv::Scalar(255, 255, 255), 3);
        };

        // Handle transverse directions(like right-up)
        // ie when moving in one direction - draw it on center
        // but when moving in two direction - draw them on corner
        int h = rect.height / 2;
        int startY = rect.y + (rect.height / 4);
        if (-offset.y >= minOffset) // up
            startY = rect.y - 20;
        if (offset.y >= minOffset) { // down
            h = -h;
            startY = rect.br().y;
        }
        int w = rect.width / 2;
        int startX = rect.x + (rect.width / 4);
        if (offset.x >= minOffset) { // right
            w = -w;
            startX = rect.br().x;
        }
        if (-offset.x >= minOffset) // left
            startX = rect.x;

        if (offset.x >= minOffset) // right
            drawCrn(rect.br().x, startY, 0, h);
        if (-offset.x >= minOffset) // left
            drawCrn(rect.x, startY, 0, h);
        if (-offset.y >= minOffset) // up
            drawCrn(startX, rect.y - 20, w, 0);
        if (offset.y >= minOffset) // down
            drawCrn(startX, rect.br().y, w, 0);
        // <- Moving direction

        // Landmarks ->
        for (const cv::Point &lm : landmarks) {
            cv::circle(img, lm, 1, clr);
        }
        // <- Landmarks

        return clr;
    }

    bool Face::operator==(const Face &f) {
        return std::tie(rect, offset, labelNotChanged, confidence)
               == std::tie(f.rect, f.offset, f.labelNotChanged, f.confidence);
    }

    bool Face::operator!=(const Face &f) {
        return !operator==(f);
    }

}