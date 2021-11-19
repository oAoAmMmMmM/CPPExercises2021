#include "hough.h"

#include <libutils/rasserts.h>
const double PI = 3.14159265358979323846264338327950288;
double toRadians(double degrees)
{
    return degrees * PI / 180.0;
}

double estimateR(double x0, double y0, double ttr)
{
    double r0 = x0 * cos(ttr) + y0 * sin(ttr);
    return r0;
}

cv::Mat buildHough(cv::Mat sobel) {// единственный аргумент - это результат свертки Собелем изначальной картинки
    // проверяем что входная картинка - одноканальная и вещественная:
    rassert(sobel.type() == CV_32FC1, 237128273918006);

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = 100; //  замените это число так как вам кажется правильным (отталкиваясь от разрешения картинки - ее ширины и высоты)

    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа
    cv::Mat accumulator(max_r, max_theta, CV_32FC1,0.0f); //  подумайте какого разрешения она должна быть и поправьте ее размер
    for(int i=0;i<max_r;++i)
    {
        for(int j=0;j<max_theta;++j)
        {
            accumulator.at<float>(i, j) = 0;
        }
    }
    //  не забудьте заполнить эту матрицу-картинку-аккумулятор нулями (очистить)

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    double z=sqrt(width*width+height*height+0.0);
    for (int y0 = 0; y0 < height; ++y0) {
        for (int x0 = 0; x0 < width; ++x0) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(y0, x0);//  считайте его "силу градиента" из картинки sobel
            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):

            for (int theta0 = 0; theta0 < max_theta-1; ++theta0) {
                //  рассчитайте на базе информации о том какие координаты у пикселя - (x0, y0) и какой параметр theta0 мы сейчас рассматриваем
                //  обратите внимание что функции sin/cos принимают углы в радианах, поэтому сначала нужно пересчитать theta0 в радианы (воспользуйтесь константой PI)
                double th=(theta0+0.0)/(max_theta+0.0)*2*PI;
                float r0 = x0*cos(th)+y0*sin(th);
                double th1=(theta0+1.0)/(max_theta+0.0)*2*PI;
                float r1 = x0*cos(th1)+y0*sin(th1);
                //  теперь рассчитайте координаты пикслея в пространстве Хафа (в картинке-аккумуляторе) соответсвующего параметрам theta0, r0
                for(int k=std::min(r0,r1);k<std::max(r0,r1);k++){
                    int i = theta0;
                    int j = ((k+0.0)/z)*max_r;
                    if(j>=max_r || j<0) continue;
                    rassert(i >= 0, 237891731289044);
                    rassert(i < accumulator.cols, 237891731289045);
                    rassert(j >= 0, 237891731289046);
                    rassert(j < accumulator.rows, 237891731289047);
                    accumulator.at<float>(j, i) += strength;
                    accumulator.at<float>(j, i+1) += strength;
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
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> winners;

    for (int theta = 1; theta < max_theta-1; ++theta) {
        for (int r = 1; r < max_r-1; ++r) {
            if ((houghSpace.at<float>(r, theta)>houghSpace.at<float>(r+1, theta-1))&&(houghSpace.at<float>(r, theta)>houghSpace.at<float>(r-1, theta+1))) {
                if((houghSpace.at<float>(r, theta)>houghSpace.at<float>(r-1, theta-1))&&(houghSpace.at<float>(r, theta)>houghSpace.at<float>(r+1, theta+1))){
                    if((houghSpace.at<float>(r, theta)>houghSpace.at<float>(r-1, theta))&&(houghSpace.at<float>(r, theta)>houghSpace.at<float>(r+1, theta))){
                        if ((houghSpace.at<float>(r, theta)>houghSpace.at<float>(r, theta-1))&&(houghSpace.at<float>(r, theta)>houghSpace.at<float>(r, theta+1))) {
                            PolarLineExtremum line(theta, r, houghSpace.at<float>(r, theta));
                            winners.push_back(line);
                        }
                    }
                }
            }

        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner)
{
    std::vector<PolarLineExtremum> strongLines;

    // Эта функция по множеству всех найденных локальных экстремумов (прямых) находит самую популярную прямую
    // и возвращает только вектор из тех прямых, что не сильно ее хуже (набрали хотя бы thresholdFromWinner голосов от победителя, т.е. например половину)
    float q=0.0;
    for(int i=0;i<allLines.size();i++){
        if(allLines[i].votes>q){
            q=allLines[i].votes;
        }
    }
    for(int i=0;i<allLines.size();i++){
        if(allLines[i].votes>q*thresholdFromWinner){
            strongLines.push_back(allLines[i]);
        }
    }

    return strongLines;
}