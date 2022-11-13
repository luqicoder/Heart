#include "heart.h"
#include <algorithm>
#include <set>
#include <unordered_set>

Heart::Heart(cv::Scalar co) :color(co), random(time(nullptr)), randNum1(0, 1), randNum2(0, PI * 2)
{
}

void Heart::build(int number)
{
    std::unordered_set<std::pair<int, int>, hash_pair> pr;
    // make heart
    for (int i = 0; i < number; i++)
    {
        pr.insert(heart_function(i));
    }
    for (auto it = pr.begin(); it != pr.end(); it++)
    {
        _points.emplace_back(it->first, it->second);
    } 

    // Heart immediate inside
    for (auto& pt : _points)
    {
        for (auto i : { 0, 1, 2 })
        {
            auto x = pt.x;
            auto y = pt.y;
            _ex_points.push_back(scatter_inside(x, y, 0.05));
        }
    }

    // Inside
    std::uniform_int_distribution<int> randNum(0,_points.size() - 1);
    for (int i = 0; i <= 8000; i++)
    {
        auto pt = _points[randNum(random)];
        _inside.push_back(scatter_inside(pt.x, pt.y, 0.17));
    }
}

void Heart::draw(cv::Mat &image, int time_interval)
{
    // 获取
    for (int i = 0; i < 20; i++)
        calc(i);
    for (int i = 0; i < 2000; i++)
    {
        image = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
        render(image, i);
        cv::imshow("Heart", image);
        cv::waitKey(time_interval);
    }
    cv::waitKey(0);

}

void Heart::clear()
{
    _points.clear();
    _ex_points.clear();
    _inside.clear();
}

std::pair<int, int> Heart::heart_function(double t)
{

    // heart function
    double x = 16 * pow(sin(t), 3);
    double y = -(13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t));

    // enlarge
    x *= IMAGE_ENLARGE;
    y *= IMAGE_ENLARGE;

    // shift centor
    x += CENTER_X;
    y += CENTER_Y;

    return std::pair<int, int>(x, y);
}

cv::Point2d Heart::scatter_inside(const double &x, const double &y, double beta)
{
    auto ratiox = -beta * log(randNum1(random));
    auto ratioy = -beta * log(randNum1(random));
    auto dx = ratiox * (x - CENTER_X);
    auto dy = ratioy * (y - CENTER_Y);
    return cv::Point2d(x - dx, y - dy);
}

// 计算不同帧时每一个点的位置
cv::Point2d Heart::cal_position(const double &x, const double &y, double ratio)
{
    std::uniform_int_distribution<int> randNum(-1, 1);
    double force = 1 / pow(pow(x - CENTER_X, 2) + pow(y - CENTER_Y, 2), 0.520);
    auto dx = ratio * force * (x - CENTER_X) + randNum(random);
    auto dy = ratio * force * (y - CENTER_Y) + randNum(random);
    return cv::Point2d(x - dx, y - dy);
}

// 调整初始状态，使其更圆润
cv::Point2d Heart::shrink(const double& x,const double& y, int ratio)
{
    auto force = 1 / pow((pow(x - CENTER_X, 2) + pow(y - CENTER_Y, 2)), 0.6);
    auto dx = ratio * force * (x - CENTER_X);
    auto dy = ratio * force * (y - CENTER_Y);
    return cv::Point2d(x - dx, y - dy);
}

void Heart::calc(int frame)
{
    auto ratio = 10 * curve(frame / static_cast<double>(10) * PI);  // 圆滑的周期的缩放比例
    std::cout << ratio << std::endl;
    std::uniform_int_distribution<int> randNum1(1, 3);
    std::uniform_int_distribution<int> randNum2(1, 2);
    std::uniform_int_distribution<int> randNum3(-24, 24);
    int arr[] = { 0, 1, 2, 2 };
    std::unordered_set<std::pair<int, int>, hash_pair> pr_set;

    std::vector<cv::Point3d> all_points;

    auto halo_radius = int(4 + 6 * (1 + curve(frame / static_cast<double>(10) * PI)));
    auto halo_number = int(3000 + 4000 * abs(curve(pow((frame / static_cast<double>(10) * PI), 2))));

    // 光环
    auto tmp = 0;
    for (int i = 0; i < halo_number; i++)
    {
        auto t = this->randNum2(random);//随机不到的地方造成爱心有缺口
        auto pr = heart_function(t);
        if (pr_set.find(pr) == pr_set.end())
        {
            pr_set.insert(pr);
            tmp++;
            auto p2d = shrink(pr.first, pr.second, halo_radius);
            auto size = arr[randNum1(random)];
            auto x = p2d.x + randNum3(random);
            auto y = p2d.y + randNum3(random);
            all_points.emplace_back(x, y, size);
        }       
    }

    // outline
    for (auto& pt : _points)
    {
        auto size = randNum1(random);
        auto pi = cal_position(pt.x, pt.y, ratio);
        all_points.emplace_back(pi.x, pi. y, size);
    }

    // inner
    for (auto& pt : _ex_points)
    {
        auto size = randNum2(random);
        auto pi = cal_position(pt.x, pt.y, ratio);
        all_points.emplace_back(pi.x, pi.y, size);
    }

    // center
    for (auto& pt : _inside)
    {
        auto size = randNum2(random);
        auto pi = cal_position(pt.x, pt.y, ratio);
        all_points.emplace_back(pi.x, pi.y, size);
    }

    _all_points.push_back(all_points);
}

void Heart::render(cv::Mat& image, int frame)
{
    for (auto& pt : _all_points[frame % 20])
    {
        cv::rectangle(image,
            cv::Point2d(pt.x, pt.y),
            cv::Point2d(pt.x + pt.z - 1, pt.y + pt.z - 1),
            color, 
            1);
    }
}

double Heart::curve(const double& p)
{
    return 2 * (2 * sin(4 * p)) / (2 * PI);
}
