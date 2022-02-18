#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
// беда не работает дайте руки и мозг
int debugPoint(int line) {
    if (line < 0)
        return 0;

    // You can put breakpoint at the following line to catch any rassert failure:
    return line;
}

#define rassert(condition, message) if (!(condition)) { std::stringstream ss; (ss << "Assertion \"" << message << "\" failed at line " << debugPoint(__LINE__) << "!"); throw std::runtime_error(ss.str()); }
bool found(std::vector<int> a, int b) {
    for(int i = 0; i < a.size(); i++) {
        if(a.at(i) == b) {
            return true;
        }
    }
    return false;
}

struct Edge {
    int u, v; // номера вершин которые это ребро соединяет
    int w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(int u, int v, int w) : u(u), v(v), w(w) {}
};

void run() {
    // https://codeforces.com/problemset/problem/20/C?locale=ru
    // Не требуется сделать оптимально быструю версию, поэтому если вы получили:
    //
    // Превышено ограничение времени на тесте 31
    //
    // То все замечательно и вы молодец.

    int nvertices, medges;
    std::cin >> nvertices;
    std::cin >> medges;
    std :: cout  << "1";
    std::vector<std::vector<Edge>> edges_by_vertex(nvertices);
    std::vector<bool> edg_be(nvertices, false);
    for (int i = 0; i < medges; ++i) {
        int ai, bi, w;
        std::cin >> ai >> bi >> w;
        rassert(ai >= 1 && ai <= nvertices, 23472894792020);
        rassert(bi >= 1 && bi <= nvertices, 23472894792021);

        ai -= 1;
        bi -= 1;
        rassert(ai >= 0 && ai < nvertices, 3472897424024);
        rassert(bi >= 0 && bi < nvertices, 3472897424025);

        Edge edgeAB(ai, bi, w);
        edges_by_vertex[ai].push_back(edgeAB);

        edges_by_vertex[bi].push_back(Edge(bi, ai, w)); // а тут - обратное ребро, можно конструировать объект прямо в той же строчке где он и потребовался
    }

    int start = 0;
    const int finish = nvertices - 1;
    const int INF = std::numeric_limits<int>::max();
    Edge a(0,0,0);
    int s = 0;
    std::vector<int> distances(nvertices, INF);
    distances.at(0) = 0;
    int min = INF;
    int num = 0;
    std::vector<int> first_s;
    first_s.push_back(0);
    std::vector<int> second_s;
    // TODO ...
    while (true) {
        for(int i = 0; i < first_s.size(); i++) {
            for(int k =0; k < edges_by_vertex[first_s.at(i)].size(); k++) {
                a = edges_by_vertex[first_s.at(i)].at(k);
                if((!found(second_s, a.v)) && (!edg_be.at(first_s.at(i)))) {
                    second_s.push_back(a.v);
                }
                if(distances.at(a.v) > distances.at(a.u) + a.w) {
                    distances.at(a.v) = distances.at(a.u) + a.w;
                }
            }
            edg_be.at(first_s.at(i)) = true;
        }
        if(second_s.empty()) {
            break;
        }
        first_s = second_s;
        second_s = std::vector<int>();
    }

//    while (true) {
//
//    }

//    if (...) {
//        ...
//        for (...) {
//            std::cout << (path[i] + 1) << " ";
//        }
//        std::cout << std::endl;
//    } else {
//        std::cout << -1 << std::endl;
//    }
}

using namespace std;

int main() {
    int a;
    cin >> a;
    cout << "1";
    try {
  //      run();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
