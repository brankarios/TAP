#include <iostream>
#include <algorithm>
using namespace std;

int kadane(int array[], int size){

    int current_max = array[0];
    int total_max = array[0];

    for (int i = 1; i < size; i++){

        current_max = max(array[i], current_max + array[i]);

        if (current_max > total_max) total_max = current_max;
    }

    return total_max;
}

int main(){

    int size = 0;

    cin >> size;

    int array[size] = {};

    for (int i = 0; i < size; i++){

        cin >> array[i];
    }

    int result = kadane(array, size);

    cout << "La suma maxima del subarreglo es: " << result << endl;

    return 0;
}