#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <chrono>  
#include <random>   
#include <algorithm> 
#include <numeric>   
#include <set>      
#include <functional> 
#include <tuple>    

using namespace std;

const unsigned int TEST_RANDOM_SEED = 12345; 

struct KeyValue {
    int key;
    int value;
};

int hashFunction(int key, int table_size){
    return key % table_size;
}

// Create HashTable: Crea y retorna una tabla hash vacía con el tamaño especificado.
vector<list<pair<int, int>>> createSeparateChainingTable(int size){
    return vector<list<pair<int, int>>>(size);
}

// Insertar: Inserta un par clave-valor en la tabla hash.
void insertSeparateChaining(vector<list<pair<int, int>>>& hashTable, int key, int value, int table_size) {
    int index = hashFunction(key, table_size);
    hashTable[index].push_back({key, value});
}

// Buscar: Busca un par clave valor en la tabla hash.
bool searchSeparateChaining(const vector<list<pair<int, int>>>& hashTable, int key, int& outValue, int table_size) {
    int index = hashFunction(key, table_size);
    for (const auto& pair : hashTable[index]) {
        if (pair.first == key) {
            outValue = pair.second;
            return true;
        }
    }
    return false;
}

// Eliminar: Elimina un par clave-valor de la tabla hash.
bool deleteSeparateChaining(vector<list<pair<int, int>>>& hashTable, int key, int table_size) {
    int index = hashFunction(key, table_size);
    for (auto it = hashTable[index].begin(); it != hashTable[index].end(); ++it) {
        if (it->first == key) {
            hashTable[index].erase(it);
            // cout << "Deleted key " << key << " from index " << index << endl; // Desactivado para pruebas de rendimiento
            return true;
        }
    }
    return false;
}

// Imprime el contenido de la tabla hash
/*void printHashTable(const vector<list<pair<int, int>>>& hashTable) {
    cout << "\n--- Current State of the Hash Table (Separate Chaining) ---" << endl;
    for (int i = 0; i < hashTable.size(); ++i) {
        cout << "Index " << i << ": ";
        if (hashTable[i].empty()) {
            cout << "Empty";
        } else {
            for (const auto& pair : hashTable[i]) {
                cout << "[" << pair.first << ":" << pair.second << "] ";
            }
        }
        cout << endl;
    }
    cout << "---------------------------------------------------------------------" << endl;
}*/

// --- Funciones para Generar Datos de Prueba ---

// Genera un vector de pares clave-valor únicos
// Ahora toma la semilla como parámetro
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
// Ahora toma la semilla como parámetro
std::vector<int> generateKeysForOperations(const std::vector<KeyValue>& insertedData, int count, bool includeNonExistent, unsigned int seed) {
    std::vector<int> keys;
    std::mt19937 gen(seed); // Inicializa el generador con la misma semilla

    // Asegurarse de que 'count' no exceda los datos disponibles si no se incluyen no existentes.
    int actual_count = std::min((int)insertedData.size(), count);

    // Selecciona 'actual_count' claves existentes aleatoriamente
    std::vector<KeyValue> temp_data = insertedData;
    std::shuffle(temp_data.begin(), temp_data.end(), gen); // Pasa el generador para shuffle
    for (int i = 0; i < actual_count; ++i) {
        keys.push_back(temp_data[i].key);
    }

    // Si se necesitan más claves y se permite incluir no existentes
    if (includeNonExistent && keys.size() < count) {
        std::uniform_int_distribution<> distrib_non_existent(1, 2 * 200000); // Rango más amplio para no existentes
        std::set<int> existing_keys;
        for(const auto& kv : insertedData) existing_keys.insert(kv.key);

        int keysToAdd = count - keys.size();
        for (int i = 0; i < keysToAdd; ++i) {
            int nonExistentKey;
            do {
                nonExistentKey = distrib_non_existent(gen);
            } while (existing_keys.count(nonExistentKey)); // Asegurarse de que sea realmente nueva
            keys.push_back(nonExistentKey);
        }
    }
    std::shuffle(keys.begin(), keys.end(), gen); // Mezclar para aleatorizar el orden de las operaciones
    return keys;
}

// --- Funciones para Ejecutar Experimentos ---

// Definiciones de tipos para las funciones de la tabla hash (Separate Chaining)
using SeparateChainingHashTable = vector<list<pair<int, int>>>;
using SC_InitFunc = SeparateChainingHashTable (*)(int);
using SC_InsertFunc = function<void(SeparateChainingHashTable&, int, int, int)>;
using SC_SearchFunc = function<bool(const SeparateChainingHashTable&, int, int&, int)>;
using SC_DeleteFunc = function<bool(SeparateChainingHashTable&, int, int)>;


