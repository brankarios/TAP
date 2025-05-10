#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
using namespace std;
using namespace std::chrono;

const int BOARD_SIZE = 64;
const double PROBABILITY = 0.3;

int board[BOARD_SIZE][BOARD_SIZE] = {0};
bool solution_found = false;
mt19937 rng(random_device{}()); // Generador de números pseudoaleatorios Mersenne Twister 19937

/* void print_board(){

    for (int i = 0; i < BOARD_SIZE; i++){

        for (int j = 0; j < BOARD_SIZE; j++){

            if (board[i][j] < 0){

                cout << "Q" << -board[i][j] << " ";
                
            } else{

                cout << ".  ";
            }
        }
        cout << endl;
    }
} */

void check_box(int row, int column, int queen_ID, bool mark){

    for(int i = 0; i < BOARD_SIZE; i++){

        if (mark){

            if(board[i][column] == 0){

                board[i][column] = queen_ID;

            }

        }else{
            if(board[i][column] == queen_ID){

                board[i][column] = 0;
            }
        }
    }

    for (int step = 1; step < BOARD_SIZE; step++){

        if (row + step < BOARD_SIZE && column + step < BOARD_SIZE){

            if (mark){

                if (board[row + step][column + step] == 0){

                    board[row + step][column + step] = queen_ID;

                }
            } else{

                if (board[row + step][column + step] == queen_ID){

                    board[row + step][column + step] = 0;
                }
            }
        }

        if(row - step >= 0 && column - step >= 0){

            if (mark){

                if (board[row - step][column - step] == 0){

                    board[row - step][column - step] = queen_ID;

                }
            }else{

                if (board[row - step][column - step] == queen_ID){

                    board[row - step][column - step] = 0;

                }
            }
        }

        if(row + step < BOARD_SIZE && column - step >= 0){

            if (mark){

                if (board[row + step][column - step] == 0){

                    board[row + step][column - step] = queen_ID;

                }
            }else{

                if (board[row + step][column - step] == queen_ID){

                    board[row + step][column - step] = 0;
                }
            }
        }

        if (row - step >= 0 && column + step < BOARD_SIZE){

            if (mark){

                if (board[row - step][column + step] == 0){

                    board[row - step][column + step] = queen_ID;

                }
            }else{

                if (board[row - step][column + step] == queen_ID){

                    board[row - step][column + step] = 0;
                }
            }
        }
    }

    if (mark){

        board[row][column] = -queen_ID;

    } else{

        board[row][column] = 0;
    }
}

void probabilistic_backtracking(int row, int queen_ID){

    if (solution_found) return;

    if (row == BOARD_SIZE){

        solution_found = true;
        // print_board();
        return;

    }

    vector<int> columns(BOARD_SIZE);
    iota(columns.begin(), columns.end(), 0);
    shuffle(columns.begin(), columns.end(), rng);

    int positions_to_check = max(1, static_cast<int>(BOARD_SIZE * PROBABILITY));

    for (int i = 0; i < positions_to_check; i++){

        int column = columns[i];

        if (board[row][column] == 0){

            check_box(row, column, queen_ID, true);

            probabilistic_backtracking(row + 1, queen_ID + 1);

            if (solution_found) return;

            check_box(row, column, queen_ID, false);
        }
    }
}


void reset_board(){

    solution_found = false;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

int main(){

    const int ITERATIONS = 10;
    long long total_microseconds = 0;
    int solutions_found = 0;

    for (int i = 0; i < ITERATIONS; i++){
        reset_board();
        
        auto start = high_resolution_clock::now();
        probabilistic_backtracking(0, 1);
        auto stop = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(stop - start);
        total_microseconds += duration.count();
        
        if (solution_found) {
            solutions_found++;
        }
    }

    double avg_microseconds = static_cast<double>(total_microseconds) / ITERATIONS;
    double avg_milliseconds = avg_microseconds / 1000.0;
    double success_rate = (static_cast<double>(solutions_found) / ITERATIONS) * 100.0;

    cout << "Total duration: " << total_microseconds << endl;
    cout << "Solutions found: " << solutions_found << " (" << success_rate << "% success rate)" << endl;
    cout << "Average execution time: " << avg_microseconds << " microseconds (" << avg_milliseconds << " milliseconds)" << endl;
    
    return 0;
}