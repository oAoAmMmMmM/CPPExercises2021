#include "hog.h"
#include "math.h"
#include <libutils/rasserts.h>

#include <opencv2/imgproc.hpp>

#define _USE_MATH_DEFINES
#include <math.h>


HoG buildHoG(cv::Mat grad_x, cv::Mat grad_y) {
    rassert(grad_x.type() == CV_32FC1, 2378274892374008);
    rassert(grad_y.type() == CV_32FC1, 2378274892374008);

    rassert(grad_x.rows == grad_y.rows, 3748247980010);
    rassert(grad_x.cols == grad_y.cols, 3748247980011);
    int height = grad_x.rows;
    int width = grad_x.cols;

    HoG hog;

    // TODO
    // 1) увеличьте размер вектора hog до NBINS (ведь внутри это просто обычный вектор вещественных чисел) - yes
    for(int i =0; i < NBINS; i++) {
        hog.push_back(0);
    }
    // 2) заполните его нулями - yes
    // 3) пробегите по всем пикселям входной картинки и посмотрите на каждый градиент
    // (определенный двумя числами: dx проекцией на ось x в grad_x, dy проекцией на ось y в grad_y)
    // 4) определите его силу (корень из суммы квадратов), определите его угол направления:
    // рекомендую воспользоваться atan2(dy, dx) - он возвращает радианы - https://en.cppreference.com/w/cpp/numeric/math/atan2
    // прочитайте по ссылке на документацию (в прошлой строке) - какой диапазон значений у угла-результата atan2 может быть?
    // 5) внесите его силу как голос за соответствующую его углу корзину
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            float dx = grad_x.at<float>(j, i);
            float dy = grad_y.at<float>(j, i);
            float strength = sqrt(dx * dx + dy * dy);

            if (strength < 10) { // пропускайте слабые градиенты, это нужно чтобы игнорировать артефакты сжатия в jpeg (например в line01.jpg пиксели не идеально белые/черные, есть небольшие отклонения)
                continue;
            }
            double angle = atan2(dx, dy) + M_PI;
            // TODO рассчитайте в какую корзину нужно внести голос
            int bin = angle / (M_PI/4) - 1;

            rassert(bin >= 0, 3842934728039);
            rassert(bin < NBINS, 34729357289040);
            hog[bin] += strength;
        }
    }

    rassert(hog.size() == NBINS, 23478937290010);
    return hog;
}

// Эта функция просто преобразует картинку в черно-белую, строит градиенты и вызывает buildHoG объявленный выше - TODO ЕЕ ВАМ И НУЖНО ДОДЕЛАТЬ
HoG buildHoG(cv::Mat originalImg) {
    cv::Mat img = originalImg.clone();

    rassert(img.type() == CV_8UC3, 347283678950077);

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // преобразуем в оттенки серого

    cv::Mat grad_x, grad_y; // в этих двух картинках мы получим производную (градиент=gradient) по оси x и y
    // для этого достаточно дважды применить оператор Собеля (реализованный в OpenCV)
    cv::Sobel(img, grad_x, CV_32FC1, 1, 0);
    cv::Sobel(img, grad_y, CV_32FC1, 0, 1);
    rassert(!grad_x.empty(), 234892748239070017);
    rassert(!grad_y.empty(), 234892748239070018);

    // TODO реализуйте эту функцию:
    HoG hog = buildHoG(grad_x, grad_y);
    return hog;
}

// TODO реализуйте функцию которая позволит выводить гистограмму в консоль через std::cout << myHOG << std::endl;
// Пример корректного вывода (выводите не само значение накопленных голосов за каждое направление, а процент от общей суммы голосов):
// HoG[22.5=0%, 67.5=78%, 112.5=21%, 157.5=0%, 202.5=0%, 247.5=0%, 292.5=0%, 337.5=0%]
std::ostream &operator<<(std::ostream &os, const HoG &hog) {
    rassert(hog.size() == NBINS, 234728497230016);
    double all = 0;
    // TODO
    for(int i = 0; i< NBINS; i++) {
        all+=hog.at(i);
    }
    os << "HoG[";
    double angleInDegrees = 22.5;
    for (int bin = 0; bin < NBINS; ++bin) {
        double percentage = (hog.at(bin)/all)*100;
        os << angleInDegrees << "=" << percentage << "%, ";
        angleInDegrees+=45;
    }
    os << "]";
    return os;
}

double pow2(double x) {
    return x * x;
}

double p(double x, double y){
    return double ((x/y)*100);
}

// TODO реализуйте функцию которая по двум гистограммам будет говорить насколько они похожи
double distance(HoG a, HoG b) {
    rassert(a.size() == NBINS, 237281947230077);
    rassert(b.size() == NBINS, 237281947230078);
    double alla = 0;
    // TODO
    for(int i = 0; i< NBINS; i++) {
        alla+=a.at(i);
    }
    double allb = 0;
    for(int i = 0; i< NBINS; i++) {
        allb+=b.at(i);
    }

    // TODO рассчитайте декартово расстояние (т.е. корень из суммы квадратов разностей)
    // подумайте - как можно добавить независимость (инвариантность) гистаграммы градиентов к тому насколько контрастная или блеклая картинка?
    // подсказка: на контрастной картинке все градиенты гораздо сильнее, а на блеклой картинке все градиенты гораздо слабее, но пропорции между градиентами (распроцентовка) не изменны!

    for (int g = 0; g < 8; g++){
        a.push_back(a.at(g));
    }


    double res = DBL_MAX;
    for ( int i = 0; i < 8; i++){
        double len = pow2(p(a.at(i), alla) - p(b.at(0), allb) );
        len += pow2(p(a.at(i+1), alla) - p(b.at(1), allb) );
        len += pow2(p(a.at(i+2), alla) - p(b.at(2), allb) );
        len += pow2(p(a.at(i+3), alla) - p(b.at(3), allb) );
        len += pow2(p(a.at(i+4), alla) - p(b.at(4), allb) );
        len += pow2(p(a.at(i+5), alla) - p(b.at(5), allb) );
        len += pow2(p(a.at(i+6), alla) - p(b.at(6), allb) );
        len += pow2(p(a.at(i+7), alla) - p(b.at(7), allb) );
        len = sqrt(len);
        if (len < res){
            res = len;
        }
    }


    return res;
}