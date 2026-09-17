#include <iostream>
#include "estadisticas.h"
using namespace std;

// ============================================================
// 9. ESTADÍSTICAS
// ============================================================
void mostrarEstadisticas(int filas, int columnas, int bytes_totales,
                         int eliminaciones_usuario, int total_fichas_eliminadas,
                         int total_combinaciones, int total_cascadas, int puntuacion) {
    cout << "\n=== ESTADISTICAS ===\n";
    cout << "Dimensiones actuales:           " << filas << " x " << columnas << "\n";
    cout << "Memoria reservada:              " << bytes_totales << " bytes\n";
    cout << "Eliminaciones del usuario:      " << eliminaciones_usuario << "\n";
    cout << "Fichas eliminadas en total:     " << total_fichas_eliminadas << "\n";
    cout << "Combinaciones detectadas:       " << total_combinaciones << "\n";
    cout << "Cascadas producidas en total:   " << total_cascadas << "\n";
    cout << "Puntuacion:                     " << puntuacion << "\n";
    cout << "====================\n";
}