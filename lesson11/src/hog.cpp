#include "hog.h"

#include <libutils/rasserts.h>

#include <opencv2/imgproc.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>


HoG buildHoG(cv::Mat grad_x, cv::Mat grad_y) {
    rassert(grad_x.type() == CV_32FC1, 2378274892374008);
    rassert(grad_y.type() == CV_32FC1, 2378274892374008);

    rassert(grad_x.rows == grad_y.rows, 3748247980010);
    rassert(grad_x.cols == grad_y.cols, 3748247980011);
    int height = grad_x.rows;
    int width = grad_x.cols;

    HoG hog(NBINS, 0);
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            float dx = grad_x.at<float>(j, i);
            float dy = grad_y.at<float>(j, i);
            float strength = sqrt(dx * dx + dy * dy);

            if (strength < 10)
                continue;

            double angle = atan2(dy,dx)+M_PI;
            int bin = std::min(NBINS-1, std::max(0,(int)(angle*NBINS/(2*M_PI))));
            hog[bin] += strength;
        }
    }

    rassert(hog.size() == NBINS, 23478937290010);
    return hog;
}

HoG buildHoG(cv::Mat originalImg) {
    cv::Mat img = originalImg.clone();

    rassert(img.type() == CV_8UC3, 347283678950077);

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::Mat grad_x, grad_y;
    cv::Sobel(img, grad_x, CV_32FC1, 1, 0);
    cv::Sobel(img, grad_y, CV_32FC1, 0, 1);
    rassert(!grad_x.empty(), 234892748239070017);
    rassert(!grad_y.empty(), 234892748239070018);
    HoG hog = buildHoG(grad_x, grad_y);
    return hog;
}

std::ostream &operator<<(std::ostream &os, const HoG &hog) {
    rassert(hog.size() == NBINS, 234728497230016);

    os << "HoG[";

    double sum = 0;
    for(double v : hog)
        sum+=v;
    os << std::fixed << std::setprecision(3);
    for (int bin = 0; bin < NBINS; ++bin) {
        os << 360*bin/NBINS << "=" << (hog[bin]*100/sum) << "%";
        if(bin+1 != NBINS)
            os << ", ";
    }
    os << "]";
    return os;
}

double distance(HoG a, HoG b) {
    rassert(a.size() == NBINS, 237281947230077);
    rassert(b.size() == NBINS, 237281947230078);

    double maxStr = 0;
    for(double v : a)
        maxStr = std::max(maxStr, v);
    for(double & i : a)
        i /= maxStr;

    maxStr = 0;
    for(double v : b)
        maxStr = std::max(maxStr, v);
    for(double & i : b)
        i /= maxStr;
    double res = 0;
    for(int i = 0; i < NBINS; i++){
        res+=pow(a[i]-b[i], 2);
    }
    return sqrt(res);
}