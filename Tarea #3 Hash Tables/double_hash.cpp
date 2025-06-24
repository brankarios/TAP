#include <iostream>
#include <vector>
#include <utility>
#include <chrono>  
#include <random>  
#include <algorithm> 
#include <set>     
#include <functional> 
#include <tuple>    
#include <iomanip> 
#include <map>     

using namespace std;

const unsigned int TEST_RANDOM_SEED = 12345;

struct KeyValue{
    int key;
    int value;
};

enum EntryState{
    EMPTY,    
    OCCUPIED, 
    DELETED   
};

struct HashEntry{
    int key;
    int value;
    EntryState state;

    HashEntry() : key(0), value(0), state(EMPTY) {}
};


int h1(int key, int table_size){
    return key % table_size;
}

// Función para encontrar el primo más grande menor que un número
// Necesitamos un primo R para h2(key)
int findLargestPrimeLessThan(int n){
    for (int i = n - 1; i >= 2; --i){
        bool is_prime = true;
        for (int j = 2; j * j <= i; ++j){
            if (i % j == 0){
                is_prime = false;
                break;
            }
        }
        if (is_prime){
            return i;
        }
    }
    return 2; // Retorna 2 si no se encuentra un primo menor que n
}

// Segunda Función Hash (h2): R - (key % R)
// R debe ser un primo menor que el tamaño de la tabla M.
int h2(int key, int R_prime){
    return R_prime - (key % R_prime);
}


// Crea y retorna una tabla hash vacía con el tamaño especificado.
vector<HashEntry> createDoubleHashingTable(int size){
    return vector<HashEntry>(size);
}

// Inserta un par clave-valor en la tabla hash.
bool insertDoubleHashing(vector<HashEntry>& hashTable, int key, int value, int table_size, int R_prime){

    if (table_size == 0) return false;

    int initial_pos = h1(key, table_size);
    int step = h2(key, R_prime); 
    int probes = 0;             

    while (probes < table_size){
        long long current_offset = (long long)probes * step;
        int current_index = (initial_pos + current_offset) % table_size;

        if (current_index < 0) {
            current_index += table_size; 
        }

        if (hashTable[current_index].state == EMPTY || hashTable[current_index].state == DELETED) {
            hashTable[current_index].key = key;
            hashTable[current_index].value = value;
            hashTable[current_index].state = OCCUPIED;
            return true; 
        }
        
        if (hashTable[current_index].state == OCCUPIED && hashTable[current_index].key == key) {
             hashTable[current_index].value = value; 
             return true; 
        }

        probes++; 
    }

    return false;
}

// Busca un par clave valor en la tabla hash.
bool searchDoubleHashing(const vector<HashEntry>& hashTable, int key, int& outValue, int table_size, int R_prime){
    if (table_size == 0) return false;

    int initial_pos = h1(key, table_size);
    int step = h2(key, R_prime);
    int probes = 0;

    while (probes < table_size) {
        long long current_offset = (long long)probes * step;
        int current_index = (initial_pos + current_offset) % table_size;

        if (current_index < 0) {
            current_index += table_size;
        }

        if (hashTable[current_index].state == OCCUPIED) {
            if (hashTable[current_index].key == key) {
                outValue = hashTable[current_index].value;
                return true; 
            }
        } else if (hashTable[current_index].state == EMPTY) {
            return false; 
        }
        probes++;
    }
    return false; 
}

// Elimina un par clave-valor de la tabla hash.
bool deleteDoubleHashing(vector<HashEntry>& hashTable, int key, int table_size, int R_prime){

    if (table_size == 0) return false;

    int initial_pos = h1(key, table_size);
    int step = h2(key, R_prime);
    int probes = 0;

    while (probes < table_size) {
        long long current_offset = (long long)probes * step;
        int current_index = (initial_pos + current_offset) % table_size;

        if (current_index < 0) {
            current_index += table_size;
        }

        if (hashTable[current_index].state == OCCUPIED) {
            if (hashTable[current_index].key == key) {
                hashTable[current_index].state = DELETED; 
                return true; 
            }
        } else if (hashTable[current_index].state == EMPTY) {
           
            return false; 
        }

        probes++;
    }

    return false; 
}


// --- Funciones para Generar Datos de Prueba ---

// Genera un vector de pares clave-valor únicos
std::vector<KeyValue> generateRandomData(int count, int maxKeyValue, unsigned int seed) {
    std::vector<KeyValue> data;
    std::set<int> uniqueKeys; // Para asegurar claves únicas

    std::mt19937 gen(seed); // Inicializa el generador con la semilla
    std::uniform_int_distribution<> distrib(1, maxKeyValue);

    while (data.size() < count) {
        int key = distrib(gen);
        if (uniqueKeys.find(key) == uniqueKeys.end()) {
            uniqueKeys.insert(key);
            data.push_back({key, key * 10}); // Valor simple: key * 10
        }
    }
    return data;
}

