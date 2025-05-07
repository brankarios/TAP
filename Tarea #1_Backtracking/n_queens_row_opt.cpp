#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

const int BOARD_SIZE = 32;

int board[BOARD_SIZE][BOARD_SIZE] = {0};

bool solution_found = false;

void print_board(){

    for (int i = 0; i < BOARD_SIZE; i++){

        for (int j = 0; j < BOARD_SIZE; j++){

            if (board[i][j] < 0){

                cout << "Q" << -board[i][j] << " "; // Imprime la reina con su ID

            } else{

                cout << ".  "; // Imprime casilla vacía

            }
        }

        cout << endl;

    }
}


void check_box(int row, int column, int queen_ID, bool mark){

    for(int i = 0; i < BOARD_SIZE; i++){

        if (mark){

            if(board[i][column] == 0){

                board[i][column] = queen_ID; // Marca toda la columna como ocupada por la reina

            }

        }else{

            if(board[i][column] == queen_ID){

                board[i][column] = 0; // Desmarca toda la columna como ocupada por la reina si se devuelve
            }
        }
    }

    // Marca las diagonales ocupadas por la reina

    for (int step = 1; step < BOARD_SIZE; step++){

        if (row + step < BOARD_SIZE && column + step < BOARD_SIZE){

            if (mark){

                if (board[row + step][column + step] == 0){

                    board[row + step][column + step] = queen_ID;

                }

            }else{

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

    }else{

        board[row][column] = 0; 

    }
}

void backtracking(int row, int queen_ID){

    if (solution_found){

        return;

    }

    if (row == BOARD_SIZE){

        solution_found = true; 
        print_board(); 
        return;

    }

    for (int column = 0; column < BOARD_SIZE; column++){

        if (board[row][column] == 0){

            check_box(row, column, queen_ID, true); // Marca la posición de la reina y las casillas ocupadas

            backtracking(row + 1, queen_ID + 1); // Llama a la función recursiva para la siguiente fila

            check_box(row, column, queen_ID, false); // Desmarca la posición de la reina y las casillas ocupadas
        }
    }
}

int main(){

    auto start = high_resolution_clock::now();
    backtracking(0, 1);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    
    cout << "Execution time: " << duration.count()/1000.0 << " miliseconds" << endl;
    
    return 0;
    
}



