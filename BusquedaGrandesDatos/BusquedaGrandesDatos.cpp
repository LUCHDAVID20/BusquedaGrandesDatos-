#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <limits>

using namespace std;

static constexpr int32_t MINV = -50'000'000;
static constexpr int32_t MAXV = 50'000'000;

static constexpr size_t N = 10'000'000;    // 10 millones
static constexpr int AUTO_SEARCHES = 1000; // 1000 búsquedas

// ---- Utilidades de tiempo ----
using Clock = chrono::high_resolution_clock;

template <class Fn>
double measure_avg_ms(Fn fn, int repeats) {
    auto t0 = Clock::now();
    for (int i = 0; i < repeats; i++) fn();
    auto t1 = Clock::now();
    chrono::duration<double, milli> ms = t1 - t0;
    return ms.count() / repeats; // promedio
}

// ---- Memoria aproximada ----
static string bytes_to_human(uint64_t bytes) {
    const char* suf[] = { "B", "KB", "MB", "GB" };
    double v = (double)bytes;
    int i = 0;
    while (v >= 1024.0 && i < 3) { v /= 1024.0; i++; }
    ostringstream oss;
    oss << fixed << setprecision(2) << v << " " << suf[i];
    return oss.str();
}

// Aproximación muy razonable:
// vector<int32_t> ~ 4 bytes * N (más overhead mínimo)
// unordered_set: es difícil exacto; estimamos por buckets + nodos.
// Nota: Esto es aproximado (como pide la tarea).
static uint64_t approx_memory_vector_int32(size_t n) {
    return (uint64_t)n * sizeof(int32_t);
}

static uint64_t approx_memory_unordered_set(const unordered_set<int32_t>& s) {
    // Estimación: buckets (punteros) + nodos
    // buckets: bucket_count * sizeof(void*)
    // nodo: (valor 4 bytes) + puntero(s) + padding -> aproximamos 24 bytes por elemento en 64-bit (puede variar)
    uint64_t buckets = (uint64_t)s.bucket_count() * sizeof(void*);
    uint64_t nodes = (uint64_t)s.size() * 24ULL;
    return buckets + nodes;
}

// ---- Paso 1: Generar archivo ----
bool generate_file(const string& filename) {
    ofstream out(filename, ios::out | ios::trunc);
    if (!out) return false;

    // Generador aleatorio
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int32_t> dist(MINV, MAXV);

    // Buffer grande para acelerar escritura
    // (sin complicarnos con binary, la tarea pide texto)
    for (size_t i = 0; i < N; i++) {
        out << dist(gen) << '\n';
    }
    return true;
}

// ---- Paso 2: Cargar archivo a vector ----
bool load_file(const string& filename, vector<int32_t>& data) {
    ifstream in(filename);
    if (!in) return false;

    data.clear();
    data.reserve(N);

    int64_t x;
    while (in >> x) {
        if (x < MINV || x > MAXV) continue; // seguridad
        data.push_back((int32_t)x);
    }
    return !data.empty();
}

// ---- Búsquedas ----
static inline bool search_sorted_vector(const vector<int32_t>& sorted, int32_t target) {
    return binary_search(sorted.begin(), sorted.end(), target);
}