// Genera claves para búsquedas/eliminaciones a partir de un conjunto ya insertado
std::vector<int> generateKeysForOperations(const std::vector<KeyValue>& insertedData, int count, bool includeNonExistent, unsigned int seed) {
    std::vector<int> keys;
    std::mt19937 gen(seed); // Inicializa el generador con la misma semilla

    int actual_count = std::min((int)insertedData.size(), count);

    std::vector<KeyValue> temp_data = insertedData;
    std::shuffle(temp_data.begin(), temp_data.end(), gen);
    for (int i = 0; i < actual_count; ++i) {
        keys.push_back(temp_data[i].key);
    }

    if (includeNonExistent && keys.size() < count) {
        std::uniform_int_distribution<> distrib_non_existent(1, 2 * 20000); // Ajusta según MAX_KEY_VALUE si es necesario
        std::set<int> existing_keys;
        for(const auto& kv : insertedData) existing_keys.insert(kv.key);

        int keysToAdd = count - keys.size();
        for (int i = 0; i < keysToAdd; ++i) {
            int nonExistentKey;
            do {
                nonExistentKey = distrib_non_existent(gen);
            } while (existing_keys.count(nonExistentKey));
            keys.push_back(nonExistentKey);
        }
    }
    std::shuffle(keys.begin(), keys.end(), gen);
    return keys;
}

// --- Funciones para Ejecutar Experimentos (Adaptadas para Double Hashing) ---

// Definiciones de tipos para las funciones de la tabla hash (Double Hashing)
using DoubleHashingHashTable = vector<HashEntry>;
using DH_InitFunc = DoubleHashingHashTable (*)(int);
// Las funciones de operación ahora necesitan R_prime como parámetro adicional
using DH_InsertFunc = function<bool(DoubleHashingHashTable&, int, int, int, int)>;
using DH_SearchFunc = function<bool(const DoubleHashingHashTable&, int, int&, int, int)>;
using DH_DeleteFunc = function<bool(DoubleHashingHashTable&, int, int, int)>;


