/**
 * @file CentroidTracker.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 16 Aug 2020
 * @copyright MIT License
 *
 * @brief This file contains a centroid tracker
 */

#ifndef FACES_CENTROIDTRACKER_H
#define FACES_CENTROIDTRACKER_H

#include <utils/utils.h>

#include <Tracker/Tracker.hpp>

namespace faces {

    /**
     * A nearest centroid tracker, which matches face to the one with the nearest center point
     */
    class CentroidTracker : public Tracker {
    public:
        FACES_MAIN_CONSTRUCTOR(CentroidTracker);

    protected:
        using PointDistancesT = std::vector<std::vector<double>>;

        std::vector<std::pair<int, int>> _track(std::vector<Face> const &prevFaces,
                                                std::vector<Face> const &actualFaces,
                                                cv::Mat const &prevImg, cv::Mat const &actualImg) override;

        /**
         * @return a vector of center points of the given faces
         */
        [[nodiscard]] static std::vector<cv::Point> _getCentroids(std::vector<Face> const &faces);

        /**
         * @return distances from point in vector 'a' to all points from 'b'
         */
        [[nodiscard]] static PointDistancesT _getDistances(std::vector<cv::Point> const &a,
                                                           std::vector<cv::Point> const &b);
    };

    FACES_REGISTER_SUBCLASS(Tracker, CentroidTracker, Centroid)

}


#endif //FACES_CENTROIDTRACKER_H