static inline bool search_hash(const unordered_set<int32_t>& h, int32_t target) {
    return h.find(target) != h.end();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string filename = "numeros_10m.txt";

    cout << "=== Tarea: Busqueda eficiente sobre grandes volumenes de datos ===\n";
    cout << "Archivo: " << filename << "\n\n";

    // Menú básico
    cout << "1) Generar archivo con 10,000,000 numeros\n";
    cout << "2) Cargar archivo, construir estructuras y medir rendimiento\n";
    cout << "Elige una opcion (1/2): ";

    int opt = 0;
    cin >> opt;

    if (opt == 1) {
        cout << "\nGenerando archivo... (puede tardar un rato)\n";
        auto t0 = Clock::now();
        if (!generate_file(filename)) {
            cerr << "Error: no se pudo crear el archivo.\n";
            return 1;
        }
        auto t1 = Clock::now();
        chrono::duration<double> sec = t1 - t0;
        cout << "Listo. Tiempo: " << fixed << setprecision(2) << sec.count() << " s\n";
        cout << "Ahora ejecuta de nuevo y elige opcion 2.\n";
        return 0;
    }

    // Opción 2
    vector<int32_t> data;
    cout << "\nCargando archivo...\n";
    auto tload0 = Clock::now();
    if (!load_file(filename, data)) {
        cerr << "Error: no se pudo leer el archivo o esta vacio.\n";
        cerr << "Tip: primero ejecuta la opcion 1 para generar el archivo.\n";
        return 1;
    }
    auto tload1 = Clock::now();
    chrono::duration<double> loadsec = tload1 - tload0;
    cout << "Numeros cargados: " << data.size() << "\n";
    cout << "Tiempo de carga: " << fixed << setprecision(2) << loadsec.count() << " s\n\n";

    // Construir estructura 1: vector ordenado
    vector<int32_t> sorted = data;
    cout << "Ordenando vector (estructura: arreglo ordenado)...\n";
    auto tsort0 = Clock::now();
    sort(sorted.begin(), sorted.end());
    auto tsort1 = Clock::now();
    chrono::duration<double> sortsec = tsort1 - tsort0;
    cout << "Tiempo de ordenamiento: " << fixed << setprecision(2) << sortsec.count() << " s\n\n";

    // Construir estructura 2: hash
    cout << "Construyendo tabla hash (unordered_set)...\n";
    auto th0 = Clock::now();
    unordered_set<int32_t> h;
    // Reservar para evitar rehash costoso
    h.reserve(data.size() * 1.3);
    for (auto v : data) h.insert(v);
    auto th1 = Clock::now();
    chrono::duration<double> hsec = th1 - th0;
    cout << "Tiempo de construccion hash: " << fixed << setprecision(2) << hsec.count() << " s\n\n";

    // Preparar búsquedas automáticas
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int32_t> dist(MINV, MAXV);

    vector<int32_t> queries;
    queries.reserve(AUTO_SEARCHES);
    for (int i = 0; i < AUTO_SEARCHES; i++) queries.push_back(dist(gen));

    // Medir tiempos promedio
    size_t idx = 0;

    double avg_sorted_ms = measure_avg_ms([&]() {
        search_sorted_vector(sorted, queries[idx]);
        idx = (idx + 1) % queries.size();
        }, AUTO_SEARCHES);

    idx = 0;
    double avg_hash_ms = measure_avg_ms([&]() {
        search_hash(h, queries[idx]);
        idx = (idx + 1) % queries.size();
        }, AUTO_SEARCHES);

    // Memoria aproximada
    uint64_t mem_sorted = approx_memory_vector_int32(sorted.size());
    uint64_t mem_hash = approx_memory_unordered_set(h);

    // Tabla comparativa
    cout << "================= TABLA COMPARATIVA =================\n";
    cout << left
        << setw(25) << "Estructura de Datos"
        << setw(28) << "Tiempo prom. busqueda"
        << setw(26) << "Memoria aprox. usada"
        << "Complejidad teorica\n";
    cout << string(95, '-') << "\n";

    cout << left
        << setw(25) << "Arreglo ordenado"
        << setw(28) << (to_string(avg_sorted_ms) + " ms")
        << setw(26) << bytes_to_human(mem_sorted)
        << "O(log n)\n";

    cout << left
        << setw(25) << "Tabla Hash"
        << setw(28) << (to_string(avg_hash_ms) + " ms")
        << setw(26) << bytes_to_human(mem_hash)
        << "O(1) promedio\n";

    cout << string(95, '-') << "\n\n";

    // Búsqueda manual
    cout << "Busqueda manual (escribe 'salir' para terminar)\n";
    cout << "Ingresa un numero: ";
    string s;
    while (cin >> s) {
        if (s == "salir" || s == "SALIR") break;

        try {
            long long val = stoll(s);
            if (val < MINV || val > MAXV) {
                cout << "Fuera de rango [" << MINV << ", " << MAXV << "].\n";
            }
            else {
                int32_t x = (int32_t)val;
                bool a = search_sorted_vector(sorted, x);
                bool b = search_hash(h, x);
                cout << "Arreglo ordenado: " << (a ? "ENCONTRADO" : "NO") << " | "
                    << "Hash: " << (b ? "ENCONTRADO" : "NO") << "\n";
            }
        }
        catch (...) {
            cout << "Entrada invalida.\n";
        }
        cout << "Ingresa un numero: ";
    }

    cout << "\nFin.\n";
    return 0;
}