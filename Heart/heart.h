#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#include <iostream>
#include <random>

#define WIDTH 640
#define HEIGHT 480
#define IMAGE_ENLARGE 11
#define CENTER_X WIDTH / 2
#define CENTER_Y HEIGHT / 2
#define PI 3.1415926

class Heart {
public:
    Heart(cv::Scalar co = cv::Scalar(203, 192, 255));
    void build(int number);
    void draw(cv::Mat &image, int time_interval);
    void clear();
    std::pair<int, int> heart_function(double t);
    cv::Point2d scatter_inside(const double &x, const double &y, double beta = 0.05);
    cv::Point2d cal_position(const double &x, const double &y, double ratio);
    cv::Point2d shrink(const double& x,const double& y, int ratio);
    void calc(int frame);
    void render(cv::Mat& image, int frame);
    double curve(const double& p);
private:
    cv::Scalar color;
    // point array
    std::vector<cv::Point2d> _points;
    std::vector<cv::Point2d> _ex_points;
    std::vector<cv::Point2d> _inside;
    std::vector<std::vector<cv::Point3d>> _all_points;
    std::default_random_engine random;
    std::uniform_real_distribution<double> randNum1;
    std::uniform_real_distribution<double> randNum2;
};


class hash_pair {
public:
    size_t operator()(const std::pair<int, int>& pr)const {
        std::hash<int> hs;
        int p1 = 53;
        int p2 = 53;
        auto v = hs((p1 + pr.first) * p2 + pr.second);
        return v;
    }
};