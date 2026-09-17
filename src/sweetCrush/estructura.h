#ifndef ESTRUCTURA_H
#define ESTRUCTURA_H

// Regla del 65%
void aplicarRegla65(unsigned char*& buffer, int filas, int columnas,
                    int total_posiciones, int& bytes_totales, int bytes_anteriores);

// Modificación estructural
void agregarFila(unsigned char*& buffer, int& filas, int columnas,
                 int& total_posiciones, int& bytes_totales,
                 unsigned int& semilla, int pos_fila);
void eliminarFila(unsigned char*& buffer, int& filas, int columnas,
                  int& total_posiciones, int& bytes_totales, int pos_fila);
void agregarColumna(unsigned char*& buffer, int filas, int& columnas,
                    int& total_posiciones, int& bytes_totales,
                    unsigned int& semilla, int pos_col);
void eliminarColumna(unsigned char*& buffer, int filas, int& columnas,
                     int& total_posiciones, int& bytes_totales, int pos_col);

#endif