#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <set>
#include <list>
#include <iomanip>
#include <cstdint>
#include <sstream>

using namespace std;

static constexpr int32_t MINV = -50000000;
static constexpr int32_t MAXV = 50000000;

static constexpr size_t N = 10000000;     // 10 millones
static constexpr int AUTO_SEARCHES = 1000; // 1000 búsquedas

using Clock = chrono::high_resolution_clock;

template <class Fn>
double measure_avg_us(Fn fn, int repeats) {
    auto t0 = Clock::now();
    for (int i = 0; i < repeats; i++) fn();
    auto t1 = Clock::now();
    chrono::duration<double, micro> us = t1 - t0; // microsegundos
    return us.count() / repeats;
}

static string bytes_to_human(uint64_t bytes) {
    const char* suf[] = { "B", "KB", "MB", "GB" };
    double v = (double)bytes;
    int i = 0;
    while (v >= 1024.0 && i < 3) { v /= 1024.0; i++; }
    ostringstream oss;
    oss << fixed << setprecision(2) << v << " " << suf[i];
    return oss.str();
}

static uint64_t approx_memory_vector_int32(size_t n) {
    return (uint64_t)n * sizeof(int32_t);
}

static uint64_t approx_memory_list_int32(size_t n) {
    // Estimación: nodo list en 64-bit ~ valor(4) + 2 punteros(16) + padding -> ~24 bytes
    return (uint64_t)n * 24ULL;
}

static uint64_t approx_memory_set_int32(size_t n) {
    // Estimación: nodo de árbol rojo-negro ~ 32-48 bytes; usamos 48 como aproximación conservadora
    return (uint64_t)n * 48ULL;
}

static uint64_t approx_memory_unordered_set(const unordered_set<int32_t>& s) {
    uint64_t buckets = (uint64_t)s.bucket_count() * sizeof(void*);
    uint64_t nodes = (uint64_t)s.size() * 24ULL; // aproximación
    return buckets + nodes;
}

bool generate_file(const string& filename) {
    ofstream out(filename, ios::out | ios::trunc);
    if (!out) return false;

    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int32_t> dist(MINV, MAXV);

    for (size_t i = 0; i < N; i++) out << dist(gen) << '\n';
    return true;
}

bool load_file(const string& filename, vector<int32_t>& data) {
    ifstream in(filename);
    if (!in) return false;

    data.clear();
    data.reserve(N);

    long long x;
    while (in >> x) {
        if (x < MINV || x > MAXV) continue;
        data.push_back((int32_t)x);
    }
    return !data.empty();
}

static inline bool search_linked_list(const list<int32_t>& lst, int32_t target) {
    // Búsqueda lineal
    for (int32_t v : lst) if (v == target) return true;
    return false;
}

static inline bool search_bst_set(const set<int32_t>& st, int32_t target) {
    return st.find(target) != st.end();
}

