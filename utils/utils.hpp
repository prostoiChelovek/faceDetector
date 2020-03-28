//
// Created by prostoichelovek on 06.06.19.
//

#ifndef FACES_UTILS_HPP
#define FACES_UTILS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <dirent.h>
#include <bits/stdc++.h>

#include <opencv2/core.hpp>

#include <dlib/opencv.h>

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

inline bool getFileContent(std::fstream &fs, std::vector<std::string> &vecOfStrs) {
    std::string str;
    while (std::getline(fs, str)) {
        if (!str.empty())
            vecOfStrs.push_back(str);
    }
    return true;
}

inline bool getFileContent(const std::string &fileName, std::vector<std::string> &vecOfStrs) {
    std::fstream in(fileName.c_str(), std::ios::in);
    if (!in) {
        std::cerr << "Cannot open the file: " << fileName << std::endl;
        return false;
    }
    getFileContent(in, vecOfStrs);
    in.close();
    return true;
}

template<typename T>
inline std::vector<std::vector<T>> read_csv(const std::string &filename,
                                            char separator = ',') {
    std::ifstream file(filename.c_str(), std::ifstream::in);
    if (!file) {
        log(ERROR, "Cannot open CSV file", filename);
        return std::vector<std::vector<T>>{};
    }

    std::vector<std::vector<T>> res;

    std::string line;

    while (getline(file, line)) {
        std::vector<T> ln;
        std::vector<std::string> vals = split(line, std::string(1, separator));
        for (std::string &v : vals) {
            std::stringstream ss(v);
            T val;
            ss >> val;
            if (ss.fail())
                log(ERROR, "Cannot convert '", ss.str(), "' to required type while reading csv file", filename);
            ln.emplace_back(val);
        }
        res.emplace_back(ln);
    }

    return res;
}

inline double getDist(cv::Point a, cv::Point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

inline std::vector<std::string> list_directory(const std::string &name, const std::string &ext = "") {
    std::vector<std::string> v;
    DIR *dirp = opendir(name.c_str());
    struct dirent *dp;
    while ((dp = readdir(dirp)) != nullptr) {
        std::string n = dp->d_name;
        if (!ext.empty() && n.find("." + ext) == std::string::npos)
            continue;
        else
            v.push_back(n);
    }
    closedir(dirp);
    return v;
}

inline bool createDirNotExists(const std::string &name) {
    struct stat st{};
    if (stat(name.c_str(), &st) == 0) {
        if (st.st_mode & S_IFDIR != 0)
            return false;
    }

    if (mkdir(name.c_str(), 0777) == -1) {
        log(ERROR, "Cannot create directory", name, ":", strerror(errno));
        return false;
    }

    return true;
}

static void rotatedRect(cv::Mat &img, const cv::RotatedRect &rect, const cv::Scalar &color) {
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(img, vertices[i], vertices[(i + 1) % 4], color, 2);
}

template<typename _Tp>
static cv::Mat toMat(const std::vector<std::vector<_Tp> > vecIn) {
    cv::Mat_<_Tp> matOut(vecIn.size(), vecIn.at(0).size());
    for (int i = 0; i < matOut.rows; ++i) {
        for (int j = 0; j < matOut.cols; ++j) {
            matOut(i, j) = vecIn.at(i).at(j);
        }
    }
    return matOut;
}

static cv::Rect boundingBox(std::vector<cv::Rect> rects) {
    cv::Rect res(-1, -1, -1, -1);

    for (cv::Rect &r : rects) {
        if (res.x < r.x)
            res.x = r.x;
        if (res.y < r.y)
            res.y = r.y;
        if (res.width + res.x < r.x + r.width)
            res.width = r.x + r.width - res.x;
        if (res.height + res.y < r.y + r.height)
            res.height = r.y + r.height - res.y;
    }

    return res;
}

static bool fixROI(cv::Rect &rect, const cv::Size &roi) {
    bool ok = true;
    if (rect.width <= 0) {
        ok = false;
        rect.width = roi.width;
    }
    if (rect.height <= 0) {
        ok = false;
        rect.height = roi.height;
    }
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    if (rect.x >= roi.width)
        rect.x = roi.width - rect.width;
    if (rect.y >= roi.height)
        rect.y = roi.height - rect.height;
    if (rect.x + rect.width > roi.width)
        rect.width = roi.width - rect.x;
    if (rect.y + rect.height > roi.height)
        rect.height = roi.height - rect.y;
    if (rect.width > roi.width)
        rect.width = roi.width;
    if (rect.height > roi.height)
        rect.height = roi.height;
    if (rect.x < 0) rect.x = 0;
    if (rect.y < 0) rect.y = 0;
    return ok;
}

static dlib::rectangle openCVRectToDlib(const cv::Rect &r) {
    return dlib::rectangle((long) r.tl().x, (long) r.tl().y, (long) r.br().x - 1, (long) r.br().y - 1);
}

static cv::Rect dlibRect2cv(dlib::rectangle r) {
    return cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1));
}

template<typename T>
cv::Mat dlibMatrix2cvMat(dlib::matrix<T> matr) {
    cv::Mat mat = toMat(matr);
    cv::Mat bgr;
    cvtColor(mat, bgr, cv::COLOR_RGB2BGR);
    return bgr;
}


#endif //FACES_UTILS_HPP
