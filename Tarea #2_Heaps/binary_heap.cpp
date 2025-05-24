#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

// Función para "flotar" un elemento hacia arriba en el heap

void floatUp(vector<int>& heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index] < heap[parent]) {
            swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

// Función para "hundir" un elemento hacia abajo en el heap

void sinkDown(vector<int>& heap, int index) {
    int size = heap.size();
    while (2 * index + 1 < size) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;

        if (leftChild < size && heap[leftChild] < heap[smallest]) 
            smallest = leftChild;
        if (rightChild < size && heap[rightChild] < heap[smallest]) 
            smallest = rightChild;

        if (smallest != index) {
            swap(heap[index], heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

void insert(vector<int>& heap, int value) {
    heap.push_back(value);
    floatUp(heap, heap.size() - 1);
}

int extractMin(vector<int>& heap) {
    int minVal = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty())
        sinkDown(heap, 0);
    return minVal;
}

int getMin(const vector<int>& heap) {
    return heap[0];
}

void experiment(const unsigned long long N, unsigned seed){

    vector<int> heap;
    // vector<int> random_numbers; // Almacena los números generados
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);

    /* Generar y almacenar todos los números primero
    for (int i = 0; i < N; ++i) {
        random_numbers.push_back(dist(gen));
    }

    // Imprimir todos los números generados
    cout << "=== Numeros aleatorios (N = " << N << ") ===" << endl;
    for (int i = 0; i < N; ++i) {
        cout << random_numbers[i] << " ";
        if ((i + 1) % 10 == 0) cout << endl;
    }

    cout << endl << "----------------------------------" << endl;*/

    auto start_insert = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        int num = dist(gen);
        insert(heap, num);
    }
    auto end_insert = high_resolution_clock::now();
    auto duration_insert = duration_cast<microseconds>(end_insert - start_insert);

    auto start_getMin = high_resolution_clock::now();
    for (int i = 0; i < N; ++i){
        int min_val = getMin(heap); 
    }
    auto end_getMin = high_resolution_clock::now();
    auto duration_getMin = duration_cast<microseconds>(end_getMin - start_getMin);

    auto start_extract = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        if (!heap.empty()){
            extractMin(heap);
        }
    }
    auto end_extract = high_resolution_clock::now();
    auto duration_extract = duration_cast<microseconds>(end_extract - start_extract);

    cout << "N = " << N << endl;
    cout << "Tiempo inserciones: " << duration_insert.count() << " microsegundos" << endl;
    cout << "Tiempo consultas minimo: " << duration_getMin.count() << " microsegundos" << endl;
    cout << "Tiempo extracciones minimo: " << duration_extract.count() << " microsegundos" << endl;
    cout << "----------------------------------" << endl;
}

int main(){

    const unsigned seed = 123456789; // Semilla para el generador de números aleatorios

    const unsigned long long N = 1000000;

    experiment(N, seed);

    return 0;
}