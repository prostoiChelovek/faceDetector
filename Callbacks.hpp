//
// Created by prostoichelovek on 21.07.19.
//

#ifndef FACES_CALLBACKS_HPP
#define FACES_CALLBACKS_HPP


#include <string>
#include <thread>
#include <functional>
#include <utility>
#include <memory>
#include <map>

#include "Face/Face.h"

namespace Faces {


    using CallbackFn = std::function<void(Face * )>;


    class Callback {
    public:

        CallbackFn callback;
        int numFrames = -1;
        bool once = false;
        std::string name;

        Callback() = default;

        explicit Callback(CallbackFn callback, std::string name, int numFrames = -1, bool once = false)
                : callback(std::move(callback)), name(std::move(name)), numFrames(numFrames), once(once) {}

        void operator()(Face *f) {
            if (!callback)
                return;

            if (f->executedCallbacks.count(name)) {
                if (once && f->executedCallbacks[name] == -1)
                    return;

                if (f->executedCallbacks[name] < numFrames && numFrames != -1) {
                    f->executedCallbacks[name]++;
                    return;
                }
            } else {
                f->executedCallbacks[name] = 1;
                if (numFrames != -1)
                    return;
            }

            std::thread(callback, f).detach();
            f->executedCallbacks[name] = -1;
        }

    };


    class Callbacks {
    public:

        Callbacks() = default;

        void newCallback(const std::string &name, const CallbackFn &cb, int numFrames = -1, bool once = false) {
            callbacks[name] = Callback(cb, name, numFrames, once);
        }

        void call(const std::string &name, Face *f) {
            if (callbacks.count(name) > 0) {
                callbacks[name](f);
            }
        }

    private:
        std::map<std::string, Callback> callbacks;

    };


}


#endif //FACES_CALLBACKS_HPP
