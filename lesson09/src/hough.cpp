#include "hough.h"

#include <libutils/rasserts.h>

#include <opencv2/imgproc.hpp>
#include <iostream>

const double PI = 3.14159265358979323846264338327950288;
double toRadians(double degrees)
{
    return degrees * PI / 180.0;
}

double estimateR(double x0, double y0, double theta0radians)
{
    double r0 = x0 * cos(theta0radians) + y0 * sin(theta0radians);
    return r0;
}


cv::Mat buildHough(cv::Mat sobel) {
    rassert(sobel.type() == CV_32FC1, "ch_and_type");

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = sqrt(sobel.cols * sobel.cols + sobel.rows * sobel.rows) ; //  замените это число так как вам кажется правильным (отталкиваясь от разрешения картинки - ее ширины и высоты)
    std::cout << "max_r  = " << max_r << std::endl;
    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа kjkufhkuhfkus
    cv::Mat accumulator(max_r, max_theta, CV_32FC1,
                        0.0f); //  подумайте какого разрешения она должна быть и поправьте ее размер
    for (int i = 0; i < max_r; ++i) {
        for (int j = 0; j < max_theta; ++j) {
            accumulator.at<float>(i, j) =  0;
        }
    }
    //  не забудьте заполнить эту матрицу-картинку-аккумулятор нулями (очистить)

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    double z = sqrt(width * width + height * height + 0.0);
    for (int y0 = 0; y0 < height; ++y0) {
        for (int x0 = 0; x0 < width; ++x0) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(y0, x0);//  считайте его "силу градиента" из картинки sobel
            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):

            for (int theta0 = 0; theta0 < max_theta - 1; ++theta0) {
                //  рассчитайте на базе информации о том какие координаты у пикселя - (x0, y0) и какой параметр theta0 мы сейчас рассматриваем
                //  обратите внимание что функции sin/cos принимают углы в радианах, поэтому сначала нужно пересчитать theta0 в радианы (воспользуйтесь константой PI)
                double th = (theta0 + 0.0) / (max_theta + 0.0) * 2 * PI;
                float r0 = x0 * cos(th) + y0 * sin(th);
                double th1 = (theta0 + 1.0) / (max_theta + 0.0) * 2 * PI;
                float r1 = x0 * cos(th1) + y0 * sin(th1);
                //  теперь рассчитайте координаты пикслея в пространстве Хафа (в картинке-аккумуляторе) соответсвующего параметрам theta0, r0
                for (int k = std::min(r0, r1); k < std::max(r0, r1); k++) {
                    int i = theta0;
                    int j = ((k + 0.0) / z) * max_r;
                    if (j >= max_r || j < 0) {
                        continue;
                    }
                    rassert(i >= 0, "colserrorDown");
                    rassert(i < accumulator.cols, "colserrorUp");
                    rassert(j >= 0, "rowserrorDown");
                    rassert(j < accumulator.rows, "rowserrorUp");
                    accumulator.at<float>(j, i) += strength;
                    accumulator.at<float>(j, i + 1) += strength;
                }


                // чтобы проверить не вышли ли мы за пределы картинки-аккумулятора - давайте явно это проверим:

                // теперь легко отладить случай выхода за пределы картинки
                //  просто поставьте точку остановки внутри rassert:
                // нажмите Ctrl+Shift+N -> rasserts.cpp
                // и поставьте точку остановки на 8 строке: "return line;"

                //  и добавьте в картинку-аккумулятор наш голос с весом strength (взятый из картинки свернутой Собелем)

            }
        }
    }

    return accumulator;
    // TODO скопируйте сюда свою реализацию построения пространства Хафа из прошлого задания - lesson08
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> result;
    int i = 0;
    for (int theta = 1; theta < max_theta - 1; ++theta) {
        for (int r = 1; r < max_r - 1; ++r) {
            if ((houghSpace.at<float>(r, theta) > houghSpace.at<float>(r + 1, theta - 1)) &&
                (houghSpace.at<float>(r, theta) > houghSpace.at<float>(r - 1, theta + 1))) {
                if ((houghSpace.at<float>(r, theta) > houghSpace.at<float>(r - 1, theta - 1)) &&
                    (houghSpace.at<float>(r, theta) > houghSpace.at<float>(r + 1, theta + 1))) {
                    if ((houghSpace.at<float>(r, theta) > houghSpace.at<float>(r - 1, theta)) &&
                        (houghSpace.at<float>(r, theta) > houghSpace.at<float>(r + 1, theta))) {
                        if ((houghSpace.at<float>(r, theta) > houghSpace.at<float>(r, theta - 1)) &&
                            (houghSpace.at<float>(r, theta) > houghSpace.at<float>(r, theta + 1))) {
                            PolarLineExtremum line(theta, r, houghSpace.at<float>(r, theta));
                            result.push_back(line);
                            i++;
                            std::cout << "Test Line #" << i << " theta=" << line.theta << " r=" << line.r << " votes=" << line.votes << std::endl;
                        }
                    }
                }
            }

        }
    }
    std::cout << "Total lines = " << (i + 0) << std::endl;
    return result;
    // TODO скопируйте сюда свою реализацию извлечения экстремумов из прошлого задания - lesson08
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner)
{
    std::vector<PolarLineExtremum> strongLines;

    // Эта функция по множеству всех найденных локальных экстремумов (прямых) находит самые правильные
    // проход по голосам
    float q = 0.0;
    for (int i = 0; i < allLines.size(); i++) {
        if (allLines[i].votes > q) {
            q = allLines[i].votes;
        }
    }
    for (int i = 0; i < allLines.size(); i++) {
        if (allLines[i].votes > q * thresholdFromWinner) {
            strongLines.push_back(allLines[i]);
        }
    }

    return strongLines;
    // TODO скопируйте сюда свою реализацию фильтрации сильных прямых из прошлого задания - lesson08
}

