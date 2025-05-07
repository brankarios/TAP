#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

bool solution_found = false;

bool sameDiagonal(int col1, int row1, int col2, int row2){

    return abs(col1 - col2) == abs(row1 - row2);

}

void printSolution(int A[], int n){

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
}

void permute(int step, int A[], int n){

    if (solution_found){

        return;

    }

    if (step == n){

        printSolution(A, n);
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

int main(){

    const int n = 32;
    int queens[n];

    for (int i = 0; i < n; i++){

        queens[i] = i;

    }

    auto start = high_resolution_clock::now();
    permute(0, queens, n);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Execution time: " << duration.count()/1000.0 << " miliseconds" << endl;


    return 0;
}