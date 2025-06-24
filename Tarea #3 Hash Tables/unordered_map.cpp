#include <iostream>
#include <vector>
#include <chrono>       
#include <random>       
#include <algorithm>    
#include <set>          
#include <functional>   
#include <tuple>        
#include <iomanip>      
#include <unordered_map>

using namespace std;

const unsigned int TEST_RANDOM_SEED = 12345;

struct KeyValue{
    int key;
    int value;
};

// --- Funciones para Generar Datos de Prueba ---

// Genera un vector de pares clave-valor únicos
std::vector<KeyValue> generateRandomData(int count, int maxKeyValue, unsigned int seed) {
    std::vector<KeyValue> data;
    std::set<int> uniqueKeys;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distrib(1, maxKeyValue);
    while (data.size() < count) {
        int key = distrib(gen);
        if (uniqueKeys.find(key) == uniqueKeys.end()) {
            uniqueKeys.insert(key);
            data.push_back({key, key * 10});
        }
    }
    return data;
}

// Genera claves para búsquedas/eliminaciones a partir de un conjunto ya insertado
std::vector<int> generateKeysForOperations(const std::vector<KeyValue>& insertedData, int count, bool includeNonExistent, unsigned int seed) {
    std::vector<int> keys;
    std::mt19937 gen(seed);
    int actual_count = std::min((int)insertedData.size(), count);
    std::vector<KeyValue> temp_data = insertedData;
    std::shuffle(temp_data.begin(), temp_data.end(), gen);
    for (int i = 0; i < actual_count; ++i) {
        keys.push_back(temp_data[i].key);
    }
    if (includeNonExistent && keys.size() < count) {
        std::uniform_int_distribution<> distrib_non_existent(1, 2 * 20000);
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

// --- Implementación y Pruebas para std::unordered_map ---

// Wrapper function for std::unordered_map insert
bool insertUnorderedMap(unordered_map<int, int>& umap, int key, int value){
    umap[key] = value; 
    return true; 
}

bool searchUnorderedMap(const unordered_map<int, int>& umap, int key, int& outValue) {
    auto it = umap.find(key);
    if (it != umap.end()) {
        outValue = it->second;
        return true;
    }
    return false;
}

bool deleteUnorderedMap(unordered_map<int, int>& umap, int key) {
    return umap.erase(key) > 0; 
}

double runUnorderedMapExperiment(
    const std::vector<KeyValue>& base_data_for_insert,
    const std::vector<int>& keys_for_search,
    const std::vector<int>& keys_for_delete,
    double insertion_ratio,
    double search_ratio,
    double delete_ratio
) {
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<int, int> umap;

    for (const auto& kv : base_data_for_insert) {
        insertUnorderedMap(umap, kv.key, kv.value);
    }
    
    int total_mixed_ops = base_data_for_insert.size() * 2; 
    
    std::mt19937 gen_mixed_ops(TEST_RANDOM_SEED);
    std::uniform_real_distribution<> distrib_ratio(0.0, 1.0);
    
    // Generador de nuevas claves para inserciones en fase 2
    std::mt19937 gen_new_key(TEST_RANDOM_SEED + 1);
    std::uniform_int_distribution<> distrib_new_key(2000001, 3000000); 

    int current_search_idx = 0;
    int current_delete_idx = 0;

    for (int i = 0; i < total_mixed_ops; ++i) {
        double r = distrib_ratio(gen_mixed_ops);
        int val; 

        if (r < insertion_ratio) { // Realizar inserción
            insertUnorderedMap(umap, distrib_new_key(gen_new_key), distrib_new_key(gen_new_key) * 10);
        } else if (r < insertion_ratio + search_ratio) { // Realizar búsqueda
            if (!keys_for_search.empty()) {
                searchUnorderedMap(umap, keys_for_search[current_search_idx % keys_for_search.size()], val);
                current_search_idx++;
            }
        } else { // Realizar eliminación
            if (!keys_for_delete.empty()) {
                deleteUnorderedMap(umap, keys_for_delete[current_delete_idx % keys_for_delete.size()]);
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

    cout << "\n=========== PRUEBAS DE std::unordered_map ===========" << endl;
    for (const auto& scenario : scenarios) {
        string scenario_name = get<0>(scenario);
        double insert_ratio = get<1>(scenario);
        double search_ratio = get<2>(scenario);
        double delete_ratio = get<3>(scenario);
        double total_time_ns = 0.0;

        cout << "\n  Escenario: " << scenario_name
                  << " (I:" << (int)(insert_ratio*100) << "%, S:" << (int)(search_ratio*100) << "%, D:" << (int)(delete_ratio*100) << "%)" << endl;
        cout << "    (Factor de Carga manejado dinamicamente por std::unordered_map)" << endl;

        for (int i = 0; i < NUM_REPETITIONS; ++i) {
            double current_run_time = runUnorderedMapExperiment(
                base_data_for_insert, base_keys_for_search, base_keys_for_delete,
                insert_ratio, search_ratio, delete_ratio
            );
            total_time_ns += current_run_time;
            cout << "    Repeticion " << (i + 1) << ": " << current_run_time / 1e6 << " ms" << endl;
        }
        double average_time_ms = (total_time_ns / NUM_REPETITIONS) / 1e6;
        cout << "  Tiempo promedio para " << scenario_name << ": " << average_time_ms << " ms" << endl;
    }

    cout << "\n=======================================================\n" << endl;

    return 0;
}