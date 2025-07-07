#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

bool solution_found = false;

bool sameDiagonal(int col1, int row1, int col2, int row2){

    return abs(col1 - col2) == abs(row1 - row2);

}

/* void printSolution(int A[], int n){

    for (int i = 0; i < n; i++){

        for (int j = 0; j < n; j++){

            if (A[j] == i){

                cout << "Q ";

            } else{

                cout << ". ";

            }
        }

        cout << endl;
    }
} */

void permute(int step, int A[], int n){

    if (solution_found){

        return;

    }

    if (step == n){

        // printSolution(A, n);
        solution_found = true;
        return;

    }

    for (int i = step; i < n; i++){

        bool valid = true;

        for (int k = 0; k < step; k++){

            if (sameDiagonal(k, A[k], step, A[i])){

                valid = false;
                break;

            }

        }

        if (valid){

            swap(A[step], A[i]); // Intercambia de lugar el elemento actual con el de la posición i
            permute(step + 1, A, n); // Llama a la función recursiva para la siguiente posición
            swap(A[step], A[i]); // Backtracking

        }
    }
}

void reset(int queens[], int n){

    solution_found = false;
    for (int i = 0; i < n; i++) {
        queens[i] = i;
    }
}

int main(){

    const int n = 32; 
    const int ITERATIONS = 50; 
    int queens[n];
    long long total_duration = 0;
    long long total_microseconds = 0;
    double total_milliseconds = 0.0;

    for (int run = 0; run < ITERATIONS; run++){

        reset(queens, n);
        
        auto start = high_resolution_clock::now();
        permute(0, queens, n);
        auto stop = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(stop - start);
        total_microseconds += duration.count();
        total_milliseconds += duration.count() / 1000.0;

        total_duration += duration.count();
    }

    double avg_microseconds = static_cast<double>(total_microseconds) / ITERATIONS;
    double avg_milliseconds = total_milliseconds / ITERATIONS;

    cout << "Total execution time for " << ITERATIONS << " iterations: " << total_duration << " microseconds" << endl;
    cout << "Average execution time: " << avg_microseconds << " microseconds (" << avg_milliseconds << " milliseconds)" << endl;

    return 0;
}