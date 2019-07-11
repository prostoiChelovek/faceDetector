//
// Created by prostoichelovek on 06.06.19.
//

#ifndef VIDEOTRANS_UTILS_HPP
#define VIDEOTRANS_UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

enum LogType {
    INFO, WARNING, ERROR
};

template<typename T>
void coutMany(T t) {
    cout << t << " " << endl;
}

template<typename T, typename... Args>
void coutMany(T t, Args... args) { // recursive variadic function
    cout << t << " " << flush;
    coutMany(args...);
}

template<typename T>
void cerrMany(T t) {
    cerr << t << " " << endl;
}

template<typename T, typename... Args>
void cerrMany(T t, Args... args) { // recursive variadic function
    cerr << t << " " << flush;
    cerrMany(args...);
}

template<typename T, typename... Args>
void log(int type, T t, Args... args) {
    switch (type) {
        case INFO:
            cout << "\033[;32mINFO:\033[0m ";
            coutMany(t, args...);
            break;
        case WARNING:
            cout << "\033[1;33mWARNING:\033[0m ";
            coutMany(t, args...);
            break;
        case ERROR:
            cerr << "\033[1;31mERROR:\033[0m ";
            cerrMany(t, args...);
            break;
    };
}

#endif //VIDEOTRANS_UTILS_HPP
