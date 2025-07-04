#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
void crearNcopias(int N) {
    string texto; //Guardar el texto completo de original.txt
    ifstream archivoOriginal("original.txt"); //Abrir archivo

    if (!archivoOriginal.is_open()) {  //si no existe sale error
        cout << "Error al abrir archivo " << endl;
    }
    string linea;
    //getline lee una lina a la vez
    while (getline(archivoOriginal, linea)) {
        texto += linea + "\n"; //cada linea se va acumulando en texto agregando un salto de linea
    }
    archivoOriginal.close();

    if (N < 1 || N > 50) {
        cout<<"Numero invalido de copias"<<endl;
    }

    for (int i = 1; i <= N; ++i) {
        string nombreArchivo = to_string(i) + ".txt";
        ofstream copia(nombreArchivo);  //escribir archivo
        if (!copia.is_open()) {
            cout << "Error creando " <<nombreArchivo << endl;
            continue;
        }
        copia << texto;
        copia.close();
        cout<< "se creo: " << nombreArchivo << endl;
    }
}

string encriptarTexto(string texto) {
    string resultado;

    for (int i = 0; i < texto.length(); ++i) {
        char caracter = texto[i];

        //Para caracteres mayuscula
        if ( caracter >= 'A' && caracter <= 'Z') {

        }
    }
}
int main() {
    int Ncopias; // N copias del usuario
    cout << "Ingrese el numero de copias que desea: " << endl;
    cin >> Ncopias;

    crearNcopias(Ncopias);

    return 0;
}
