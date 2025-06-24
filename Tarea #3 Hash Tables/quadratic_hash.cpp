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

using namespace std;

const unsigned int TEST_RANDOM_SEED = 12345;

struct KeyValue {
    int key;
    int value;
};

enum EntryState {
    EMPTY,    
    OCCUPIED, 
    DELETED   
};

struct HashEntry {
    int key;
    int value;
    EntryState state;

    HashEntry() : key(0), value(0), state(EMPTY) {}
};

int hashFunction(int key, int table_size) {
    return key % table_size;
}


const int C1 = 0; // Coeficientes para el desplazamiento cuadrático
const int C2 = 1;

// Crea y retorna una tabla hash vacía con el tamaño especificado
vector<HashEntry> createQuadraticProbingTable(int size) {
    return vector<HashEntry>(size);
}

// Inserta un par clave-valor en la tabla hash.
bool insertQuadraticProbing(vector<HashEntry>& hashTable, int key, int value, int table_size){

    if (table_size == 0) return false;

    int initial_hash = hashFunction(key, table_size);
    int probes = 0; 

    while (probes < table_size){ 
        // Fórmula para el desplazamiento cuadrático
        long long offset = (long long)C1 * probes + (long long)C2 * probes * probes;
        
        int current_index = (initial_hash + offset) % table_size;
        if (current_index < 0){ 
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
bool searchQuadraticProbing(const vector<HashEntry>& hashTable, int key, int& outValue, int table_size){

    // Misma lógica que en linear probing, con la diferencia del desplazamiento que ahora es cuadrático
    if (table_size == 0) return false;

    int initial_hash = hashFunction(key, table_size);
    int probes = 0; 

    while (probes < table_size){ 

        long long offset = (long long)C1 * probes + (long long)C2 * probes * probes;
        int current_index = (initial_hash + offset) % table_size;
        if (current_index < 0) {
            current_index += table_size;
        }

        if (hashTable[current_index].state == OCCUPIED) {
            if (hashTable[current_index].key == key) {
                outValue = hashTable[current_index].value;
                return true; // Clave encontrada
            }
        } else if (hashTable[current_index].state == EMPTY) {
         
            return false;
        }        
        probes++;
    }

    return false; 
}

// Elimina un par clave-valor de la tabla hash.
bool deleteQuadraticProbing(vector<HashEntry>& hashTable, int key, int table_size){

    // Misma lógica que en linear probing

    if (table_size == 0) return false;

    int initial_hash = hashFunction(key, table_size);
    int probes = 0; 

    while (probes < table_size){ 

        long long offset = (long long)C1 * probes + (long long)C2 * probes * probes;
        int current_index = (initial_hash + offset) % table_size;
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

    return false; // Clave no encontrada para eliminar
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

// --- Funciones para Ejecutar Experimentos (Adaptadas para Quadratic Probing) ---

// Definiciones de tipos para las funciones de la tabla hash (Quadratic Probing)
using QuadraticProbingHashTable = vector<HashEntry>;
using QP_InitFunc = QuadraticProbingHashTable (*)(int);
using QP_InsertFunc = function<bool(QuadraticProbingHashTable&, int, int, int)>;
using QP_SearchFunc = function<bool(const QuadraticProbingHashTable&, int, int&, int)>;
using QP_DeleteFunc = function<bool(QuadraticProbingHashTable&, int, int)>;


// Función para ejecutar un escenario de prueba para una implementación dada
double runQuadraticProbingExperiment(
    QP_InitFunc initFunc,
    QP_InsertFunc insertOp,
    QP_SearchFunc searchOp,
    QP_DeleteFunc deleteOp,
    int table_size,
    const std::vector<KeyValue>& base_data_for_insert, // Datos para la fase de inserción inicial
    const std::vector<int>& keys_for_search,           // Claves para buscar
    const std::vector<int>& keys_for_delete,           // Claves para eliminar
    double insertion_ratio,
    double search_ratio,
    double delete_ratio
) {
    auto start_time = chrono::high_resolution_clock::now();

    QuadraticProbingHashTable hashTable = initFunc(table_size);

    // FASE 1: Inserciones iniciales para construir la tabla base
    int successful_initial_inserts = 0;
    for (const auto& kv : base_data_for_insert) {
        if (insertOp(hashTable, kv.key, kv.value, table_size)) {
            successful_initial_inserts++;
        } else {
            // WARNING: Initial insert failed. This happens if the table is too small for Linear/Quadratic Probing.
            // For proper Quadratic Probing, table_size should be prime and alpha <= 0.5.
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
        int val; // Para búsquedas

        if (r < insertion_ratio) { // Realizar inserción
            insertOp(hashTable, distrib_new_key(gen_new_key), distrib_new_key(gen_new_key) * 10, table_size);
        } else if (r < insertion_ratio + search_ratio) { // Realizar búsqueda
            if (!current_keys_to_search.empty()) {
                searchOp(hashTable, current_keys_to_search[current_search_idx % current_keys_to_search.size()], val, table_size);
                current_search_idx++;
            }
        } else { // Realizar eliminación
            if (!current_keys_to_delete.empty()) {
                deleteOp(hashTable, current_keys_to_delete[current_delete_idx % current_keys_to_delete.size()], table_size);
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
    const int NUM_REPETITIONS = 3;    
    
    // IMPORTANTE para Quadratic Hash:
    // 1. El tamaño de la tabla (M) debe ser un número primo
    // 2. El factor de carga (N/M) debe ser <= 0.5 para garantizar que siempre se encuentre un espacio

    vector<int> table_sizes = {20003, 25003, 50021, 100000, 100003}; 
                                                           
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

    cout << "\n=========== PRUEBAS DE QUADRATIC PROBING ===========" << endl;
    for (int current_table_size : table_sizes) {
        cout << "\n--- Probando Quadratic Probing con TABLE_SIZE (M) = " << current_table_size << " ---" << endl;
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
                // Aquí llamamos a la función de experimento específica de Quadratic Probing
                double current_run_time = runQuadraticProbingExperiment(
                    createQuadraticProbingTable,
                    insertQuadraticProbing,
                    searchQuadraticProbing,
                    deleteQuadraticProbing,
                    current_table_size,
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