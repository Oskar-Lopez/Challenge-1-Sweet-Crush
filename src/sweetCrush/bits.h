#ifndef BITS_H
#define BITS_H

// Generador pseudoaleatorio
unsigned int siguienteAleatorio(unsigned int& semilla);

// Genera un entero uniforme en [1, 6]
int generarFichaUniforme(unsigned int& semilla);

// Cálculo de bytes necesarios según dimensiones
int calcular_bytes_necesarios(int filas, int columnas);

// Guardar una ficha (3 bits) en el buffer
void asignarFicha(unsigned char* buffer, int posicion, unsigned char valor, int bytes_totales);

// Leer una ficha (3 bits) del buffer
unsigned char obtenerFicha(const unsigned char* buffer, int posicion, int bytes_totales);

#endif