// ============================================================
// Sweet Crush - Prototipo: Tablero, Fichas y Detección de Combinaciones
// Objetivo: Probar la lógica de bits y la detección de cruces antes de modularizar.
// ============================================================
#include <iostream>
using namespace std;

// ------------------------------------------------------------
// Generador pseudoaleatorio simple (LCG)
// Semilla definida por el usuario.
// ------------------------------------------------------------
unsigned int siguienteAleatorio(unsigned int& semilla) {
    semilla = semilla * 1103515245u + 12345u;
    return (semilla >> 16) & 0x7FFF;
}

//Yudis Apreciado: funcionalidad reserva de memoria.
//------------------------------------------------
//  calculo de memoria necesaria.
//------------------------------------------------
unsigned int calcular_bytes_necesarios(int filas, int columnas) {
    int total_fichas = filas * columnas;
    int total_bits = total_fichas * 3;
    return (total_bits + 7) / 8; // el 7 redondea al siguiente byte completo.
}

// ------------------------------------------------------------
// Escribe una ficha (3 bits) en la posición lógica indicada
// ------------------------------------------------------------
void asignarFicha(unsigned char* buffer, int total_posiciones, int posicion, unsigned char valor) {
    valor &= 0x07; // Limitar a 3 bits
    int bit_inicial = posicion * 3;
    int indice_byte = bit_inicial / 8;
    int desplazamiento = bit_inicial % 8;

    if (desplazamiento <= 5) {
        unsigned char mascara = 0x07 << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara) | (valor << desplazamiento);
    } else {
        int bits_primer_byte = 8 - desplazamiento;
        int bits_segundo_byte = 3 - bits_primer_byte;

        unsigned char mascara1 = ((1 << bits_primer_byte) - 1) << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara1) |
                              ((valor & ((1 << bits_primer_byte) - 1)) << desplazamiento);

        unsigned char mascara2 = (1 << bits_segundo_byte) - 1;
        buffer[indice_byte + 1] = (buffer[indice_byte + 1] & ~mascara2) |
                                  (valor >> bits_primer_byte);
    }
}

// ------------------------------------------------------------
// Leer una ficha (3 bits) de la posición lógica indicada
// ------------------------------------------------------------
unsigned char obtenerFicha(const unsigned char* buffer, int total_posiciones, int posicion) {
    int bit_inicial = posicion * 3;
    int indice_byte = bit_inicial / 8;
    int desplazamiento = bit_inicial % 8;

    if (desplazamiento <= 5) {
        return (buffer[indice_byte] >> desplazamiento) & 0x07;
    } else {
        int bits_primer_byte = 8 - desplazamiento;
        int bits_segundo_byte = 3 - bits_primer_byte;

        unsigned char parte1 = (buffer[indice_byte] >> desplazamiento) &
                               ((1 << bits_primer_byte) - 1);
        unsigned char parte2 = (buffer[indice_byte + 1] &
                                ((1 << bits_segundo_byte) - 1)) << bits_primer_byte;
        return parte1 | parte2;
    }
}

// ------------------------------------------------------------
// Crear el tablero y generar fichas pseudoaleatorias
// ------------------------------------------------------------
void crearTablero(unsigned char*& buffer, int filas, int columnas, int& total_posiciones, int& bytes_totales, unsigned int& semilla) {
    total_posiciones = filas * columnas;
    bytes_totales = calcular_bytes_necesarios(filas, columnas);

    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }

    buffer = new unsigned char[bytes_totales]();
    for (int i = 0; i < bytes_totales; i++) buffer[i] = 0;

    // fichas del 1 al 6 (reserva 0 para vacios y 7 para marcados)
    for (int i = 0; i < total_posiciones; i++) {
        // TRUCO DE PRUEBA: Cambia '% 6' por '% 3' para forzar combinaciones y ver las 'X'.
        asignarFicha(buffer, total_posiciones, i, (siguienteAleatorio(semilla) % 6) + 1);
    }
}

// ------------------------------------------------------------
// Mostrar el tablero en formato fichas
// ------------------------------------------------------------
void mostrarTableroFichas(const unsigned char* buffer, int filas, int columnas, int total_posiciones) {
    cout << "\n=== TABLERO (FICHAS) ===\n";
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            int posicion = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, total_posiciones, posicion);

            if (valor == 0) cout << ".  ";       // Vacío
            else if (valor == 7) cout << "X  ";  // Marcado
            else cout << (char)('A' + valor - 1) << "  "; // 1-6 a A-F
        }
        cout << "\n";
    }
    cout << "========================\n";
}

