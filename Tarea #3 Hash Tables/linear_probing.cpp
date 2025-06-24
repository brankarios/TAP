#include <iostream>
#include <vector>
#include <utility> 
#include <chrono>  
#include <random>  
#include <algorithm> 
#include <set>     
#include <functional> 
#include <tuple>    

using namespace std;

const unsigned int TEST_RANDOM_SEED = 12345;

struct KeyValue{
    int key;
    int value;
};

// Estados para las entradas de Linear Probing 
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

int hashFunction(int key, int table_size){
    return key % table_size;
}

// Crea y retorna una tabla hash vacía con el tamaño especificado. Con un vector de HashEntry es suficiente
vector<HashEntry> createLinearProbingTable(int size){
    return vector<HashEntry>(size); 
}

// Inserta un par clave-valor en la tabla hash
bool insertLinearProbing(vector<HashEntry>& hashTable, int key, int value, int table_size){

    if (table_size == 0) return false; 

    int initial_index = hashFunction(key, table_size);
    int current_index = initial_index;
    int probes = 0; // Para contar las pruebas y evitar bucles infinitos en tablas llenas

    do{
        // Si la celda está vacía o marcada como DELETED, se inserta el elemento
        if (hashTable[current_index].state == EMPTY || hashTable[current_index].state == DELETED){
            hashTable[current_index].key = key;
            hashTable[current_index].value = value;
            hashTable[current_index].state = OCCUPIED;
            return true;
        }
        
        // Si la clave ya existe, no la insertamos (partimos del hecho de que no debe haber duplicados)
        if (hashTable[current_index].state == OCCUPIED && hashTable[current_index].key == key) {
             hashTable[current_index].value = value; // Actualizar el valor solo para hacer algo
             return true; 
        }

        current_index = (current_index + 1) % table_size; // Pasamos a la siguiente posición para insertar
        probes++;

        // Si hemos recorrido toda la tabla (o superado un límite razonable), no podemos insertar
        // Esto es útil para evitar los bucles infinitos en tablas que estén llenas
        if (probes >= table_size){
            return false; // No haremos ningún resize para efectos de las pruebas
        }

    } while (current_index != initial_index); // Se busca un espacio hasta haber recorrido la tabla circularmente

    return false;
}

// Busca un par clave valor en la tabla hash
bool searchLinearProbing(const vector<HashEntry>& hashTable, int key, int& outValue, int table_size){

    if (table_size == 0) return false;
    int initial_index = hashFunction(key, table_size);
    int current_index = initial_index;
    int probes = 0;

    do{
        if (hashTable[current_index].state == OCCUPIED){
            if (hashTable[current_index].key == key){
                outValue = hashTable[current_index].value;
                return true;
            }
        }else if (hashTable[current_index].state == EMPTY){
            return false; // Clave no encontrada porque llegamos a una celda marcada como vacía
        }       

        current_index = (current_index + 1) % table_size; 
        probes++;

        if (probes >= table_size) {
            return false;
        }

    } while (current_index != initial_index); 

    return false;
}

// Elimina un par clave-valor de la tabla hash
bool deleteLinearProbing(vector<HashEntry>& hashTable, int key, int table_size){

    if (table_size == 0) return false;
    int initial_index = hashFunction(key, table_size);
    int current_index = initial_index;
    int probes = 0;

    do {
        if (hashTable[current_index].state == OCCUPIED){
            if (hashTable[current_index].key == key){
                hashTable[current_index].state = DELETED; // Si encontramos la key, se marca como eliminada
                return true;
            }
        } else if (hashTable[current_index].state == EMPTY){
            return false; 
        }

        current_index = (current_index + 1) % table_size; 
        probes++;

        if (probes >= table_size) {
            return false;
        }

    } while (current_index != initial_index);

    return false;
}


// --- Funciones para Generar Datos de Prueba (Mismas que en Separate Chaining) ---

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

// --- Funciones para Ejecutar Experimentos (Adaptadas para Linear Probing) ---

// Definiciones de tipos para las funciones de la tabla hash (Linear Probing)
using LinearProbingHashTable = vector<HashEntry>;
using LP_InitFunc = LinearProbingHashTable (*)(int);
using LP_InsertFunc = function<bool(LinearProbingHashTable&, int, int, int)>; // Ahora retorna bool
using LP_SearchFunc = function<bool(const LinearProbingHashTable&, int, int&, int)>;
using LP_DeleteFunc = function<bool(LinearProbingHashTable&, int, int)>;


