#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

struct Point{
    int x, y;
};

//Definimos cómo deben compararse los pares ordenados, así que sobrecargamos el operador <
bool operator<(const Point& a, const Point& b){ 
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

vector<Point> hull_points_vector;

//Calculamos el producto cruz 
int findSide(Point p1, Point p2, Point p){
    long long val = (long long)(p.y - p1.y) * (p2.x - p1.x) - (long long)(p2.y - p1.y) * (p.x - p1.x);
    if (val > 0) return 1; //P está a la izquierda
    if (val < 0) return -1; // P está a la derecha
    return 0;
}

//Encontramos el punto más alejado de una línea
int lineDist(Point p1, Point p2, Point p){
    return abs((p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x));
}

//Corazón del algoritmo. Encuentra segmentos que forman la envolvente convexa final, lo hace de manera recursiva
void quickHullRec(const vector<Point>& points, Point p1, Point p2, int side){
    int ind = -1;
    int max_dist = 0;

    for (int i = 0; i < points.size(); i++){
        int temp_dist = lineDist(p1, p2, points[i]);
        if (findSide(p1, p2, points[i]) == side && temp_dist > max_dist) {
            ind = i;
            max_dist = temp_dist;
        }
    }

    if (ind == -1){
        hull_points_vector.push_back(p1);
        hull_points_vector.push_back(p2);
        return;
    }

    quickHullRec(points, points[ind], p1, -findSide(points[ind], p1, p2));
    quickHullRec(points, points[ind], p2, -findSide(points[ind], p2, p1));
}

//Inicia el proceso de búsqueda de la envolvente convexa. No acepta menos de tres puntos
void findHull(const vector<Point>& points){
    if (points.size() < 3) {
        hull_points_vector = points;
        return;
    }

    int min_x_idx = 0, max_x_idx = 0;
    for (int i = 1; i < points.size(); i++){
        if (points[i].x < points[min_x_idx].x) min_x_idx = i;
        if (points[i].x > points[max_x_idx].x) max_x_idx = i;
    }
    
    hull_points_vector.clear();
    quickHullRec(points, points[min_x_idx], points[max_x_idx], 1); //Recursión en la parte superior izquierda
    quickHullRec(points, points[min_x_idx], points[max_x_idx], -1); //Recuersión en la parte inferior derecha
}

int main(){

    srand(12345); //Misma semilla para ambos algoritmos

    vector<int> N_values = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    int num_tests_per_N = 30;

    cout << "--- Quickhull Performance Test ---" << endl;

    for (int N : N_values) {
        double total_duration_ms = 0;
        for (int test = 0; test < num_tests_per_N; ++test) {
            vector<Point> points(N);
            for (int i = 0; i < N; ++i) {
                points[i].x = rand() % 10000;
                points[i].y = rand() % 10000;
            }

            auto start_time = chrono::high_resolution_clock::now();
            findHull(points);
            auto end_time = chrono::high_resolution_clock::now();

            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            total_duration_ms += duration.count() / 1000.0;
        }
        cout << "N = " << N << ": Tiempo promedio = " << total_duration_ms / num_tests_per_N << " ms" << endl;
    }

    return 0;
}