/**
 * @file utils.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.ru>
 * @date 14 Jul 2020
 * @copyright MIT License
 *
 * @brief This file contains implementations of the common utility functions
 */

#include "utils.h"

namespace faces {

    std::vector<dlib::point> dPointsVec(std::vector<cv::Point> const &pts) {
        std::vector<dlib::point> res;
        for (cv::Point const &pt : pts) {
            res.emplace_back(pt.x, pt.y);
        }
        return res;
    }

    dlib::rectangle dRect(cv::Rect const &r) {
        return dlib::rectangle((long) r.tl().x, (long) r.tl().y, (long) r.br().x - 1, (long) r.br().y - 1);
    }

    double getDist(cv::Point const &a, cv::Point const &b) {
        return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
    }

}
