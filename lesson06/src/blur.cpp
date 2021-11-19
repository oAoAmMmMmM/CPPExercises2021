#include "blur.h"

#include <libutils/rasserts.h>

cv::Mat blur(cv::Mat img, double sigma) {
    // TODO формулу весов можно найти тут:
    // https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D0%B7%D0%BC%D1%8B%D1%82%D0%B8%D0%B5_%D0%BF%D0%BE_%D0%93%D0%B0%D1%83%D1%81%D1%81%D1%83
    int width = img.cols;
    int height = img.rows;
    cv::Mat blurImage(height,  width, CV_32FC1);
    int sumr = 0;
    int sumg = 0;
    int sumb = 0;
    int ksum1r;
    int ksum1g;
    int ksum1b;
    for (int i = 0; i < img.cols; ++i) {
        for (int j = 0; j < img.rows; ++j) {
            cv::Vec3b color = img.at<cv::Vec3b>(j, i);
            unsigned char blue = color[0];
            unsigned char green = color[1];
            unsigned char red = color[2];
            cv::Vec3b c = img.at<cv::Vec3b>(j, i);
            int r = c[2];
            int g = c[1];
            int b = c[0];
            sumr += r;
            sumg += g;
            sumb +=b;
            int ssumr = sumr/(i*j);
            int ssumg = sumg/(i*j);
            int ssumb = sumb/(i*j);
            ksum1r = (r - ssumr)^2;
            ksum1g = (r - ssumg)^2;
            ksum1b = (r - ssumb)^2;
        }
    }
    double sigmaR = sqrt(ksum1r/(img.cols*img.rows));
    double sigmaG = sqrt(ksum1g/(img.cols*img.rows));
    double sigmaB = sqrt(ksum1b/(img.cols*img.rows));



    return img;
}