cv::Mat drawCirclesOnExtremumsInHoughSpace(cv::Mat houghSpace, std::vector<PolarLineExtremum> lines, int radius)
{
    // TODO Доделайте эту функцию - пусть она скопирует картинку с пространством Хафа и отметит на ней красным кружком указанного радиуса (radius) места где были обнаружены экстремумы (на базе списка прямых)

    // делаем копию картинки с пространством Хафа (чтобы не портить само пространство Хафа)
    cv::Mat houghSpaceWithCrosses = houghSpace.clone();

    // проверяем что пространство состоит из 32-битных вещественных чисел (т.е. картинка одноканальная)
    rassert(houghSpaceWithCrosses.type() == CV_32FC1, 347823472890137);

    // но мы хотим рисовать КРАСНЫЙ кружочек вокруг найденных экстремумов, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку
    cv::cvtColor(houghSpaceWithCrosses, houghSpaceWithCrosses, cv::COLOR_GRAY2BGR);
    // проверяем что теперь все хорошо и картинка трехканальная (но при этом каждый цвет - 32-битное вещественное число)
    rassert(houghSpaceWithCrosses.type() == CV_32FC3, 347823472890148);

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];

        // Пример как рисовать кружок в какой-то точке (закомментируйте его):
   //     cv::Point point(100, 50);
     //   cv::Scalar color(0, 0, 255); // BGR, т.е. красный цвет
       // cv::circle(houghSpaceWithCrosses, point, 3, color);
        cv::Point p(line.theta, line.r);
        cv::Scalar c(0,0,255);
        cv::circle(houghSpaceWithCrosses, p, 5, c);
        // TODO отметьте в пространстве Хафа красным кружком радиуса radius экстремум соответствующий прямой line
    }

    return houghSpaceWithCrosses;
}

