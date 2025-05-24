#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;

struct BinomialNode {
    int key;
    int degree;
    BinomialNode* parent;
    BinomialNode* child;
    BinomialNode* sibling;
};

BinomialNode* createNode(int key) {
    BinomialNode* node = new BinomialNode;
    node->key = key;
    node->degree = 0;
    node->parent = node->child = node->sibling = nullptr;
    return node;
}

void linkTrees(BinomialNode* y, BinomialNode* z) {
    y->parent = z;
    y->sibling = z->child;
    z->child = y;
    z->degree++;
}

BinomialNode* mergeRootLists(BinomialNode* h1, BinomialNode* h2) {
    if (!h1) return h2;
    if (!h2) return h1;
    
    BinomialNode* head = nullptr;
    BinomialNode* tail = nullptr;
    BinomialNode* a = h1;
    BinomialNode* b = h2;

    if (a->degree <= b->degree) {
        head = a;
        a = a->sibling;
    } else {
        head = b;
        b = b->sibling;
    }
    tail = head;

    while (a && b) {
        if (a->degree <= b->degree) {
            tail->sibling = a;
            a = a->sibling;
        } else {
            tail->sibling = b;
            b = b->sibling;
        }
        tail = tail->sibling;
    }
    tail->sibling = (a ? a : b);
    return head;
}

BinomialNode* unionHeaps(BinomialNode* h1, BinomialNode* h2) {
    BinomialNode* newHead = mergeRootLists(h1, h2);
    if (!newHead) return nullptr;

    BinomialNode* prev = nullptr;
    BinomialNode* curr = newHead;
    BinomialNode* next = curr->sibling;

    while (next) {
        if ((curr->degree != next->degree) || 
            (next->sibling && next->sibling->degree == curr->degree)) {
            prev = curr;
            curr = next;
        } else {
            if (curr->key <= next->key) {
                curr->sibling = next->sibling;
                linkTrees(next, curr);
            } else {
                if (!prev)
                    newHead = next;
                else
                    prev->sibling = next;
                linkTrees(curr, next);
                curr = next;
            }
        }
        next = curr->sibling;
    }
    return newHead;
}

BinomialNode* reverseList(BinomialNode* node) {
    BinomialNode* prev = nullptr;
    while (node) {
        BinomialNode* next = node->sibling;
        node->sibling = prev;
        node->parent = nullptr;
        prev = node;
        node = next;
    }
    return prev;
}

void insert(BinomialNode*& heap, int key) {
    BinomialNode* node = createNode(key);
    heap = unionHeaps(heap, node);
}

int getMin(BinomialNode* heap) {
    if (!heap) return -1;

    BinomialNode* x = heap;
    int min = x->key;
    x = x->sibling;
    while (x) {
        if (x->key < min) {
            min = x->key;
        }
        x = x->sibling;
    }
    return min;
}

int extractMin(BinomialNode*& heap) {
    if (!heap) return -1;

    BinomialNode* minPrev = nullptr;
    BinomialNode* minNode = heap;
    BinomialNode* prev = nullptr;
    BinomialNode* curr = heap;
    int min = curr->key;

    while (curr) {
        if (curr->key < min) {
            min = curr->key;
            minPrev = prev;
            minNode = curr;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (minPrev)
        minPrev->sibling = minNode->sibling;
    else
        heap = minNode->sibling;

    BinomialNode* child = reverseList(minNode->child);
    heap = unionHeaps(heap, child);

    int minKey = minNode->key;
    delete minNode;
    return minKey;
}

void clear(BinomialNode* node){

    if (!node) return;
    clear(node->child);
    clear(node->sibling);
    delete node;
}

BinomialNode* createRandomHeap(int size, mt19937& gen, uniform_int_distribution<int>& dist){

    BinomialNode* heap = nullptr;
    for (int i = 0; i < size; ++i) {
        insert(heap, dist(gen));
    }
    return heap;
}

void experiment(const unsigned long long N, mt19937& gen, uniform_int_distribution<int>& dist){

    BinomialNode* heap = nullptr;
    
    auto start_insert = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        insert(heap, dist(gen));
    }
    auto end_insert = high_resolution_clock::now();
    auto duration_insert = duration_cast<microseconds>(end_insert - start_insert);

    auto start_getMin = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        int min_val = getMin(heap);
    }
    auto end_getMin = high_resolution_clock::now();
    auto duration_getMin = duration_cast<microseconds>(end_getMin - start_getMin);

    auto start_extract = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        if (!heap) break;
        extractMin(heap);
    }
    auto end_extract = high_resolution_clock::now();
    auto duration_extract = duration_cast<microseconds>(end_extract - start_extract);

    auto start_union = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        int heap1_size = dist(gen) % 100 + 1;
        int heap2_size = dist(gen) % 100 + 1;
        BinomialNode* heap1 = createRandomHeap(heap1_size, gen, dist);
        BinomialNode* heap2 = createRandomHeap(heap2_size, gen, dist);
        BinomialNode* merged = unionHeaps(heap1, heap2);
        clear(merged);
    }
    auto end_union = high_resolution_clock::now();
    auto duration_union = duration_cast<microseconds>(end_union - start_union);

    cout << "N = " << N << endl;
    cout << "Tiempo inserciones: " << duration_insert.count() << " microsegundos" << endl;
    cout << "Tiempo consultas minimo: " << duration_getMin.count() << " microsegundos" << endl;
    cout << "Tiempo extracciones minimo: " << duration_extract.count() << " microsegundos" << endl;
    cout << "Tiempo uniones: " << duration_union.count() << " microsegundos" << endl;
    cout << "----------------------------------" << endl;

    clear(heap);
}

int main(){

    const unsigned seed = 123456789; // Misma semilla que en el heap binario
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(1, 1000000);
    const unsigned long long N = 100000; 
    
    experiment(N, gen, dist);
    

    return 0;
}