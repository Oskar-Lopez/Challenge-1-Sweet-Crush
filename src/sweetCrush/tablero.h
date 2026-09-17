#ifndef TABLERO_H
#define TABLERO_H

// Crear el tablero con dimensiones dadas
void crearTablero(unsigned char*& buffer, int filas, int columnas,
                  int& total_posiciones, int& bytes_totales, unsigned int& semilla);

// Mostrar el tablero en formato fichas
void mostrarTableroFichas(const unsigned char* buffer, int filas, int columnas, int bytes_totales);

// Mostrar la memoria en binario
void mostrarMemoriaBinaria(const unsigned char* buffer, int bytes_totales);

#endif