cv::Point PolarLineExtremum::intersect(PolarLineExtremum that)
{
    // Одна прямая - наш текущий объект (this) у которого был вызван этот метод, у этой прямой такие параметры:
    double theta0 = this->theta;
    double r0 = this->r;

    // Другая прямая - другой объект (that) который был передан в этот метод как аргумент, у этой прямой такие параметры:
    double theta1 = that.theta;
    double r1 = that.r;

    // TODO реализуйте поиск пересечения этих двух прямых, напоминаю что формула прямой описана тут - https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/02/lesson8-hough-transform.html
    // после этого загуглите как искать пересечение двух прямых, пример запроса: "intersect two 2d lines"
    // и не забудьте что cos/sin принимают радианы (используйте toRadians)
    double k0 = -(cos(toRadians(theta0))/sin(toRadians(theta0)));
    double b0 = r0/sin(toRadians(theta0));
    double k1 = -(cos(toRadians(theta1))/sin(toRadians(theta1)));
    double b1 = r1/sin(toRadians(theta1));
    int x = (b0 - b1) / (k1 - k0);
    int y = ((k1 * (b0 - b1)) / (k1 - k0)) + b1;

    return cv::Point(x, y);
}

// TODO Реализуйте эту функцию - пусть она скопирует картинку и отметит на ней прямые в соответствии со списком прямых
cv::Mat drawLinesOnImage(cv::Mat img, std::vector<PolarLineExtremum> lines)
{
    // делаем копию картинки (чтобы при рисовании не менять саму оригинальную картинку)
    cv::Mat imgWithLines = img.clone();

    // проверяем что картинка черно-белая (мы ведь ее такой сделали ради оператора Собеля) и 8-битная
    rassert(imgWithLines.type() == CV_8UC1, 45728934700167);

    // но мы хотим рисовать КРАСНЫЕ прямые, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку с 8 битами в каждом пикселе
    cv::cvtColor(imgWithLines, imgWithLines, cv::COLOR_GRAY2BGR);
    rassert(imgWithLines.type() == CV_8UC3, 45728934700172);

    // выпишем размер картинки
    int width = imgWithLines.cols;
    int height = imgWithLines.rows;

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];
        int theta0 = line.theta;
        double r0 = line.r;
        double k0 = -(cos(toRadians(theta0))/sin(toRadians(theta0)));
        double b0 = r0/(sin(toRadians(theta0)));
        for(int x = 0; x< width; x++) {
            int y = k0*x + b0;
            if((y >= 0) && (y<height)) {
                cv::Point p(x, y);
                cv::Scalar c(0,0,255);
                cv::circle(imgWithLines, p, 5, c);
            }
        }
        // нам надо найти точки на краях картинки
        //cv::Point pointA;
       // cv::Point pointB;

        // TODO создайте четыре прямых соответствующих краям картинки (на бумажке нарисуйте картинку и подумайте какие theta/r должны быть у прямых?):
        // напоминаю - чтобы посмотреть какие аргументы требует функция (или в данном случае конструктор объекта) - нужно:
        // 1) раскомментировать эти четыре строки ниже
        // 2) поставить каретку (указатель где вы вводите новые символы) внутри скобок функции (или конструктора, т.е. там где были три вопроса: ???)
        // 3) нажать Ctrl+P чтобы показать список параметров (P=Parameters)
      //  PolarLineExtremum leftImageBorder(180,0,0);
       // PolarLineExtremum bottomImageBorder(90,height,0);
    //    PolarLineExtremum rightImageBorder(0, width,0);
      //  PolarLineExtremum topImageBorder(270, 0, 0);

        // TODO воспользуйтесь недавно созданной функций поиска пересечения прямых чтобы найти точки пересечения краев картинки:
       // pointA = line.intersect(leftImageBorder);
        //pointB = line.intersect(rightImageBorder);

        // TODO а в каких случаях нужно использовать пересечение с верхним и нижним краем картинки?
     //   pointA = line.intersect(topImageBorder);
      //  pointB = line.intersect(bottomImageBorder);

        // TODO переделайте так чтобы цвет для каждой прямой был случайным (чтобы легче было различать близко расположенные прямые)
        cv::Scalar color(0, 0, 255);
     //   cv::line(imgWithLines, pointA, pointB, color);
    }

    return imgWithLines;
}
