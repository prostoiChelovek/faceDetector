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

        PointDistancesT allDistances = _getDistances(prevCentroids, actualCentroids);
        for (std::size_t i = 0; i < allDistances.size(); ++i) {
            std::vector<double> &distances = allDistances[i];

            auto minDistance = std::min_element(distances.begin(), distances.end());
            int matchingIdx = -1;
            if (minDistance != distances.end() && *minDistance != -1) {
                matchingIdx = std::distance(distances.begin(), minDistance);
                *minDistance = -1;
                actualCentroids[matchingIdx] = {-1, -1};
            }
            res.emplace_back(i, matchingIdx);
        }

        for (std::size_t i = 0; i < actualCentroids.size(); ++i) {
            if (actualCentroids[i] != cv::Point(-1, -1)) {
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

    CentroidTracker::PointDistancesT CentroidTracker::_getDistances(std::vector<cv::Point> const &a,
                                                                    std::vector<cv::Point> const &b) {
        PointDistancesT res;
        for (const auto &firstPt : a) {
            res.emplace_back(std::vector<double>{});
            for (auto const &secondPt : b) {
                double distance = getDist(firstPt, secondPt);
                res.back().emplace_back(distance);
            }
        }
        return res;
    }


}