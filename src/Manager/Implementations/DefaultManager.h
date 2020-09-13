/**
 * @file DefaultManager.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 30 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DEFAULTMANAGER_H
#define FACES_DEFAULTMANAGER_H

#include <random>

#include <Manager/Manager.hpp>
#include <Config/Config.h>

namespace faces {

    class DefaultManager : public Manager {
    public:
        inline static int handleNewFaceAfter = 15;

        NewFaceHandlerBase *newFaceHandler;

        explicit DefaultManager(Config const &config, NewFaceHandlerBase *newFaceHandler = nullptr);

    protected:
        void _processNewFace(Face &face) override;

        void _processLostFace(Face &face) override;

        void _processTrackedFace(Face &prevFace, Face &actualFace) override;

    private:
        std::map<int, int> _framesCount;

        std::mt19937 _random;
        std::uniform_int_distribution<int> _randomDistribution;

        [[nodiscard]] int _getTempLabel();

    };

}

#endif //FACES_DEFAULTMANAGER_H
