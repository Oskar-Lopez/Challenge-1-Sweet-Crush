#include <iostream>
#include "bits.h"
#include "tablero.h"
#include "juego.h"
#include "estructura.h"
#include "estadisticas.h"
using namespace std;

// ============================================================
// 10. FUNCIÓN PRINCIPAL
// ============================================================
int main() {
    int filas, columnas;
    unsigned int semilla = 1;
    int total_posiciones = 0;
    int bytes_totales    = 0;
    unsigned char* tablero = nullptr;

    int puntuacion              = 0;
    int total_cascadas          = 0;
    int total_combinaciones     = 0;
    int total_fichas_eliminadas = 0;
    int eliminaciones_usuario   = 0;

    int opcion = 0;

    cout << "=== SWEET CRUSH - INICIO ===\n";
    cout << "Filas: ";    cin >> filas;
    cout << "Columnas: "; cin >> columnas;
    cout << "Semilla: ";  cin >> semilla;

    crearTablero(tablero, filas, columnas, total_posiciones, bytes_totales, semilla);

    do {
        cout << "\n=== MENU PRINCIPAL ===\n";
        cout << "Tablero actual: " << filas << " filas x " << columnas << " columnas\n";
        cout << "Memoria reservada: " << bytes_totales << " bytes\n";
        cout << "Puntuacion: " << puntuacion
             << " | Cascadas: " << total_cascadas
             << " | Eliminaciones usuario: " << eliminaciones_usuario << "\n";
        cout << "1. Eliminar ficha (por coordenadas) y ver cascadas\n";
        cout << "2. Agregar una fila\n";
        cout << "3. Eliminar una fila\n";
        cout << "4. Agregar una columna\n";
        cout << "5. Eliminar una columna\n";
        cout << "6. Mostrar tablero y memoria binaria\n";
        cout << "7. Mostrar estadisticas completas\n";
        cout << "8. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;

        int cascadas_de_esta_operacion = 0;

        if (opcion == 1) {
            int f, c;
            cout << "Ingrese fila (1 a " << filas << "): ";    cin >> f;
            cout << "Ingrese columna (1 a " << columnas << "): "; cin >> c;

            if (f >= 1 && f <= filas && c >= 1 && c <= columnas) {
                int pos = (f - 1) * columnas + (c - 1);
                cout << "\n[JUGADA] Eliminando ficha en (" << f << ", " << c << ")...\n";

                asignarFicha(tablero, pos, 0, bytes_totales);
                eliminaciones_usuario++;
                total_fichas_eliminadas++;
                puntuacion += 10;

                aplicarGravedad(tablero, filas, columnas, bytes_totales);
                rellenarHuecos(tablero, filas, columnas, semilla, bytes_totales);

                cout << "[JUGADA] Tablero despues de la jugada manual:\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

                procesarCascadas(tablero, filas, columnas, total_posiciones, bytes_totales,
                                 semilla, puntuacion, total_cascadas,
                                 total_combinaciones, total_fichas_eliminadas,
                                 cascadas_de_esta_operacion);

                cout << "\n[TABLERO FINAL ESTABLE]:\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
                cout << "[RESUMEN JUGADA] Cascadas producidas por esta eliminacion: "
                     << cascadas_de_esta_operacion << "\n";
            } else {
                cout << "Coordenadas invalidas. Intente de nuevo.\n";
            }
        }
        else if (opcion == 2) {
            int pos_fila;
            cout << "Ingrese posicion de la fila a agregar (1 a " << (filas + 1) << "): ";
            cin >> pos_fila;
            if (pos_fila >= 1 && pos_fila <= filas + 1) {
                agregarFila(tablero, filas, columnas, total_posiciones, bytes_totales,
                            semilla, pos_fila - 1);
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

                procesarCascadas(tablero, filas, columnas, total_posiciones, bytes_totales,
                                 semilla, puntuacion, total_cascadas,
                                 total_combinaciones, total_fichas_eliminadas,
                                 cascadas_de_esta_operacion);
                cout << "[RESUMEN] Cascadas producidas por esta operacion: "
                     << cascadas_de_esta_operacion << "\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            } else {
                cout << "Posicion invalida.\n";
            }
        }
        else if (opcion == 3) {
            int pos_fila;
            cout << "Ingrese posicion de la fila a eliminar (1 a " << filas << "): ";
            cin >> pos_fila;
            if (pos_fila >= 1 && pos_fila <= filas) {
                eliminarFila(tablero, filas, columnas, total_posiciones, bytes_totales, pos_fila - 1);
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

                procesarCascadas(tablero, filas, columnas, total_posiciones, bytes_totales,
                                 semilla, puntuacion, total_cascadas,
                                 total_combinaciones, total_fichas_eliminadas,
                                 cascadas_de_esta_operacion);
                cout << "[RESUMEN] Cascadas producidas por esta operacion: "
                     << cascadas_de_esta_operacion << "\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            } else {
                cout << "Posicion invalida.\n";
            }
        }
        else if (opcion == 4) {
            int pos_col;
            cout << "Ingrese posicion de la columna a agregar (1 a " << (columnas + 1) << "): ";
            cin >> pos_col;
            if (pos_col >= 1 && pos_col <= columnas + 1) {
                agregarColumna(tablero, filas, columnas, total_posiciones, bytes_totales,
                               semilla, pos_col - 1);
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

                procesarCascadas(tablero, filas, columnas, total_posiciones, bytes_totales,
                                 semilla, puntuacion, total_cascadas,
                                 total_combinaciones, total_fichas_eliminadas,
                                 cascadas_de_esta_operacion);
                cout << "[RESUMEN] Cascadas producidas por esta operacion: "
                     << cascadas_de_esta_operacion << "\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            } else {
                cout << "Posicion invalida.\n";
            }
        }
        else if (opcion == 5) {
            int pos_col;
            cout << "Ingrese posicion de la columna a eliminar (1 a " << columnas << "): ";
            cin >> pos_col;
            if (pos_col >= 1 && pos_col <= columnas) {
                eliminarColumna(tablero, filas, columnas, total_posiciones, bytes_totales, pos_col - 1);
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

                procesarCascadas(tablero, filas, columnas, total_posiciones, bytes_totales,
                                 semilla, puntuacion, total_cascadas,
                                 total_combinaciones, total_fichas_eliminadas,
                                 cascadas_de_esta_operacion);
                cout << "[RESUMEN] Cascadas producidas por esta operacion: "
                     << cascadas_de_esta_operacion << "\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            } else {
                cout << "Posicion invalida.\n";
            }
        }
        else if (opcion == 6) {
            mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            mostrarMemoriaBinaria(tablero, bytes_totales);
        }
        else if (opcion == 7) {
            mostrarEstadisticas(filas, columnas, bytes_totales,
                                eliminaciones_usuario, total_fichas_eliminadas,
                                total_combinaciones, total_cascadas, puntuacion);
        }

    } while (opcion != 8);

    if (tablero != nullptr) {
        delete[] tablero;
        tablero = nullptr;
    }

    cout << "\n[INFO] Juego terminado. Puntuacion final: " << puntuacion << "\n";
    return 0;
}