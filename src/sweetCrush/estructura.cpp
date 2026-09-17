#include <iostream>
#include "estructura.h"
#include "bits.h"
using namespace std;

// ============================================================
// 6. REGLA DEL 65%
// ============================================================
void aplicarRegla65(unsigned char*& buffer, int filas, int columnas,
                    int total_posiciones, int& bytes_totales, int bytes_anteriores) {
    int bytes_necesarios  = calcular_bytes_necesarios(filas, columnas);
    double porcentaje_uso = (double)bytes_necesarios / bytes_anteriores;

    cout << "\n[REGLA 65%] Bytes necesarios: " << bytes_necesarios
         << " | Bytes anteriores: " << bytes_anteriores
         << " | Uso real: " << (int)(porcentaje_uso * 100) << "%\n";

    if (porcentaje_uso < 0.65 && bytes_necesarios < bytes_anteriores) {
        cout << "[REGLA 65%] Ocupacion menor al 65%. Redimensionando memoria fisica...\n";

        unsigned char* nuevo_buffer = new unsigned char[bytes_necesarios]();
        for (int i = 0; i < total_posiciones; i++) {
            unsigned char valor = obtenerFicha(buffer, i, bytes_anteriores);
            asignarFicha(nuevo_buffer, i, valor, bytes_necesarios);
        }

        delete[] buffer;
        buffer       = nuevo_buffer;
        bytes_totales = bytes_necesarios;

        cout << "[REGLA 65%] Memoria reducida exitosamente a " << bytes_totales << " bytes.\n";
    } else {
        cout << "[REGLA 65%] Ocupacion suficiente (>= 65%). Se mantiene el buffer de "
             << bytes_anteriores << " bytes.\n";
    }
}

// ============================================================
// 8. MODIFICACIÓN ESTRUCTURAL
// ============================================================
void agregarFila(unsigned char*& buffer, int& filas, int columnas,
                 int& total_posiciones, int& bytes_totales,
                 unsigned int& semilla, int pos_fila) {
    int bytes_anteriores = bytes_totales;
    int nuevas_filas     = filas + 1;
    int nuevo_total      = nuevas_filas * columnas;
    int nuevos_bytes     = calcular_bytes_necesarios(nuevas_filas, columnas);

    unsigned char* nuevo_buffer = new unsigned char[nuevos_bytes]();
    int pos_vieja = 0, pos_nueva = 0;

    for (int f = 0; f < nuevas_filas; f++) {
        for (int c = 0; c < columnas; c++) {
            unsigned char valor;
            if (f == pos_fila) {
                valor = generarFichaUniforme(semilla);
            } else {
                valor = obtenerFicha(buffer, pos_vieja, bytes_anteriores);
                pos_vieja++;
            }
            asignarFicha(nuevo_buffer, pos_nueva, valor, nuevos_bytes);
            pos_nueva++;
        }
    }

    delete[] buffer;
    buffer            = nuevo_buffer;
    filas             = nuevas_filas;
    total_posiciones  = nuevo_total;
    bytes_totales     = nuevos_bytes;

    cout << "\n[FILA AGREGADA] Se agrego una fila en la posicion " << (pos_fila + 1) << ".\n";
}

void eliminarFila(unsigned char*& buffer, int& filas, int columnas,
                  int& total_posiciones, int& bytes_totales, int pos_fila) {
    if (filas <= 3) {
        cout << "\n[ERROR] No se puede eliminar. Deben quedar al menos 3 filas.\n";
        return;
    }

    int bytes_anteriores = bytes_totales;
    int nuevas_filas     = filas - 1;
    int nuevo_total      = nuevas_filas * columnas;
    int nuevos_bytes     = calcular_bytes_necesarios(nuevas_filas, columnas);

    unsigned char* nuevo_buffer = new unsigned char[nuevos_bytes]();
    int pos_vieja = 0, pos_nueva = 0;

    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < columnas; c++) {
            if (f == pos_fila) {
                pos_vieja++;
            } else {
                unsigned char valor = obtenerFicha(buffer, pos_vieja, bytes_anteriores);
                asignarFicha(nuevo_buffer, pos_nueva, valor, nuevos_bytes);
                pos_vieja++;
                pos_nueva++;
            }
        }
    }

    delete[] buffer;
    buffer            = nuevo_buffer;
    filas             = nuevas_filas;
    total_posiciones  = nuevo_total;
    bytes_totales     = nuevos_bytes;

    cout << "\n[FILA ELIMINADA] Se elimino la fila en la posicion " << (pos_fila + 1) << ".\n";

    aplicarRegla65(buffer, filas, columnas, total_posiciones, bytes_totales, bytes_anteriores);
}

void agregarColumna(unsigned char*& buffer, int filas, int& columnas,
                    int& total_posiciones, int& bytes_totales,
                    unsigned int& semilla, int pos_col) {
    int bytes_anteriores  = bytes_totales;
    int nuevas_columnas   = columnas + 1;
    int nuevo_total       = filas * nuevas_columnas;
    int nuevos_bytes      = calcular_bytes_necesarios(filas, nuevas_columnas);

    unsigned char* nuevo_buffer = new unsigned char[nuevos_bytes]();
    int pos_vieja = 0, pos_nueva = 0;

    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < nuevas_columnas; c++) {
            unsigned char valor;
            if (c == pos_col) {
                valor = generarFichaUniforme(semilla);
            } else {
                valor = obtenerFicha(buffer, pos_vieja, bytes_anteriores);
                pos_vieja++;
            }
            asignarFicha(nuevo_buffer, pos_nueva, valor, nuevos_bytes);
            pos_nueva++;
        }
    }

    delete[] buffer;
    buffer            = nuevo_buffer;
    columnas          = nuevas_columnas;
    total_posiciones  = nuevo_total;
    bytes_totales     = nuevos_bytes;

    cout << "\n[COLUMNA AGREGADA] Se agrego una columna en la posicion " << (pos_col + 1) << ".\n";
}

void eliminarColumna(unsigned char*& buffer, int filas, int& columnas,
                     int& total_posiciones, int& bytes_totales, int pos_col) {
    if (columnas <= 3) {
        cout << "\n[ERROR] No se puede eliminar. Deben quedar al menos 3 columnas.\n";
        return;
    }

    int bytes_anteriores = bytes_totales;
    int nuevas_columnas  = columnas - 1;
    int nuevo_total      = filas * nuevas_columnas;
    int nuevos_bytes     = calcular_bytes_necesarios(filas, nuevas_columnas);

    unsigned char* nuevo_buffer = new unsigned char[nuevos_bytes]();
    int pos_vieja = 0, pos_nueva = 0;

    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < columnas; c++) {
            if (c == pos_col) {
                pos_vieja++;
            } else {
                unsigned char valor = obtenerFicha(buffer, pos_vieja, bytes_anteriores);
                asignarFicha(nuevo_buffer, pos_nueva, valor, nuevos_bytes);
                pos_vieja++;
                pos_nueva++;
            }
        }
    }

    delete[] buffer;
    buffer            = nuevo_buffer;
    columnas          = nuevas_columnas;
    total_posiciones  = nuevo_total;
    bytes_totales     = nuevos_bytes;

    cout << "\n[COLUMNA ELIMINADA] Se elimino la columna en la posicion " << (pos_col + 1) << ".\n";

    aplicarRegla65(buffer, filas, columnas, total_posiciones, bytes_totales, bytes_anteriores);
}