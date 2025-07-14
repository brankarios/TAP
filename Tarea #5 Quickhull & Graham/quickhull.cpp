#include <iostream>
#include <vector>
#include <algorithm>
#include <set> 

using namespace std;

struct Point {
    int x, y;
};

bool operator<(const Point& a, const Point& b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

vector<Point> hull_points_vector;

int findSide(Point p1, Point p2, Point p) {
    long long val = (long long)(p.y - p1.y) * (p2.x - p1.x) - (long long)(p2.y - p1.y) * (p.x - p1.x);
    if (val > 0) return 1;
    if (val < 0) return -1;
    return 0;
}

int lineDist(Point p1, Point p2, Point p) {
    return abs((p.y - p1.y) * (p2.x - p1.x) - (p2.y - p1.y) * (p.x - p1.x));
}

void quickHullRec(const vector<Point>& points, Point p1, Point p2, int side) {
    int ind = -1;
    int max_dist = 0;

    for (int i = 0; i < points.size(); i++) {
        int temp = lineDist(p1, p2, points[i]);
        if (findSide(p1, p2, points[i]) == side && temp > max_dist) {
            ind = i;
            max_dist = temp;
        }
    }

    if (ind == -1) {
        hull_points_vector.push_back(p1);
        hull_points_vector.push_back(p2);
        return;
    }

    quickHullRec(points, points[ind], p1, -findSide(points[ind], p1, p2));
    quickHullRec(points, points[ind], p2, -findSide(points[ind], p2, p1));
}

void findHull(const vector<Point>& points) {
    if (points.size() < 3) {
        hull_points_vector = points;
        return;
    }

    int min_x = 0, max_x = 0;
    for (int i = 1; i < points.size(); i++) {
        if (points[i].x < points[min_x].x) min_x = i;
        if (points[i].x > points[max_x].x) max_x = i;
    }
    
    hull_points_vector.clear();
    quickHullRec(points, points[min_x], points[max_x], 1);
    quickHullRec(points, points[min_x], points[max_x], -1);
}

int main(){

    vector<Point> points = {
        {0, 3}, {1, 1}, {2, 2}, {4, 4},
        {0, 0}, {1, 2}, {3, 1}, {3, 3}
    };

    cout << "--- Quickhull ---" << endl;
    
    findHull(points);
    
    set<Point> unique_points(hull_points_vector.begin(), hull_points_vector.end());

    cout << "Los puntos de la envolvente convexa son:" << endl;
    for (const auto& p : unique_points) {
        cout << "(" << p.x << ", " << p.y << ")" << endl;
    }

    return 0;
}