// Función para ejecutar un escenario de prueba para una implementación dada
double runSeparateChainingExperiment(
    SC_InitFunc initFunc,
    SC_InsertFunc insertOp,
    SC_SearchFunc searchOp,
    SC_DeleteFunc deleteOp,
    int table_size,
    const std::vector<KeyValue>& base_data_for_insert, // Datos para la fase de inserción inicial
    const std::vector<int>& keys_for_search,           // Claves para buscar
    const std::vector<int>& keys_for_delete,           // Claves para eliminar
    double insertion_ratio,
    double search_ratio,
    double delete_ratio
) {
    auto start_time = chrono::high_resolution_clock::now();

    SeparateChainingHashTable hashTable = initFunc(table_size);

    
    for (const auto& kv : base_data_for_insert) {
        insertOp(hashTable, kv.key, kv.value, table_size);
    }

    
    int total_mixed_ops = base_data_for_insert.size() * 2; 
    
    
    vector<int> current_keys_to_search = keys_for_search;
    vector<int> current_keys_to_delete = keys_for_delete;

    // Usar la misma semilla para las operaciones mixtas para reproducibilidad
    std::mt19937 gen_mixed_ops(TEST_RANDOM_SEED); // Generador para la selección de operaciones
    std::uniform_real_distribution<> distrib_ratio(0.0, 1.0); // Para seleccionar la operación
    // Generador para nuevas claves, con una semilla diferente para que no interfiera con las secuencias principales
    std::mt19937 gen_new_key(TEST_RANDOM_SEED + 1); 
    std::uniform_int_distribution<> distrib_new_key(2000001, 3000000); // Para nuevas claves en inserciones mixtas

    int current_insert_idx = 0; // Para iterar sobre data_to_insert para nuevas inserciones
    int current_search_idx = 0;
    int current_delete_idx = 0;

    for (int i = 0; i < total_mixed_ops; ++i) {
        double r = distrib_ratio(gen_mixed_ops); // Usar gen_mixed_ops para la decisión
        int val; // Para búsquedas

        if (r < insertion_ratio) { // Realizar inserción
            // Generar nuevas claves para las inserciones en la fase mixta
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
    
    const int NUM_DATA_POINTS = 10000; // Cantidad de elementos base para insertar
    const int MAX_KEY_VALUE = 20000;  // Rango de las claves iniciales
    const int NUM_REPETITIONS = 6;    // Repetir cada experimento N veces
    
    vector<int> table_sizes = {1000, 20000, 50000, 100000}; 

    // Generar datos base que serán los mismos para todas las pruebas
    cout << "Generando " << NUM_DATA_POINTS << " datos aleatorios iniciales con semilla " << TEST_RANDOM_SEED << "..." << endl;
    // Pasa la semilla a las funciones de generación de datos
    vector<KeyValue> base_data_for_insert = generateRandomData(NUM_DATA_POINTS, MAX_KEY_VALUE, TEST_RANDOM_SEED);
    // Para búsquedas: un mix de existentes y no existentes
    vector<int> base_keys_for_search = generateKeysForOperations(base_data_for_insert, NUM_DATA_POINTS / 2, true, TEST_RANDOM_SEED);
    // Para eliminaciones: un subconjunto de las claves que sí existen
    vector<int> base_keys_for_delete = generateKeysForOperations(base_data_for_insert, NUM_DATA_POINTS / 4, false, TEST_RANDOM_SEED);

    
    vector<tuple<string, double, double, double>> scenarios ={
        {"Dominado por Inserciones", 0.8, 0.1, 0.1},
        {"Dominado por Busquedas",  0.1, 0.8, 0.1},
        {"Dominado por Eliminaciones", 0.1, 0.1, 0.8},
        {"Uso Promedio",            0.33, 0.33, 0.34}
    };

    cout << "\n=========== PRUEBAS DE SEPARATE CHAINING ===========" << endl;
    for (int current_table_size : table_sizes) {
        cout << "\n--- Probando Separate Chaining con TABLE_SIZE (M) = " << current_table_size << " ---" << endl;

        for (const auto& scenario : scenarios) {
            string scenario_name = get<0>(scenario);
            double insert_ratio = get<1>(scenario);
            double search_ratio = get<2>(scenario);
            double delete_ratio = get<3>(scenario);

            double total_time_ns = 0.0;

            cout << "\n  Escenario: " << scenario_name
                      << " (I:" << (int)(insert_ratio*100) << "%, S:" << (int)(search_ratio*100) << "%, D:" << (int)(delete_ratio*100) << "%)" << endl;

            for (int i = 0; i < NUM_REPETITIONS; ++i) {
                double current_run_time = runSeparateChainingExperiment(
                    createSeparateChainingTable,
                    insertSeparateChaining,
                    searchSeparateChaining,
                    deleteSeparateChaining,
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