// Función para ejecutar un escenario de prueba para una implementación dada
double runLinearProbingExperiment(
    LP_InitFunc initFunc,
    LP_InsertFunc insertOp,
    LP_SearchFunc searchOp,
    LP_DeleteFunc deleteOp,
    int table_size,
    const std::vector<KeyValue>& base_data_for_insert, // Datos para la fase de inserción inicial
    const std::vector<int>& keys_for_search,           // Claves para buscar
    const std::vector<int>& keys_for_delete,           // Claves para eliminar
    double insertion_ratio,
    double search_ratio,
    double delete_ratio
) {
    auto start_time = chrono::high_resolution_clock::now();

    LinearProbingHashTable hashTable = initFunc(table_size);

    // FASE 1: Inserciones iniciales para construir la tabla base
    // Es crucial verificar si las inserciones iniciales fallan en LP
    int successful_initial_inserts = 0;
    for (const auto& kv : base_data_for_insert) {
        if (insertOp(hashTable, kv.key, kv.value, table_size)) {
            successful_initial_inserts++;
        } else {
            // cout << "WARNING: Initial insert failed for key " << kv.key << " (Table full or high load factor)" << endl;
            // Si una inserción inicial falla, es posible que la tabla esté demasiado llena.
            // Esto podría sesgar los resultados si la tabla no se llena completamente.
            // Para pruebas justas, table_size debe ser > NUM_DATA_POINTS.
            break; // Detener si no se puede insertar más (tabla llena)
        }
    }
    
    // Si no se pudieron insertar todos los datos iniciales, este experimento
    // no es comparable y debe ser manejado adecuadamente (ej. lanzar excepción, advertir).
    // Para la evaluación, asumiremos que table_size > NUM_DATA_POINTS en los casos relevantes.
    if (successful_initial_inserts < base_data_for_insert.size()) {
         // cout << "Warning: Only " << successful_initial_inserts << " of " << base_data_for_insert.size() << " initial elements inserted." << endl;
         // Si la tabla se llena durante las inserciones iniciales,
         // el resto de operaciones mixtas no serán representativas.
         // Puedes decidir devolver 0 o un valor especial aquí.
         // Por ahora, permitimos que continúe para ver el tiempo.
    }


    // FASE 2: Operaciones mixtas según los ratios
    int total_mixed_ops = base_data_for_insert.size() * 2; // Realizar el doble de operaciones mixtas que datos iniciales
    
    vector<int> current_keys_to_search = keys_for_search;
    vector<int> current_keys_to_delete = keys_for_delete;

    std::mt19937 gen_mixed_ops(TEST_RANDOM_SEED); // Generador para la selección de operaciones
    std::uniform_real_distribution<> distrib_ratio(0.0, 1.0); // Para seleccionar la operación
    
    // Generador para nuevas claves
    std::mt19937 gen_new_key(TEST_RANDOM_SEED + 1); 
    std::uniform_int_distribution<> distrib_new_key(2000001, 3000000); 

    int current_search_idx = 0;
    int current_delete_idx = 0;

    for (int i = 0; i < total_mixed_ops; ++i) {
        double r = distrib_ratio(gen_mixed_ops); 
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
    
    const int NUM_DATA_POINTS = 10000; 
    const int MAX_KEY_VALUE = 20000; 
    const int NUM_REPETITIONS = 6;    
    
    // IMPORTANTE: Para Linear Probing, table_size DEBE ser mayor que NUM_DATA_POINTS
    // para evitar que las inserciones iniciales fallen debido a una alta carga.

    vector<int> table_sizes = {15000, 20000, 50000, 100000}; 

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

    cout << "\n=========== PRUEBAS DE LINEAR PROBING ===========" << endl;
    for (int current_table_size : table_sizes) {
        cout << "\n--- Probando Linear Probing con TABLE_SIZE (M) = " << current_table_size << " ---" << endl;

        for (const auto& scenario : scenarios) {
            string scenario_name = get<0>(scenario);
            double insert_ratio = get<1>(scenario);
            double search_ratio = get<2>(scenario);
            double delete_ratio = get<3>(scenario);

            double total_time_ns = 0.0;

            cout << "\n  Escenario: " << scenario_name
                      << " (I:" << (int)(insert_ratio*100) << "%, S:" << (int)(search_ratio*100) << "%, D:" << (int)(delete_ratio*100) << "%)" << endl;

            for (int i = 0; i < NUM_REPETITIONS; ++i) {
                // Aquí llamamos a la función de experimento específica de Linear Probing
                double current_run_time = runLinearProbingExperiment(
                    createLinearProbingTable,
                    insertLinearProbing,
                    searchLinearProbing,
                    deleteLinearProbing,
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