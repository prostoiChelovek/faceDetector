/**
 * @file CentroidTracker.cpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 16 Aug 2020
 * @copyright MIT License
 */

#include <numeric>
#include "CentroidTracker.h"

namespace faces {

    CentroidTracker::CentroidTracker(Config const &config) {
        _ok = true;

        try {
            _maxDistance = config["CentroidTracker.maxDistance"].getInt();
        } catch (std::out_of_range &e) {
            spdlog::error("Cannot get am option 'CentroidTracker.maxDistance' in the config!");
            _maxDistance = INT_MAX;
        }

    }

    std::vector<std::pair<int, int>> CentroidTracker::_track(std::vector<Face> const &prevFaces,
                                                             std::vector<Face> const &actualFaces,
                                                             cv::Mat const &prevImg,
                                                             cv::Mat const &actualImg) {
        std::vector<std::pair<int, int>> res;

        // index of the point from actualCentroids: {index of the nearest point from prevCentroids,
        //                                           distance between that points}
        std::map<std::size_t, std::pair<std::size_t, double>> candidates;

        std::vector<cv::Point> prevCentroids = _getCentroids(prevFaces);
        std::vector<cv::Point> actualCentroids = _getCentroids(actualFaces);

        // generating candidate points from prevCentroids for the points from actualCentroids
        for (std::size_t i = 0; i < prevCentroids.size(); ++i) {
            // distances from the current point (from prevCentroids) to all of the points from actualCentroids
            std::vector<double> distances;
            std::transform(actualCentroids.begin(), actualCentroids.end(), std::back_inserter(distances),
                           [&](cv::Point &pt) -> double { return getDist(prevCentroids[i], pt); });

            // smallest distance to the point from actualCentroids
            auto minDistance = std::min_element(distances.begin(), distances.end());
            if (!distances.empty()) { // is there are any points in actualCentroids -> is *minDistance valid
                int idx = std::distance(distances.begin(), minDistance);

                decltype(candidates)::iterator candidateItr;
                // is there a already candidate for the found nearest point
                // and the distance to the current point is smaller than candidate's
                bool isFoundAndGreater = (candidateItr = candidates.find(idx)) != candidates.end()
                                         && *minDistance < candidateItr->second.second;
                if (*minDistance < _maxDistance && (isFoundAndGreater || candidateItr == candidates.end())) {
                    auto &candidate = candidates[idx];
                    candidate.first = i;
                    candidate.second = *minDistance;
                }
            }
        }

        // adding best candidates to the resulting vector
        for (auto const &candidate : candidates) {
            std::size_t const &actualIdx = candidate.first;
            std::size_t const &prevIdx = candidate.second.first;
            res.emplace_back(prevIdx, actualIdx);

            // marking points as matched
            prevCentroids[prevIdx].x = -1;
            actualCentroids[actualIdx].x = -1;
        }

        // checking non-matched points
        for (std::size_t i = 0; i < prevCentroids.size(); ++i) {
            if (prevCentroids[i].x != -1) {
                res.emplace_back(i, -1);
            }
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