//-----------------------------------------
// Yudis Apreciado: funcion mostrar memoria en binario (8 bits por byte).
//-----------------------------------------
void mostrarMemoriaBinaria(const unsigned char* buffer, int bytes_totales) {
    cout << "\n=== ESTADO EN MEMORIA (BINARIO CRUDO) ===\n";
    cout << "Total Bytes reservados: " << bytes_totales << "\n";
    for(int i = 0; i < bytes_totales; i++) {
        cout << "Byte [" << i << "]: ";
        for(int bit = 7; bit >= 0; bit--) {
            cout << ((buffer[i] >> bit) & 1);
        }
        cout << "\n";
    }
    cout << "===========================================\n";
}

// ------------------------------------------------------------
// LÓGICA DE JUEGO: Detectar combinaciones horizontales y verticales
// ------------------------------------------------------------
void detectarCombinaciones(const unsigned char* buffer, int filas, int columnas,
                           int total_posiciones, bool* marcados) {
    for (int i = 0; i < total_posiciones; i++) marcados[i] = false;

    // DETECCIÓN HORIZONTAL
    for (int fila = 0; fila < filas; fila++) {
        int columna = 0;
        while (columna < columnas - 2) {
            int posicion_actual = fila * columnas + columna;
            unsigned char valor_actual = obtenerFicha(buffer, total_posiciones, posicion_actual);

            if (valor_actual == 0 || valor_actual == 7) { columna++; continue; }

            int longitud = 1;
            while (columna + longitud < columnas) {
                int posicion_siguiente = fila * columnas + (columna + longitud);
                unsigned char valor_siguiente = obtenerFicha(buffer, total_posiciones, posicion_siguiente);
                if (valor_siguiente == valor_actual) longitud++;
                else break;
            }

            if (longitud >= 3) {
                for (int k = 0; k < longitud; k++) {
                    marcados[fila * columnas + (columna + k)] = true;
                }
            }
            columna += longitud;
        }
    }

    // DETECCIÓN VERTICAL
    for (int columna = 0; columna < columnas; columna++) {
        int fila = 0;
        while (fila < filas - 2) {
            int posicion_actual = fila * columnas + columna;
            unsigned char valor_actual = obtenerFicha(buffer, total_posiciones, posicion_actual);

            if (valor_actual == 0 || valor_actual == 7) { fila++; continue; }

            int longitud = 1;
            while (fila + longitud < filas) {
                int posicion_siguiente = (fila + longitud) * columnas + columna;
                unsigned char valor_siguiente = obtenerFicha(buffer, total_posiciones, posicion_siguiente);
                if (valor_siguiente == valor_actual) longitud++;
                else break;
            }

            if (longitud >= 3) {
                for (int k = 0; k < longitud; k++) {
                    marcados[(fila + k) * columnas + columna] = true;
                }
            }
            fila += longitud;
        }
    }
}

// ------------------------------------------------------------
// main: Punto de entrada interactivo
// ------------------------------------------------------------
int main() {
    int filas, columnas;
    unsigned int semilla = 1;
    int total_posiciones = 0, bytes_totales = 0;
    unsigned char* tablero = nullptr;

    cout << "=== INICIO DEL PROTOTIPO ===\n";
    cout << "Filas: ";    cin >> filas;
    cout << "Columnas: "; cin >> columnas;
    cout << "Semilla: ";  cin >> semilla;

    crearTablero(tablero, filas, columnas, total_posiciones, bytes_totales, semilla);

    cout << "\n[INFO] Tablero creado: " << filas << "x" << columnas
         << " | Bytes reservados: " << bytes_totales << "\n";

    mostrarTableroFichas(tablero, filas, columnas, total_posiciones);

    // LLAMADA A LA FUNCIÓN DE YUDIS (Mostrar memoria cruda)
    mostrarMemoriaBinaria(tablero, bytes_totales);

    bool* marcados = new bool[total_posiciones];
    detectarCombinaciones(tablero, filas, columnas, total_posiciones, marcados);

    int total_marcadas = 0;
    for (int i = 0; i < total_posiciones; i++) {
        if (marcados[i]) {
            total_marcadas++;
            asignarFicha(tablero, total_posiciones, i, 7); // Marcar visualmente con 'X'
        }
    }

    cout << "\n[RESULTADO] Se detectaron " << total_marcadas << " fichas en combinación.\n";
    if (total_marcadas > 0) {
        cout << "Las fichas marcadas para eliminar se muestran como 'X' abajo:\n";
        mostrarTableroFichas(tablero, filas, columnas, total_posiciones);
    }

    delete[] marcados;
    if (tablero != nullptr) {
        delete[] tablero;
        tablero = nullptr;
    }

    cout << "\n[INFO] Memoria liberada. Fin del prototipo.\n";
    return 0;
}