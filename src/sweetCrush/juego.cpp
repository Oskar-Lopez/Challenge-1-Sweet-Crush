#include <iostream>
#include "juego.h"
#include "bits.h"
#include "tablero.h"
using namespace std;

// ============================================================
// 5. LÓGICA DEL JUEGO
// ============================================================
int detectarCombinaciones(const unsigned char* buffer, int filas, int columnas,
                          int total_posiciones, bool* marcados, int bytes_totales) {
    for (int i = 0; i < total_posiciones; i++) marcados[i] = false;

    int combinaciones = 0;

    // ---- Horizontal ----
    for (int fila = 0; fila < filas; fila++) {
        int columna = 0;
        while (columna < columnas - 2) {
            int pos = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, pos, bytes_totales);
            if (valor == 0 || valor == 7) { columna++; continue; }

            int longitud = 1;
            while (columna + longitud < columnas) {
                int pos_sig = fila * columnas + (columna + longitud);
                if (obtenerFicha(buffer, pos_sig, bytes_totales) == valor) longitud++;
                else break;
            }
            if (longitud >= 3) {
                for (int k = 0; k < longitud; k++) {
                    marcados[fila * columnas + (columna + k)] = true;
                }
                combinaciones++;
            }
            columna += longitud;
        }
    }

    // ---- Vertical ----
    for (int columna = 0; columna < columnas; columna++) {
        int fila = 0;
        while (fila < filas - 2) {
            int pos = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, pos, bytes_totales);
            if (valor == 0 || valor == 7) { fila++; continue; }

            int longitud = 1;
            while (fila + longitud < filas) {
                int pos_sig = (fila + longitud) * columnas + columna;
                if (obtenerFicha(buffer, pos_sig, bytes_totales) == valor) longitud++;
                else break;
            }
            if (longitud >= 3) {
                for (int k = 0; k < longitud; k++) {
                    marcados[(fila + k) * columnas + columna] = true;
                }
                combinaciones++;
            }
            fila += longitud;
        }
    }

    return combinaciones;
}

int eliminarFichasMarcadas(unsigned char* buffer, int total_posiciones,
                           bool* marcados, int bytes_totales) {
    int eliminadas = 0;
    for (int i = 0; i < total_posiciones; i++) {
        if (marcados[i]) {
            asignarFicha(buffer, i, 0, bytes_totales);
            eliminadas++;
        }
    }
    return eliminadas;
}

void aplicarGravedad(unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    for (int c = 0; c < columnas; c++) {
        int destino = filas - 1;
        for (int f = filas - 1; f >= 0; f--) {
            int pos_lectura = f * columnas + c;
            unsigned char valor = obtenerFicha(buffer, pos_lectura, bytes_totales);
            if (valor != 0) {
                int pos_destino = destino * columnas + c;
                asignarFicha(buffer, pos_destino, valor, bytes_totales);
                if (pos_lectura != pos_destino) {
                    asignarFicha(buffer, pos_lectura, 0, bytes_totales);
                }
                destino--;
            }
        }
    }
}

void rellenarHuecos(unsigned char* buffer, int filas, int columnas,
                    unsigned int& semilla, int bytes_totales) {
    for (int c = 0; c < columnas; c++) {
        for (int f = 0; f < filas; f++) {
            int pos = f * columnas + c;
            if (obtenerFicha(buffer, pos, bytes_totales) == 0) {
                asignarFicha(buffer, pos, generarFichaUniforme(semilla), bytes_totales);
            }
        }
    }
}

// ============================================================
// 7. CASCADAS AUTOMÁTICAS
// ============================================================
void procesarCascadas(unsigned char* buffer, int filas, int columnas, int total_posiciones,
                      int bytes_totales, unsigned int& semilla, int& puntuacion,
                      int& total_cascadas, int& total_combinaciones,
                      int& total_fichas_eliminadas, int& cascadas_de_esta_operacion) {
    bool* marcados = new bool[total_posiciones];
    bool hay_combinaciones = true;
    int cascadas_en_turno = 0;

    cout << "\n--- INICIANDO CASCADAS AUTOMATICAS ---\n";

    while (hay_combinaciones) {
        int combinaciones = detectarCombinaciones(buffer, filas, columnas,
                                                  total_posiciones, marcados, bytes_totales);

        int fichas_eliminadas = 0;
        for (int i = 0; i < total_posiciones; i++) {
            if (marcados[i]) fichas_eliminadas++;
        }

        if (fichas_eliminadas == 0) {
            hay_combinaciones = false;
        } else {
            cascadas_en_turno++;
            total_combinaciones += combinaciones;

            cout << "\n=== CASCADA NUMERO " << cascadas_en_turno << " ===\n";
            cout << "Se detectaron " << combinaciones << " combinaciones y "
                 << fichas_eliminadas << " fichas para eliminar.\n";

            eliminarFichasMarcadas(buffer, total_posiciones, marcados, bytes_totales);
            cout << "[Paso 1] Despues de eliminar (huecos = '.'):\n";
            mostrarTableroFichas(buffer, filas, columnas, bytes_totales);

            aplicarGravedad(buffer, filas, columnas, bytes_totales);
            cout << "[Paso 2] Despues de aplicar gravedad (fichas cayeron):\n";
            mostrarTableroFichas(buffer, filas, columnas, bytes_totales);

            rellenarHuecos(buffer, filas, columnas, semilla, bytes_totales);
            cout << "[Paso 3] Despues de rellenar huecos (nuevas fichas):\n";
            mostrarTableroFichas(buffer, filas, columnas, bytes_totales);

            puntuacion              += fichas_eliminadas * 10;
            total_fichas_eliminadas += fichas_eliminadas;
        }
    }

    total_cascadas             += cascadas_en_turno;
    cascadas_de_esta_operacion  = cascadas_en_turno;

    if (cascadas_en_turno > 0) {
        cout << "--- FIN DE CASCADAS (Tablero estable). Cascadas en esta operacion: "
             << cascadas_en_turno << " ---\n";
    } else {
        cout << "--- NO HUBO CASCADAS EN ESTA OPERACION ---\n";
    }

    delete[] marcados;
}