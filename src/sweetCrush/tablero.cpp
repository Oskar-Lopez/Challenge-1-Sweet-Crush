#include <iostream>
#include "tablero.h"
#include "bits.h"
using namespace std;

// ============================================================
// 3. VISUALIZACIÓN
// ============================================================
void mostrarTableroFichas(const unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    cout << "\n=== TABLERO (FICHAS) ===\n";
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            int posicion = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, posicion, bytes_totales);

            if (valor == 0)      cout << ".  ";
            else if (valor == 7) cout << "X  ";
            else                 cout << char('A' + valor - 1) << "  ";
        }
        cout << "\n";
    }
    cout << "========================\n";
}

void mostrarMemoriaBinaria(const unsigned char* buffer, int bytes_totales) {
    cout << "\n=== ESTADO EN MEMORIA (BINARIO) ===\n";
    cout << "Total Bytes reservados: " << bytes_totales << "\n";

    for (int i = 0; i < bytes_totales; i++) {
        cout << "Byte [" << i << "]: ";
        for (int bit = 7; bit >= 0; bit--) {
            cout << ((buffer[i] >> bit) & 1);
        }
        cout << "\n";
    }
    cout << "===================================\n";
}

// ============================================================
// 4. CREACIÓN DEL TABLERO
// ============================================================
void crearTablero(unsigned char*& buffer, int filas, int columnas,
                  int& total_posiciones, int& bytes_totales, unsigned int& semilla) {
    total_posiciones = filas * columnas;
    bytes_totales    = calcular_bytes_necesarios(filas, columnas);

    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }

    buffer = new unsigned char[bytes_totales]();

    for (int i = 0; i < total_posiciones; i++) {
        asignarFicha(buffer, i, generarFichaUniforme(semilla), bytes_totales);
    }
}