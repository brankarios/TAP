#include <iostream>
#include <vector>
#include <algorithm>
#include <random>   
#include <chrono>   
#include <iomanip>  

using namespace std;

int submatrix_brute_force(const vector<vector<int>>& matrix) {

    if (matrix.empty() || matrix[0].empty()) {
        return 0;
    }

    int rows = matrix.size();
    int columns = matrix[0].size();

    vector<vector<int>> acum(rows, vector<int>(columns, 0));

    acum[0][0] = matrix[0][0];

    for (int j = 1; j < columns; ++j) {
        acum[0][j] = acum[0][j-1] + matrix[0][j];
    }

    for (int i = 1; i < rows; ++i) {
        acum[i][0] = acum[i-1][0] + matrix[i][0];
    }

    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < columns; ++j) {
            acum[i][j] = matrix[i][j] + acum[i-1][j] + acum[i][j-1] - acum[i-1][j-1];
        }
    }

    int global_max_sum = matrix[0][0]; 

    // Los cuatro ciclos generan todas las combinaciones posibles de coordenadas (todas las submatrices posibles)
    for (int row1 = 0; row1 < rows; ++row1) {
        for (int col1 = 0; col1 < columns; ++col1) {
            for (int row2 = row1; row2 < rows; ++row2) {
                for (int col2 = col1; col2 < columns; ++col2) {

                    // Parte principal de la optimización
                    int current_submatrix_sum = acum[row2][col2];

                    if (row1 > 0) {
                        current_submatrix_sum -= acum[row1-1][col2];
                    }
                    if (col1 > 0) {
                        current_submatrix_sum -= acum[row2][col1-1];
                    }
                    if (row1 > 0 && col1 > 0) {
                        current_submatrix_sum += acum[row1-1][col1-1];
                    }
                    global_max_sum = max(global_max_sum, current_submatrix_sum);
                }
            }
        }
    }
    return global_max_sum;
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
            int value = val_dist(rng); // Genera un valor en [-100, 100]
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

    // Semilla para el generador de números aleatorios
    mt19937 rng(12345);

    cout << "Comparacion de rendimiento (Fuerza Bruta O(N^4))" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "N\tFuerza Bruta (ms)" << endl;

    // Rango de N: 5 a 50
    for (int n = 5; n <= 50; ++n) {
        long long total_time_brute_force = 0; 

        // 20 matrices aleatorias para cada N
        for (int i_matrix = 1; i_matrix <= 20; ++i_matrix) { 
            vector<vector<int>> matrix = generateRandomMatrix(n, i_matrix, rng); 

            auto start_brute_force = chrono::high_resolution_clock::now();
            submatrix_brute_force(matrix); // Ejecuta el algoritmo
            auto end_brute_force = chrono::high_resolution_clock::now();
            total_time_brute_force += chrono::duration_cast<chrono::nanoseconds>(end_brute_force - start_brute_force).count(); // Suma el tiempo en nanosegundos
        }

        double avg_time_brute_force_ms = static_cast<double>(total_time_brute_force) / 20.0 / 1e6; 
        
        cout << n << "\t" << fixed << setprecision(6) << avg_time_brute_force_ms << endl;
    }

    return 0;
}