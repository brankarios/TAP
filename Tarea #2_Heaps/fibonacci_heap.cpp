#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

struct FibonacciNode{

    int key;
    int degree;
    bool marked;
    FibonacciNode* parent;
    FibonacciNode* child;
    FibonacciNode* left;
    FibonacciNode* right;

};

FibonacciNode* minNode = nullptr;
int numNodes = 0;

FibonacciNode* createFibonacciNode(int key) {
    FibonacciNode* node = new FibonacciNode;
    node->key = key;
    node->degree = 0;
    node->marked = false;
    node->parent = nullptr;
    node->child = nullptr;
    node->left = node;
    node->right = node;
    return node;
}

void insertToRootList(FibonacciNode* node) {
    if (minNode == nullptr) {
        minNode = node;
    } else {
        node->right = minNode->right;
        node->left = minNode;
        minNode->right->left = node;
        minNode->right = node;
    }
}

// Función para enlazar dos nodos

void linkFibonacciNodes(FibonacciNode* y, FibonacciNode* x) {
    // Remover y de la lista de raíces
    y->left->right = y->right;
    y->right->left = y->left;
    
    // Hacer y hijo de x
    y->parent = x;
    if (x->child == nullptr) {
        x->child = y;
        y->left = y->right = y;
    } else {
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    
    x->degree++;
    y->marked = false;
}

// Función para reorganizar el heap después de remover el nodo mínimo

void consolidate() {
    int maxDegree = static_cast<int>(log2(numNodes)) + 1;
    vector<FibonacciNode*> degreeTable(maxDegree, nullptr);
    
    FibonacciNode* current = minNode;
    unordered_set<FibonacciNode*> processedNodes;
    
    do {
        FibonacciNode* x = current;
        int degree = x->degree;
        
        while (degreeTable[degree] != nullptr) {
            FibonacciNode* y = degreeTable[degree];
            if (x->key > y->key) {
                swap(x, y);
            }
            linkFibonacciNodes(y, x);
            degreeTable[degree] = nullptr;
            degree++;
        }
        
        degreeTable[degree] = x;
        processedNodes.insert(x);
        current = current->right;
    } while (processedNodes.find(current) == processedNodes.end());
    
    minNode = nullptr;
    for (FibonacciNode* node : degreeTable) {
        if (node != nullptr) {
            if (minNode == nullptr) {
                minNode = node;
                minNode->left = minNode->right = minNode;
            } else {
                insertToRootList(node);
                if (node->key < minNode->key) {
                    minNode = node;
                }
            }
        }
    }
}

// Función para cortar un nodo y moverlo a la lista de raíces
void cut(FibonacciNode* x, FibonacciNode* y) {
    // Remover x de la lista de hijos de y
    if (x->right == x) {
        y->child = nullptr;
    } else {
        y->child = x->right;
        x->left->right = x->right;
        x->right->left = x->left;
    }
    
    y->degree--;
    insertToRootList(x);
    x->parent = nullptr;
    x->marked = false;
}

// Función para realizar cortes en cascada
void cascadingCut(FibonacciNode* y) {
    FibonacciNode* z = y->parent;
    if (z != nullptr) {
        if (!y->marked) {
            y->marked = true;
        } else {
            cut(y, z);
            cascadingCut(z);
        }
    }
}

void fibonacciInsert(int key) {
    FibonacciNode* node = createFibonacciNode(key);
    
    if (minNode == nullptr) {
        minNode = node;
    } else {
        insertToRootList(node);
        if (key < minNode->key) {
            minNode = node;
        }
    }
    numNodes++;
}

int fibonacciGetMin(){
    if (minNode == nullptr) return -1;
    return minNode->key;
}

int fibonacciExtractMin() {
    FibonacciNode* z = minNode;
    if (z == nullptr) return -1;
    
    // Agregar hijos a la lista de raíces
    if (z->child != nullptr) {
        FibonacciNode* child = z->child;
        do {
            FibonacciNode* nextChild = child->right;
            insertToRootList(child);
            child->parent = nullptr;
            child = nextChild;
        } while (child != z->child);
    }
    
    // Remover z de la lista de raíces
    z->left->right = z->right;
    z->right->left = z->left;
    
    int minKey = z->key;
    if (z == z->right) {
        minNode = nullptr;
    } else {
        minNode = z->right;
        consolidate();
    }
    
    delete z;
    numNodes--;
    return minKey;
}

void fibonacciUnion(FibonacciNode* otherMin){
    
    if (otherMin == nullptr) return;
    
    if (minNode == nullptr) {
        minNode = otherMin;
    } else {
        FibonacciNode* thisRight = minNode->right;
        FibonacciNode* otherLeft = otherMin->left;
        
        minNode->right = otherMin;
        otherMin->left = minNode;
        thisRight->left = otherLeft;
        otherLeft->right = thisRight;
        
        if (otherMin->key < minNode->key) {
            minNode = otherMin;
        }
    }
}

// Función para limpiar el heap
void clearFibonacciHeap() {
    if (minNode == nullptr) return;
    
    vector<FibonacciNode*> nodesToDelete;
    unordered_set<FibonacciNode*> visited;
    
    FibonacciNode* current = minNode;
    do {
        if (visited.find(current) == visited.end()) {
            visited.insert(current);
            if (current->child != nullptr) {
                FibonacciNode* child = current->child;
                do {
                    nodesToDelete.push_back(child);
                    child = child->right;
                } while (child != current->child);
            }
            nodesToDelete.push_back(current);
        }
        current = current->right;
    } while (current != minNode && visited.find(current) == visited.end());
    
    for (FibonacciNode* node : nodesToDelete) {
        delete node;
    }
    
    minNode = nullptr;
    numNodes = 0;
}

// Función para crear un heap aleatorio

FibonacciNode* randomFibonacci(int size, mt19937& gen, uniform_int_distribution<int>& dist) {
    FibonacciNode* localMin = nullptr;
    int localNum = 0;
    
    for (int i = 0; i < size; ++i) {
        FibonacciNode* node = createFibonacciNode(dist(gen));
        
        if (localMin == nullptr) {
            localMin = node;
        } else {
            node->right = localMin->right;
            node->left = localMin;
            localMin->right->left = node;
            localMin->right = node;
            
            if (node->key < localMin->key) {
                localMin = node;
            }
        }
        localNum++;
    }
    
    numNodes += localNum;
    return localMin;
}

void experiment(const unsigned long long N, mt19937& gen, uniform_int_distribution<int>& dist){

    clearFibonacciHeap();
    
    auto start_insert = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        fibonacciInsert(dist(gen));
    }
    auto end_insert = high_resolution_clock::now();
    auto duration_insert = duration_cast<microseconds>(end_insert - start_insert);

    auto start_getMin = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        int min_val = fibonacciGetMin();
    }
    auto end_getMin = high_resolution_clock::now();
    auto duration_getMin = duration_cast<microseconds>(end_getMin - start_getMin);

    auto start_extract = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        fibonacciExtractMin();
    }
    auto end_extract = high_resolution_clock::now();
    auto duration_extract = duration_cast<microseconds>(end_extract - start_extract);

    auto start_union = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        int heap1_size = dist(gen) % 100 + 1;
        int heap2_size = dist(gen) % 100 + 1;
        
        FibonacciNode* heap1 = randomFibonacci(heap1_size, gen, dist);
        FibonacciNode* heap2 = randomFibonacci(heap2_size, gen, dist);
        
        fibonacciUnion(heap1);
        fibonacciUnion(heap2);
    }
    auto end_union = high_resolution_clock::now();
    auto duration_union = duration_cast<microseconds>(end_union - start_union);

    cout << "N = " << N << endl;
    cout << "Tiempo inserciones: " << duration_insert.count() << " microsegundos" << endl;
    cout << "Tiempo consultas minimo: " << duration_getMin.count() << " microsegundos" << endl;
    cout << "Tiempo extracciones minimo: " << duration_extract.count() << " microsegundos" << endl;
    cout << "Tiempo uniones: " << duration_union.count() << " microsegundos" << endl;
    cout << "----------------------------------" << endl;
}

int main(){

    const unsigned seed = 123456789; // Misma semilla que en los dos heaps anteriores
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);
    const unsigned long long N = 100000;

    experiment(N, gen, dist);
    
    clearFibonacciHeap();
    return 0;

}