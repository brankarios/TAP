#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

struct Point{
    int x, y;
};

Point p0; //Punto de anclaje (coordenada Y mínima, en otro caso, coordenada X mínima)

//Accedemos al segundo elemento desde la cima de la pila sin eliminarlo permanentemente
Point nextToTop(stack<Point> &S){
    Point p = S.top();
    S.pop();
    Point res = S.top();
    S.push(p);
    return res;
}

//Distancia euclidiana
int distSq(Point p1, Point p2){
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

//Orientación de los tres puntos
int orientation(Point p, Point q, Point r){
    long long val = (long long)(q.y - p.y) * (r.x - q.x) - (long long)(q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0; //0 si son colineales
    return (val > 0) ? 1 : 2; //1 sentido horario, 2 sentido antihorario
}

//Ordenamos los puntos por ángulo polar respecto al punto de anclaje
int compare(const void *vp1, const void *vp2){
   Point *p1 = (Point *)vp1;
   Point *p2 = (Point *)vp2;

   int o = orientation(p0, *p1, *p2);
   if (o == 0)
     return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;

   return (o == 2) ? -1: 1;
}

//Función principal
vector<Point> convexHull(vector<Point>& points){
    int n = points.size();
    if (n < 3) return {};

    int ymin = points[0].y, min_idx = 0;
    for (int i = 1; i < n; i++) {
        int y = points[i].y;
        if ((y < ymin) || (ymin == y && points[i].x < points[min_idx].x)){
            ymin = points[i].y;
            min_idx = i;
        }
    }

    swap(points[0], points[min_idx]);
    p0 = points[0];
    qsort(&points[1], n - 1, sizeof(Point), compare); //qsort para ordenar los puntos junto con compare

    int m = 1;
    for (int i = 1; i < n; i++) {
        while (i < n - 1 && orientation(p0, points[i], points[i + 1]) == 0)
            i++;
        points[m] = points[i];
        m++;
    }
    
    vector<Point> hull_points;
    if (m < 3) {
        for(int i = 0; i < m; ++i) {
            hull_points.push_back(points[i]);
        }
        return hull_points;
    }

    stack<Point> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    for (int i = 3; i < m; i++){
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    while (!S.empty()){
        hull_points.push_back(S.top());
        S.pop();
    }
    
    reverse(hull_points.begin(), hull_points.end());

    return hull_points;
}

int main(){

    srand(12345); //De nuevo, misma semilla para ambos algoritmos

    vector<int> N_values = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    int num_tests_per_N = 30;

    cout << "--- Graham Performance Test ---" << endl;

    for (int N : N_values) {
        double total_duration_ms = 0;
        for (int test = 0; test < num_tests_per_N; ++test) {
            vector<Point> points(N);
            for (int i = 0; i < N; ++i) {
                points[i].x = rand() % 10000;
                points[i].y = rand() % 10000;
            }

            auto start_time = chrono::high_resolution_clock::now();
            vector<Point> hull = convexHull(points);
            auto end_time = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            total_duration_ms += duration.count() / 1000.0;
        }
        cout << "N = " << N << ": Tiempo promedio = " << total_duration_ms / num_tests_per_N << " ms" << endl;
    }

    return 0;
}