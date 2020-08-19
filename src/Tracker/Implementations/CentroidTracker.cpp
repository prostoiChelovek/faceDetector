/**
 * @file CentroidTracker.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 16 Aug 2020
 * @copyright MIT License
 */

#include "CentroidTracker.h"

namespace faces {

    CentroidTracker::CentroidTracker() {
        _ok = true;
    }

    std::vector<std::pair<int, int>> CentroidTracker::_track(std::vector<Face> const &prevFaces,
                                                             std::vector<Face> const &actualFaces,
                                                             cv::Mat const &prevImg,
                                                             cv::Mat const &actualImg) {
        std::vector<std::pair<int, int>> res;

        std::vector<cv::Point> prevCentroids = _getCentroids(prevFaces);
        std::vector<cv::Point> actualCentroids = _getCentroids(actualFaces);

        for (std::size_t i = 0; i < prevCentroids.size(); ++i) {
            std::vector<double> distances;
            std::transform(actualCentroids.begin(), actualCentroids.end(), std::back_inserter(distances),
                           [&](cv::Point &pt) -> double { return getDist(prevCentroids[i], pt); });

            auto minDistance = std::min_element(distances.begin(), distances.end());
            int matchingIdx = -1;
            if (!distances.empty()) {
                int idx = std::distance(distances.begin(), minDistance);
                if (actualCentroids[idx].x != -1) {
                    matchingIdx = idx;
                    actualCentroids[matchingIdx].x = -1;
                }
            }
            res.emplace_back(i, matchingIdx);
        }

        for (std::size_t i = 0; i < actualCentroids.size(); ++i) {
            if (actualCentroids[i].x != -1) {
                res.emplace_back(-1, i);
            }
        }

        return res;
    }

    std::vector<cv::Point> CentroidTracker::_getCentroids(std::vector<Face> const &faces) {
        std::vector<cv::Point> res;
        std::transform(faces.begin(), faces.end(), std::back_inserter(res),
                       [](Face const &face) -> cv::Point { return (face.rect.br() + face.rect.tl()) / 2; });
        return res;
    }


}