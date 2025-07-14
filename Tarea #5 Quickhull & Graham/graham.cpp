#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <cstdlib> 

using namespace std;

struct Point {
    int x, y;
};

Point p0;

Point nextToTop(stack<Point> &S) {
    Point p = S.top();
    S.pop();
    Point res = S.top();
    S.push(p);
    return res;
}

int distSq(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

int orientation(Point p, Point q, Point r) {
    long long val = (long long)(q.y - p.y) * (r.x - q.x) - (long long)(q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2; 
}

int compare(const void *vp1, const void *vp2) {
   Point *p1 = (Point *)vp1;
   Point *p2 = (Point *)vp2;

   int o = orientation(p0, *p1, *p2);
   if (o == 0)
     return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;

   return (o == 2) ? -1: 1;
}

vector<Point> convexHull(vector<Point>& points) {
    int n = points.size();
    if (n < 3) return {};

    int ymin = points[0].y, min_idx = 0;
    for (int i = 1; i < n; i++) {
        int y = points[i].y;
        if ((y < ymin) || (ymin == y && points[i].x < points[min_idx].x)) {
            ymin = points[i].y;
            min_idx = i;
        }
    }

    swap(points[0], points[min_idx]);
    p0 = points[0];
    qsort(&points[1], n - 1, sizeof(Point), compare);

    int m = 1;
    for (int i = 1; i < n; i++) {
        while (i < n - 1 && orientation(p0, points[i], points[i + 1]) == 0)
            i++;
        points[m] = points[i];
        m++;
    }
    
    vector<Point> hull_points;
    if (m < 3) return hull_points;

    stack<Point> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    for (int i = 3; i < m; i++) {
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    while (!S.empty()) {
        hull_points.push_back(S.top());
        S.pop();
    }
    
    return hull_points;
}

int main() {

   vector<Point> points = {
        {0, 3}, {1, 1}, {2, 2}, {4, 4},
        {0, 0}, {1, 2}, {3, 1}, {3, 3}
    };

    cout << "--- Graham Scan ---" << endl;
    
    vector<Point> hull = convexHull(points);

    cout << "Los puntos de la envolvente convexa son:" << endl;
    for (const auto& p : hull) {
        cout << "(" << p.x << ", " << p.y << ")" << endl;
    }

    return 0;
}