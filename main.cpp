/*

Integrantes:
*Ariana Fernández
*Karla Ramos

*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <cctype> // Para isalpha, isdigit
#include <chrono> // Para medición de tiempos
#include <ctime>  // Para formatear fechas
#include <mutex>
#include <atomic>
#include <thread> 

using namespace std;
using namespace std::chrono;

//=====================================================
// FUNCIÓN SHA-256 (compatible con DevC++)
//=====================================================
typedef unsigned char uint8;
typedef unsigned int uint32;

const uint32 k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
    0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
    0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
    0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66b,0xb00327c8,0xbf597fc7,
    0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
    0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
    0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
    0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
    0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

uint32 rotr(uint32 x, uint32 n) {
    return (x >> n) | (x << (32 - n));
}

string sha256(const string& input) {
    uint32 h[8] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };

    vector<uint8> data(input.begin(), input.end());

    uint64_t bitlen = data.size() * 8;
    data.push_back(0x80);
    while ((data.size() * 8) % 512 != 448) {
        data.push_back(0x00);
    }
    for (int i = 7; i >= 0; i--) {
        data.push_back((bitlen >> (i * 8)) & 0xff);
    }

    for (size_t i = 0; i < data.size(); i += 64) {
        uint32 w[64] = {};
        for (int j = 0; j < 16; j++) {
            w[j] = (data[i + j*4] << 24) | (data[i + j*4 + 1] << 16) |
                   (data[i + j*4 + 2] << 8) | (data[i + j*4 + 3]);
        }

        for (int j = 16; j < 64; j++) {
            uint32 s0 = rotr(w[j-15], 7) ^ rotr(w[j-15], 18) ^ (w[j-15] >> 3);
            uint32 s1 = rotr(w[j-2], 17) ^ rotr(w[j-2], 19) ^ (w[j-2] >> 10);
            w[j] = w[j-16] + s0 + w[j-7] + s1;
        }

        uint32 a = h[0], b = h[1], c = h[2], d = h[3];
        uint32 e = h[4], f = h[5], g = h[6], hh = h[7];

        for (int j = 0; j < 64; j++) {
            uint32 S1 = rotr(e,6)^rotr(e,11)^rotr(e,25);
            uint32 ch = (e & f) ^ ((~e) & g);
            uint32 temp1 = hh + S1 + ch + k[j] + w[j];
            uint32 S0 = rotr(a,2)^rotr(a,13)^rotr(a,22);
            uint32 maj = (a & b) ^ (a & c) ^ (b & c);
            uint32 temp2 = S0 + maj;

            hh = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }

        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }

    stringstream ss;
    for (int i = 0; i < 8; i++) {
        ss << hex << setw(8) << setfill('0') << h[i];
    }

    return ss.str();
}

//=====================================================
// CIFRAR UN SOLO CARÁCTER
//=====================================================

char cifrarCaracter(char c) {
    if (isalpha(c)) {
        char base = isupper(c) ? 'A' : 'a';
        return ((c - base + 3) % 26) + base;
    }
    else if (isdigit(c)) {
        return '9' - (c - '0');
    }
    return c;
}

//=====================================================
// CREAR N COPIAS DESDE original.txt
//=====================================================

void crearNcopias(int N, double& tiempoTotalCreacion) {
    auto inicioTotal = high_resolution_clock::now();
    
    ifstream archivoOriginal("original.txt");
    if (!archivoOriginal.is_open()) {
        cout << "Error al abrir original.txt" << endl;
        return;
    }

    string texto = "", linea;
    while (getline(archivoOriginal, linea)) {
        texto += linea + "\n";
    }
    archivoOriginal.close();

    for (int i = 1; i <= N; ++i) {
        auto inicio = high_resolution_clock::now();
        
        string nombreArchivo = to_string(i) + ".txt";
        ofstream copia(nombreArchivo);
        if (!copia.is_open()) {
            cout << "Error creando " << nombreArchivo << endl;
            continue;
        }
        copia << texto;
        copia.close();
        
        auto fin = high_resolution_clock::now();
        duration<double> duracion = fin - inicio;
        cout << "Se creó: " << nombreArchivo << " en " << duracion.count() << " segundos" << endl;
    }
    
    auto finTotal = high_resolution_clock::now();
    duration<double> duracionTotal = finTotal - inicioTotal;
    tiempoTotalCreacion = duracionTotal.count();
}

// ---------------------------------------------
// CIFRAR ARCHIVO Y SOBREESCRIBIRLO

void cifrarArchivo(const string& nombreArchivo, double& tiempoCifrado) {
    auto inicio = high_resolution_clock::now();
    
    ifstream entrada(nombreArchivo);
    if (!entrada.is_open()) {
        cout << "Error al abrir " << nombreArchivo << endl;
        return;
    }

    string contenido((istreambuf_iterator<char>(entrada)), istreambuf_iterator<char>());
    entrada.close();

    string contenidoCifrado = "";
    for (char c : contenido) {
        contenidoCifrado += cifrarCaracter(c);
    }

    ofstream salida(nombreArchivo);
    if (!salida.is_open()) {
        cout << "Error al escribir en " << nombreArchivo << endl;
        return;
    }

    salida << contenidoCifrado;
    salida.close();
    
    auto fin = high_resolution_clock::now();
    duration<double> duracion = fin - inicio;
    tiempoCifrado = duracion.count();
    
    cout << "Cifrado: " << nombreArchivo << " en " << tiempoCifrado << " segundos" << endl;
}

// ---------------------------------------------
// GENERAR HASH Y GUARDAR EN .sha

void generarHashDeArchivo(const string& nombreArchivo, double& tiempoHash) {
    auto inicio = high_resolution_clock::now();
    
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error leyendo " << nombreArchivo << " para hash" << endl;
        return;
    }

    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    archivo.close();

    string hash = sha256(contenido);

    string nombreSha = nombreArchivo.substr(0, nombreArchivo.find(".")) + ".sha";
    ofstream salida(nombreSha);
    if (!salida.is_open()) {
        cout << "Error escribiendo " << nombreSha << endl;
        return;
    }

    salida << hash;
    salida.close();
    
    auto fin = high_resolution_clock::now();
    duration<double> duracion = fin - inicio;
    tiempoHash = duracion.count();
    
    cout << "Hash de " << nombreArchivo << " guardado en " << nombreSha << " en " << tiempoHash << " segundos" << endl;
}

void validarArchivoConHash(const string& nombreArchivo, double& tiempoValidacion) {
    auto inicio = high_resolution_clock::now();
    
    // Paso 1: Leer archivo cifrado y calcular hash actual
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error al abrir " << nombreArchivo << " para validación" << endl;
        return;
    }

    string contenido((istreambuf_iterator<char>(archivo)), istreambuf_iterator<char>());
    archivo.close();

    string hashActual = sha256(contenido);

    // Paso 2: Leer archivo .sha
    string nombreSha = nombreArchivo.substr(0, nombreArchivo.find(".")) + ".sha";
    ifstream archivoSha(nombreSha);
    if (!archivoSha.is_open()) {
        cout << "Error al abrir " << nombreSha << endl;
        return;
    }

    string hashGuardado;
    getline(archivoSha, hashGuardado);
    archivoSha.close();

    // Paso 3: Comparar ambos hashes
    if (hashActual == hashGuardado) {
        cout << "? VALIDACIÓN EXITOSA: " << nombreArchivo << " no ha sido modificado." << endl;
    } else {
        cout << "? ERROR: El contenido de " << nombreArchivo << " ha sido alterado." << endl;
    }
    
    auto fin = high_resolution_clock::now();
    duration<double> duracion = fin - inicio;
    tiempoValidacion = duracion.count();
    
    cout << "Validación de " << nombreArchivo << " completada en " << tiempoValidacion << " segundos" << endl;
}

char descifrarCaracter(char c) {
    if (isalpha(c)) {
        char base = isupper(c) ? 'A' : 'a';
        // Mismo cálculo pero hacia la izquierda (usar +26 para evitar negativos)
        return ((c - base - 3 + 26) % 26) + base;
    }
    else if (isdigit(c)) {
        return '9' - (c - '0');  // es igual a la encriptación
    }
    return c; // otros caracteres quedan igual
}

void descifrarArchivo(const string& nombreArchivo, double& tiempoDescifrado) {
    auto inicio = high_resolution_clock::now();
    
    ifstream entrada(nombreArchivo);
    if (!entrada.is_open()) {
        cout << "Error al abrir " << nombreArchivo << " para desencriptar" << endl;
        return;
    }

    string contenido((istreambuf_iterator<char>(entrada)), istreambuf_iterator<char>());
    entrada.close();

    string contenidoDescifrado = "";
    for (char c : contenido) {
        contenidoDescifrado += descifrarCaracter(c);
    }

    // Guardar en archivo nuevo: i_dec.txt
    string nombreSalida = nombreArchivo.substr(0, nombreArchivo.find(".")) + "_dec.txt";
    ofstream salida(nombreSalida);
    if (!salida.is_open()) {
        cout << "Error al crear " << nombreSalida << endl;
        return;
    }

    salida << contenidoDescifrado;
    salida.close();
    
    auto fin = high_resolution_clock::now();
    duration<double> duracion = fin - inicio;
    tiempoDescifrado = duracion.count();
    
    cout << "Archivo desencriptado: " << nombreSalida << " en " << tiempoDescifrado << " segundos" << endl;
}

void compararConOriginal(const string& nombreDesencriptado, double& tiempoComparacion) {
    auto inicio = high_resolution_clock::now();
    
    ifstream archivoOriginal("original.txt");
    ifstream archivoDesencriptado(nombreDesencriptado);

    if (!archivoOriginal.is_open() || !archivoDesencriptado.is_open()) {
        cout << "Error abriendo archivos para comparar: " << nombreDesencriptado << endl;
        return;
    }

    string lineaOriginal, lineaDesencriptada;
    int linea = 1;
    bool iguales = true;

    while (getline(archivoOriginal, lineaOriginal) && getline(archivoDesencriptado, lineaDesencriptada)) {
        if (lineaOriginal != lineaDesencriptada) {
            cout << "? Diferencia en línea " << linea << ": " << endl;
            cout << "Original:     " << lineaOriginal << endl;
            cout << "Desencriptado:" << lineaDesencriptada << endl;
            iguales = false;
        }
        linea++;
    }

    // Verifica si uno tiene más líneas que el otro
    if ((getline(archivoOriginal, lineaOriginal)) || (getline(archivoDesencriptado, lineaDesencriptada))) {
        cout << "? Los archivos tienen diferente número de líneas." << endl;
        iguales = false;
    }

    if (iguales) {
        cout << "? " << nombreDesencriptado << " es igual a original.txt" << endl;
    }

    archivoOriginal.close();
    archivoDesencriptado.close();
    
    auto fin = high_resolution_clock::now();
    duration<double> duracion = fin - inicio;
    tiempoComparacion = duracion.count();
    
    cout << "Comparación de " << nombreDesencriptado << " completada en " << tiempoComparacion << " segundos" << endl;
}



// Función para obtener la fecha y hora actual formateada
string obtenerFechaHora() {
    auto ahora = system_clock::now();
    time_t tiempo = system_clock::to_time_t(ahora);
    string str = ctime(&tiempo);
    // Eliminar el salto de línea que añade ctime
    if (!str.empty() && str[str.length()-1] == '\n') {
        str.erase(str.length()-1);
    }
    return str;
}




//OPTIMIZADO-------------------------------------


void crearNcopiasop(int N, double& tiempoTotalCreacion) {
    auto inicioTotal = high_resolution_clock::now();
    
    ifstream archivoOriginal("original.txt");
    if (!archivoOriginal.is_open()) {
        cout << "Error al abrir original.txt" << endl;
        return;
    }

    string texto = "", linea;
    while (getline(archivoOriginal, linea)) {
        texto += linea + "\n";
    }
    archivoOriginal.close();

    for (int i = 1; i <= N; ++i) {
        auto inicio = high_resolution_clock::now();
        
        string nombreArchivo = to_string(i) + "op.txt";
        ofstream copia(nombreArchivo);
        if (!copia.is_open()) {
            cout << "Error creando " << nombreArchivo << endl;
            continue;
        }
        copia << texto;
        copia.close();
        
        auto fin = high_resolution_clock::now();
        duration<double> duracion = fin - inicio;
        cout << "Se creó: " << nombreArchivo << " en " << duracion.count() << " segundos" << endl;
    }
    
    auto finTotal = high_resolution_clock::now();
    duration<double> duracionTotal = finTotal - inicioTotal;
    tiempoTotalCreacion = duracionTotal.count();
}




mutex mtx_cout; // Para sincronizar salida a consola
atomic<int> archivos_procesados(0);

void procesarArchivoop(int i) {
    string nombreOriginal = to_string(i) + ".txt";
    string nombreCifrado = to_string(i) + "op_enc.txt";
    string nombreDescifrado = to_string(i) + "op_dec.txt";
    string nombreHash = to_string(i) + "op.sha";

    auto inicio = high_resolution_clock::now();
    
    // 1. Leer archivo original
    ifstream in(nombreOriginal, ios::binary);
    if (!in) {
        lock_guard<mutex> lock(mtx_cout);
        cerr << "Error al abrir " << nombreOriginal << endl;
        return;
    }
    string contenido((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();

	// 2. Cifrar contenido
    string cifrado;
    for (char c : contenido) {
        cifrado += cifrarCaracter(c);
    }

    // 3. Guardar archivo cifrado
    ofstream outEnc(nombreCifrado, ios::binary);
    if (!outEnc) {
        lock_guard<mutex> lock(mtx_cout);
        cerr << "Error al crear " << nombreCifrado << endl;
        return;
    }
    
    outEnc << cifrado;
    outEnc.close();

    // 4. Generar y guardar hash
    string hash = sha256(cifrado);
    ofstream outHash(nombreHash, ios::binary);
    if (!outHash) {
        lock_guard<mutex> lock(mtx_cout);
        cerr << "Error al crear " << nombreHash << endl;
        return;
    }
    outHash << hash;
    outHash.close();

    // 5. Descifrar archivo
    string descifrado;
    for (char c : cifrado) {
        descifrado += descifrarCaracter(c);
    }
	 // 6. Guardar archivo descifrado
    ofstream outDec(nombreDescifrado, ios::binary);
    if (!outDec) {
        lock_guard<mutex> lock(mtx_cout);
        cerr << "Error al crear " << nombreDescifrado << endl;
        return;
    }
    outDec << descifrado;
    outDec.close();

    // 7. Validar hash
    ifstream inHash(nombreHash);
    string hashGuardado;
    getline(inHash, hashGuardado);
    inHash.close();

    bool hashValido = (sha256(cifrado) == hashGuardado);
    bool contenidoValido = (descifrado == contenido);

    auto fin = high_resolution_clock::now();
    double tiempo = duration_cast<duration<double>>(fin - inicio).count();

    // Mostrar resultados
    {
    
 lock_guard<mutex> lock(mtx_cout);
        cout << "Procesado " << nombreOriginal << " en " << fixed << setprecision(3) << tiempo << "s\n";
        cout << "  Hash válido: " << (hashValido ? "SI" : "NO") << endl;
        cout << "  Descifrado correcto: " << (contenidoValido ? "SI" : "NO") << endl;
    }

    archivos_procesados++;
}





// ---------------------------------------------
// FUNCIÓN PRINCIPAL

int main() {
	
	//PROCESO BASEEEEEEEEEEEE
    // Mostrar hora de inicio
    string inicioProceso = obtenerFechaHora();
    cout << "PROCESO BASE: " << inicioProceso;
    
    int N;
    cout << "¿Cuántas copias deseas crear? (máx 50): ";
    cin >> N;

    if (N < 1 || N > 50) {
        cout << "Número inválido." << endl;
        return 1;
    }

    // Variables para almacenar tiempos
    double tiempoTotalCreacion = 0;
    double tiempoTotalCifrado = 0;
    double tiempoTotalHash = 0;
    double tiempoTotalValidacion = 0;
    double tiempoTotalDescifrado = 0;
    double tiempoTotalComparacion = 0;
    
    // Crear copias
    cout << "\n= Creando " << N << " copias =" << endl;
    crearNcopias(N, tiempoTotalCreacion);
    
    // Cifrar archivos
    
    for (int i = 1; i <= N; ++i) {
        string nombre = to_string(i) + ".txt";
        double tiempoCifrado = 0;
        cifrarArchivo(nombre, tiempoCifrado);
        tiempoTotalCifrado += tiempoCifrado;
    }
    
    // Generar hashes

    for (int i = 1; i <= N; ++i) {
        string nombre = to_string(i) + ".txt";
        double tiempoHash = 0;
        generarHashDeArchivo(nombre, tiempoHash);
        tiempoTotalHash += tiempoHash;
    }

    cout << "\nTodo listo: copias, cifrado y hash generados." << endl;
    system("pause");
    
    // Validar archivos
    
    for (int i = 1; i <= N; ++i) {
        string nombre = to_string(i) + ".txt";
        double tiempoValidacion = 0;
        validarArchivoConHash(nombre, tiempoValidacion);
        tiempoTotalValidacion += tiempoValidacion;
    }
    
    // Desencriptar archivos
    
    for (int i = 1; i <= N; ++i) {
        string nombre = to_string(i) + ".txt";
        double tiempoDescifrado = 0;
        descifrarArchivo(nombre, tiempoDescifrado);
        tiempoTotalDescifrado += tiempoDescifrado;
    }
    
    // Comparar archivos
    
    for (int i = 1; i <= N; ++i) {
        string nombreDesencriptado = to_string(i) + "_dec.txt";
        double tiempoComparacion = 0;
        compararConOriginal(nombreDesencriptado, tiempoComparacion);
        tiempoTotalComparacion += tiempoComparacion;
    }
    
    // Calcular promedios
    double promedioCifrado = tiempoTotalCifrado / N;
    double promedioHash = tiempoTotalHash / N;
    double promedioValidacion = tiempoTotalValidacion / N;
    double promedioDescifrado = tiempoTotalDescifrado / N;
    double promedioComparacion = tiempoTotalComparacion / N;
    
    // Calcular tiempo total
    double tiempoTotal = tiempoTotalCreacion + tiempoTotalCifrado + tiempoTotalHash + 
                        tiempoTotalValidacion + tiempoTotalDescifrado + tiempoTotalComparacion;
    
    // Mostrar resumen de tiempo
    cout << "TIEMPO TOTAL DEL PROCESO: " << tiempoTotal << " segundos" << endl;
    
    // Mostrar hora de finalización
    string finProceso = obtenerFechaHora();
    cout << "\nFin del proceso: " << finProceso<<""<<endl;
    cout << "===============================================\n";
    cout << "===============================================\n";
    
    
    
    
    
    //PROCESO OPTIMIZADOOOO
    
    system("pause");
    N==0;
    cout << "PROCESO OPTIMIZADO\n";
    cout << "===============================================\n";
    cout << "¿Cuántas copias deseas crear? (máx 50):";
    cin >> N;
    
    if (N < 1) {
        cerr << "Error: N debe ser mayor que 0" << endl;
        return 1;
    }
    
    
    auto inicioTotal = high_resolution_clock::now();
    
    // Crear copias
    crearNcopiasop(N, tiempoTotalCreacion);
	
    
    // Determinar número de hilos
    unsigned int numHilos = thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 10; // Valor por defecto si no se puede determinar
    cout << "Usando " << numHilos << " hilos para procesamiento paralelo\n";

    // Procesar archivos en paralelo por lotes
    vector< thread > hilos;
    for (int i = 1; i <= N; ) {
        // Lanzar un lote de hilos
        for (unsigned int t = 0; t < numHilos && i <= N; t++, i++) {
            hilos.emplace_back(procesarArchivoop, i);
        }

        // Esperar a que terminen los hilos del lote actual
        for (auto& hilo : hilos) {
            hilo.join();
        }
        hilos.clear();
    }

    auto finTotal = high_resolution_clock::now();
    double tiempoTotalopti = duration_cast<duration<double>>(finTotal - inicioTotal).count();
	
	 cout << "\nRESUMEN FINAL:\n";
    cout << "Archivos procesados: " << archivos_procesados << endl;
    cout << fixed << setprecision(3);
    cout << "Tiempo total: " << tiempoTotalopti << " segundos\n";
    
    system("pause");
    
    
    
    return 0;
}
