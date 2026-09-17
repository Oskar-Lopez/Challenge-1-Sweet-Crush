#ifndef JUEGO_H
#define JUEGO_H

// Detectar combinaciones de 3 o más fichas
int detectarCombinaciones(const unsigned char* buffer, int filas, int columnas,
                          int total_posiciones, bool* marcados, int bytes_totales);

// Eliminar las fichas marcadas
int eliminarFichasMarcadas(unsigned char* buffer, int total_posiciones,
                           bool* marcados, int bytes_totales);

// Aplicar gravedad
void aplicarGravedad(unsigned char* buffer, int filas, int columnas, int bytes_totales);

// Rellenar huecos con fichas nuevas
void rellenarHuecos(unsigned char* buffer, int filas, int columnas,
                    unsigned int& semilla, int bytes_totales);

// Procesar cascadas automáticas
void procesarCascadas(unsigned char* buffer, int filas, int columnas, int total_posiciones,
                      int bytes_totales, unsigned int& semilla, int& puntuacion,
                      int& total_cascadas, int& total_combinaciones,
                      int& total_fichas_eliminadas, int& cascadas_de_esta_operacion);

#endif