static inline bool search_hash(const unordered_set<int32_t>& h, int32_t target) {
    return h.find(target) != h.end();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string filename = "numeros_10m.txt";

    cout << "=== Comparacion de busqueda: Lista enlazada vs Arbol (set) vs Hash ===\n";
    cout << "Archivo: " << filename << "\n\n";

    cout << "1) Generar archivo con 10,000,000 numeros\n";
    cout << "2) Cargar archivo, construir estructuras y medir rendimiento\n";
    cout << "Elige una opcion (1/2): ";

    int opt = 0;
    cin >> opt;

    if (opt == 1) {
        cout << "\nGenerando archivo... (puede tardar)\n";
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

    // ---- Cargar datos ----
    vector<int32_t> data;
    cout << "\nCargando archivo...\n";
    auto tload0 = Clock::now();
    if (!load_file(filename, data)) {
        cerr << "Error: no se pudo leer el archivo o esta vacio.\n";
        cerr << "Tip: primero ejecuta la opcion 1.\n";
        return 1;
    }
    auto tload1 = Clock::now();
    chrono::duration<double> loadsec = tload1 - tload0;
    cout << "Numeros cargados: " << data.size() << "\n";
    cout << "Tiempo de carga: " << fixed << setprecision(2) << loadsec.count() << " s\n\n";

    // ---- Construir estructuras ----
    cout << "Construyendo LISTA ENLAZADA (std::list)...\n";
    auto tl0 = Clock::now();
    list<int32_t> lst(data.begin(), data.end());
    auto tl1 = Clock::now();
    cout << "Tiempo construccion lista: " << fixed << setprecision(2)
        << chrono::duration<double>(tl1 - tl0).count() << " s\n\n";

    cout << "Construyendo ARBOL BINARIO balanceado (std::set)...\n";
    auto ts0 = Clock::now();
    set<int32_t> bst;
    bst.insert(data.begin(), data.end());
    auto ts1 = Clock::now();
    cout << "Tiempo construccion arbol(set): " << fixed << setprecision(2)
        << chrono::duration<double>(ts1 - ts0).count() << " s\n\n";

    cout << "Construyendo TABLA HASH (unordered_set)...\n";
    auto th0 = Clock::now();
    unordered_set<int32_t> h;
    h.reserve((size_t)(data.size() * 1.3));
    for (auto v : data) h.insert(v);
    auto th1 = Clock::now();
    cout << "Tiempo construccion hash: " << fixed << setprecision(2)
        << chrono::duration<double>(th1 - th0).count() << " s\n\n";

    // ---- Consultas automáticas ----
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<int32_t> dist(MINV, MAXV);

    vector<int32_t> queries;
    queries.reserve(AUTO_SEARCHES);
    for (int i = 0; i < AUTO_SEARCHES; i++) queries.push_back(dist(gen));

    size_t idx = 0;
    double avg_list_us = measure_avg_us([&]() {
        search_linked_list(lst, queries[idx]);
        idx = (idx + 1) % queries.size();
        }, AUTO_SEARCHES);

    idx = 0;
    double avg_bst_us = measure_avg_us([&]() {
        search_bst_set(bst, queries[idx]);
        idx = (idx + 1) % queries.size();
        }, AUTO_SEARCHES);

    idx = 0;
    double avg_hash_us = measure_avg_us([&]() {
        search_hash(h, queries[idx]);
        idx = (idx + 1) % queries.size();
        }, AUTO_SEARCHES);

    // ---- Memoria aproximada ----
    uint64_t mem_list = approx_memory_list_int32(lst.size());
    uint64_t mem_bst = approx_memory_set_int32(bst.size());
    uint64_t mem_hash = approx_memory_unordered_set(h);

    // ---- Tabla comparativa ----
    cout << "================= TABLA COMPARATIVA =================\n";
    cout << left
        << setw(25) << "Estructura de Datos"
        << setw(26) << "Tiempo prom. busqueda"
        << setw(24) << "Memoria aprox."
        << "Complejidad teorica\n";
    cout << string(90, '-') << "\n";

    cout << left
        << setw(25) << "Lista enlazada"
        << setw(26) << (to_string(avg_list_us) + " us")
        << setw(24) << bytes_to_human(mem_list)
        << "O(n)\n";

    cout << left
        << setw(25) << "Arbol binario (set)"
        << setw(26) << (to_string(avg_bst_us) + " us")
        << setw(24) << bytes_to_human(mem_bst)
        << "O(log n)\n";

    cout << left
        << setw(25) << "Tabla hash"
        << setw(26) << (to_string(avg_hash_us) + " us")
        << setw(24) << bytes_to_human(mem_hash)
        << "O(1) promedio\n";

    cout << string(90, '-') << "\n\n";

    // ---- Búsqueda manual ----
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
                bool a = search_linked_list(lst, x);
                bool b = search_bst_set(bst, x);
                bool c = search_hash(h, x);

                cout << "Lista: " << (a ? "ENCONTRADO" : "NO") << " | "
                    << "Arbol(set): " << (b ? "ENCONTRADO" : "NO") << " | "
                    << "Hash: " << (c ? "ENCONTRADO" : "NO") << "\n";
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