// Función para ejecutar un escenario de prueba para una implementación dada
double runDoubleHashingExperiment(
    DH_InitFunc initFunc,
    DH_InsertFunc insertOp,
    DH_SearchFunc searchOp,
    DH_DeleteFunc deleteOp,
    int table_size,
    int R_prime, // Agregamos R_prime como parámetro
    const std::vector<KeyValue>& base_data_for_insert,
    const std::vector<int>& keys_for_search,
    const std::vector<int>& keys_for_delete,
    double insertion_ratio,
    double search_ratio,
    double delete_ratio
) {
    auto start_time = chrono::high_resolution_clock::now();

    DoubleHashingHashTable hashTable = initFunc(table_size);

    // FASE 1: Inserciones iniciales para construir la tabla base
    int successful_initial_inserts = 0;
    for (const auto& kv : base_data_for_insert) {
        if (insertOp(hashTable, kv.key, kv.value, table_size, R_prime)) { // Pasa R_prime
            successful_initial_inserts++;
        } else {
            // cout << "WARNING: Initial insert failed for key " << kv.key << " (Table full or high load factor)" << endl;
            break; 
        }
    }
    
    if (successful_initial_inserts < base_data_for_insert.size()) {
         // cout << "Warning: Only " << successful_initial_inserts << " of " << base_data_for_insert.size() << " initial elements inserted." << endl;
    }


    // FASE 2: Operaciones mixtas según los ratios
    int total_mixed_ops = base_data_for_insert.size() * 2; // Realizar el doble de operaciones mixtas que datos iniciales
    
    vector<int> current_keys_to_search = keys_for_search;
    vector<int> current_keys_to_delete = keys_for_delete;

    std::mt19937 gen_mixed_ops(TEST_RANDOM_SEED); 
    std::uniform_real_distribution<> distrib_ratio(0.0, 1.0); 
    
    std::mt19937 gen_new_key(TEST_RANDOM_SEED + 1); 
    std::uniform_int_distribution<> distrib_new_key(2000001, 3000000); 

    int current_search_idx = 0;
    int current_delete_idx = 0;

    for (int i = 0; i < total_mixed_ops; ++i) {
        double r = distrib_ratio(gen_mixed_ops); 
        int val; 

        if (r < insertion_ratio){ 
            insertOp(hashTable, distrib_new_key(gen_new_key), distrib_new_key(gen_new_key) * 10, table_size, R_prime); // Pasa R_prime
        } else if (r < insertion_ratio + search_ratio) {
            if (!current_keys_to_search.empty()) {
                searchOp(hashTable, current_keys_to_search[current_search_idx % current_keys_to_search.size()], val, table_size, R_prime); // Pasa R_prime
                current_search_idx++;
            }
        } else { 
            if (!current_keys_to_delete.empty()) {
                deleteOp(hashTable, current_keys_to_delete[current_delete_idx % current_keys_to_delete.size()], table_size, R_prime); // Pasa R_prime
                current_delete_idx++;
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
}


int main(){
    
    const int NUM_DATA_POINTS = 10000; 
    const int MAX_KEY_VALUE = 20000; 
    const int NUM_REPETITIONS = 6;    
    
    // IMPORTANTE para Double Hashing:
    // 1. El tamaño de la tabla (M) debe ser un número primo.
    // 2. Necesitamos un primo R menor que M para la segunda función hash.
    
    vector<int> table_sizes = {15013, 20003, 50021, 100000, 100003}; 
                                                                  

    // Mapa para almacenar el primo R para cada tamaño de tabla M
    map<int, int> R_primes_for_M;
    for(int m_size : table_sizes) {
        R_primes_for_M[m_size] = findLargestPrimeLessThan(m_size);
        // cout << "R prime for M=" << m_size << " is " << R_primes_for_M[m_size] << endl; // Debugging
    }

    // Generar datos base que serán los mismos para todas las pruebas
    cout << "Generando " << NUM_DATA_POINTS << " datos aleatorios iniciales con semilla " << TEST_RANDOM_SEED << "..." << endl;
    vector<KeyValue> base_data_for_insert = generateRandomData(NUM_DATA_POINTS, MAX_KEY_VALUE, TEST_RANDOM_SEED);
    vector<int> base_keys_for_search = generateKeysForOperations(base_data_for_insert, NUM_DATA_POINTS / 2, true, TEST_RANDOM_SEED);
    vector<int> base_keys_for_delete = generateKeysForOperations(base_data_for_insert, NUM_DATA_POINTS / 4, false, TEST_RANDOM_SEED);

    
    vector<tuple<string, double, double, double>> scenarios ={
        {"Dominado por Inserciones", 0.8, 0.1, 0.1},
        {"Dominado por Busquedas",  0.1, 0.8, 0.1},
        {"Dominado por Eliminaciones", 0.1, 0.1, 0.8},
        {"Uso Promedio",            0.33, 0.33, 0.34}
    };

    cout << "\n=========== PRUEBAS DE DOUBLE HASHING ===========" << endl;
    for (int current_table_size : table_sizes) {
        int R_prime = R_primes_for_M[current_table_size]; // Obtener el R_prime correspondiente
        cout << "\n--- Probando Double Hashing con TABLE_SIZE (M) = " << current_table_size << " y R_prime = " << R_prime << " ---" << endl;
        // Calculamos el factor de carga actual para la información
        double current_alpha = (double)NUM_DATA_POINTS / current_table_size;
        cout << "    (Factor de Carga Inicial ~ " << fixed << setprecision(2) << current_alpha << ")" << endl;


        for (const auto& scenario : scenarios) {
            string scenario_name = get<0>(scenario);
            double insert_ratio = get<1>(scenario);
            double search_ratio = get<2>(scenario);
            double delete_ratio = get<3>(scenario);

            double total_time_ns = 0.0;

            cout << "\n  Escenario: " << scenario_name
                      << " (I:" << (int)(insert_ratio*100) << "%, S:" << (int)(search_ratio*100) << "%, D:" << (int)(delete_ratio*100) << "%)" << endl;

            for (int i = 0; i < NUM_REPETITIONS; ++i) {
                // Aquí llamamos a la función de experimento específica de Double Hashing
                double current_run_time = runDoubleHashingExperiment(
                    createDoubleHashingTable,
                    insertDoubleHashing,
                    searchDoubleHashing,
                    deleteDoubleHashing,
                    current_table_size,
                    R_prime, // Pasa R_prime al experimento
                    base_data_for_insert,
                    base_keys_for_search,
                    base_keys_for_delete,
                    insert_ratio,
                    search_ratio,
                    delete_ratio
                );
                total_time_ns += current_run_time;
                cout << "    Repeticion " << (i + 1) << ": " << current_run_time / 1e6 << " ms" << endl;
            }

            double average_time_ms = (total_time_ns / NUM_REPETITIONS) / 1e6; // Convertir a milisegundos
            cout << "  Tiempo promedio para " << scenario_name << ": " << average_time_ms << " ms" << endl;
        }
    }

    cout << "\n=======================================================\n" << endl;

    return 0;
}