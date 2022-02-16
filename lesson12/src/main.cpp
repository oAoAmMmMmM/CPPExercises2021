#include "line.h"

#include <iostream>
#include <filesystem>


void test1() {
    cv::Scalar b(255, 0, 0);
    cv::Scalar g(0, 255, 0);
    cv::Scalar r(0, 0, 255);
    cv::Scalar bR(255, 0, 255);

    std::string results = "lesson12/resultsData/";
    if (!std::filesystem::exists(results)) { // если папка еще не создана
        std::filesystem::create_directory(results); // то создаем ее
    }

    // нарисуйте на листочке эту прямую, т.е. множество точек соответствующих уравнению 0.5*x-y+5=0
    Line line(0.5, -1, 5);
    std::cout << "Line: " << line << std::endl;

    // здесь создается несколько случайных точек на прямой
    int n = 75;
    double fromX = 0.0;
    double toX = 40.0;
    double plotScale = 50.0;
    double sigma = 0.4;
    std::vector<cv::Point2f> pointsNearLine = line.generatePoints(n, fromX, toX, sigma);
    std::cout << pointsNearLine.size() << " points: ";
    for (int i = 0; i < pointsNearLine.size(); ++i) {
        std::cout << pointsNearLine[i] << ", ";
    }
    std::cout << std::endl;
    cv::Mat plot;
    plotPoints(plot, pointsNearLine, plotScale, b);
    line.plot(plot, plotScale, g);
    cv::imwrite(results + "test1_1points_and_line.png", plot);
    for (int i = 0; i + 1 < pointsNearLine.size(); ++i) {
        cv::Point2f a = pointsNearLine[i];
        cv::Point2f b = pointsNearLine[i + 1];
        if (a==b) continue;
        Line lineI = fitLineFromTwoPoints(a, b);
        std::cout << "Line #" << (i + 1) << ": " << lineI << std::endl;
    }
    cv::imwrite(results + "test1_2all_lines.png", plot);

    Line oneLineToRuleThemAll = fitLineFromNPoints(pointsNearLine);
    std::cout << "One line to rule them all: " << oneLineToRuleThemAll << std::endl;
    oneLineToRuleThemAll.plot(plot, plotScale, b);
    cv::imwrite(results + "test1_3final_line.png", plot);
}


int main() {
    try {
        test1();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}