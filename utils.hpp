//
// Created by prostoichelovek on 06.06.19.
//

#ifndef VIDEOTRANS_UTILS_HPP
#define VIDEOTRANS_UTILS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <opencv2/core.hpp>

enum LogType {
    INFO, WARNING, ERROR
};

template<typename T>
void coutMany(T t) {
    std::cout << t << " " << std::endl;
}

template<typename T, typename... Args>
void coutMany(T t, Args... args) { // recursive variadic function
    std::cout << t << " " << std::flush;
    coutMany(args...);
}

template<typename T>
void cerrMany(T t) {
    std::cerr << t << " " << std::endl;
}

template<typename T, typename... Args>
void cerrMany(T t, Args... args) { // recursive variadic function
    std::cerr << t << " " << std::flush;
    cerrMany(args...);
}

template<typename T, typename... Args>
void log(int type, T t, Args... args) {
    switch (type) {
        case INFO:
            std::cout << "\033[;32mINFO:\033[0m ";
            coutMany(t, args...);
            break;
        case WARNING:
            std::cout << "\033[1;33mWARNING:\033[0m ";
            coutMany(t, args...);
            break;
        case ERROR:
            std::cerr << "\033[1;31mERROR:\033[0m ";
            cerrMany(t, args...);
            break;
    };
}

inline std::vector<std::string> split(const std::string &str, const std::string &delim) {
    std::vector<std::string> parts;
    size_t start, end = 0;
    while (end < str.size()) {
        start = end;
        while (start < str.size() && (delim.find(str[start]) != std::string::npos)) {
            start++;  // skip initial whitespace
        }
        end = start;
        while (end < str.size() && (delim.find(str[end]) == std::string::npos)) {
            end++; // skip to end of word
        }
        if (end - start != 0) {  // just ignore zero-length strings.
            parts.emplace_back(str, start, end - start);
        }
    }
    return parts;
}


inline bool getFileContent(const std::string &fileName, std::vector<std::string> &vecOfStrs) {
    std::ifstream in(fileName.c_str());
    if (!in) {
        std::cerr << "Cannot open the file: " << fileName << std::endl;
        return false;
    }
    std::string str;
    while (std::getline(in, str)) {
        if (!str.empty())
            vecOfStrs.push_back(str);
    }
    in.close();
    return true;
}

inline bool read_csv(const std::string &filename, std::vector<cv::Mat> &images, std::vector<int> &labels,
                     char separator = ';') {
    std::ifstream file(filename.c_str(), std::ifstream::in);
    if (!file) {
        std::cerr << "No valid input file was given, please check the given filename." << std::endl;
        return false;
    }
    std::string line, path, classlabel;
    while (getline(file, line)) {
        std::stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if (!path.empty() && !classlabel.empty()) {
            images.push_back(cv::imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
    return labels.size() == images.size();
}

inline double getDist(cv::Point a, cv::Point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

#endif //VIDEOTRANS_UTILS_HPP
