#include <iostream>
#include <vector>
#include <algorithm>
#include <random>   
#include <chrono>   
#include <numeric>  
#include <iomanip>

using namespace std;

int kadane1D(const vector<int>& arr) {

    if (arr.empty()) { 
        return 0;
    }
    int current_max = arr[0];
    int total_max = arr[0];

    for (int i = 1; i < arr.size(); ++i) { 
        current_max = max(arr[i], current_max + arr[i]);
        total_max = max(total_max, current_max);
    }
    return total_max;
}

int kadane2D(const vector<vector<int>>& matrix) {

    if (matrix.empty() || matrix[0].empty()) {
        return 0;
    }

    int rows = matrix.size();
    int column = matrix[0].size();
    int max_sum = matrix[0][0]; 

    for (int upper_row = 0; upper_row < rows; ++upper_row) {
        vector<int> temp_array(column, 0); 
        for (int lower_row = upper_row; lower_row < rows; ++lower_row) {
            for (int col = 0; col < column; ++col) {
                temp_array[col] += matrix[lower_row][col];
            }
            max_sum = max(max_sum, kadane1D(temp_array));
        }
    }
    return max_sum;
}

// Función para generar una matriz aleatoria de N*N
// con valores entre [-100, 100] y probabilidad de positivo i_matrix / 20
vector<vector<int>> generateRandomMatrix(int n, int i_matrix, mt19937& rng) {
    vector<vector<int>> matrix(n, vector<int>(n));
    uniform_int_distribution<int> val_dist(-100, 100);
    uniform_real_distribution<double> prob_dist(0.0, 1.0); 

    double positive_probability = static_cast<double>(i_matrix) / 20.0; 

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            int value = val_dist(rng); 
            if (value != 0) { 
                if (prob_dist(rng) > positive_probability) {
                    value = -abs(value); 
                } else {
                    value = abs(value); 
                }
            }
            matrix[r][c] = value;
        }
    }
    return matrix;
}


int main() {

    // Semilla, la misma está en ambos archivos .cpp
    mt19937 rng(12345);

    cout << "Kadane 2D" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << "N\tKadane 2D (ms)" << endl;

    // Rango de N: 5 a 50
    for (int n = 5; n <= 50; ++n) { 
        long long total_time_kadane = 0; 

        // 20 matrices aleatorias para cada N
        for (int i_matrix = 1; i_matrix <= 20; ++i_matrix) { 
            vector<vector<int>> matrix = generateRandomMatrix(n, i_matrix, rng); 

            auto start_kadane = chrono::high_resolution_clock::now();
            kadane2D(matrix); // Ejecuta el algoritmo
            auto end_kadane = chrono::high_resolution_clock::now();
            total_time_kadane += chrono::duration_cast<chrono::nanoseconds>(end_kadane - start_kadane).count(); // Suma el tiempo en nanosegundos

        }
        double avg_time_kadane_ms = static_cast<double>(total_time_kadane) / 20.0 / 1e6; 
        
       
        cout << n << "\t" << fixed << setprecision(6) << avg_time_kadane_ms << endl; 
    }

    return 0;
}