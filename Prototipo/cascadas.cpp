// ============================================================
// Sweet Crush - Prototipo: Juego Interactivo con Cascadas
// Estudiantes: Yudis Apreciado, Oscar Lopez
// Curso: Informática 2 C++
// ============================================================
#include <iostream>
using namespace std;

// ------------------------------------------------------------
// Función para generar números pseudoaleatorios (LCG)
// ------------------------------------------------------------
unsigned int siguienteAleatorio(unsigned int& semilla) {
    semilla = semilla * 1103515245u + 12345u;
    return (semilla >> 16) & 0x7FFF;
}

// ------------------------------------------------------------
// Aporte de Yudis Apreciado: Calcula cuántos bytes necesitamos
// CORRECCIÓN: Se quitó el +2 para usar la memoria exacta que pide el desafío
// ------------------------------------------------------------
int calcular_bytes_necesarios(int filas, int columnas) {
    int total_fichas = filas * columnas;
    int total_bits = total_fichas * 3;
    // Cálculo exacto de bytes
    return (total_bits + 7) / 8;
}

// ------------------------------------------------------------
// Guardar una ficha en el buffer (3 bits)
// ------------------------------------------------------------
void asignarFicha(unsigned char* buffer, int posicion, unsigned char valor, int bytes_totales) {
    valor = valor & 0x07;
    int bit_inicial = posicion * 3;
    int indice_byte = bit_inicial / 8;
    int desplazamiento = bit_inicial % 8;

    // Validación de límites para evitar warnings de acceso a memoria
    if (indice_byte >= bytes_totales) return;

    if (desplazamiento <= 5) {
        unsigned char mascara = 0x07 << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara) | (valor << desplazamiento);
    } else {
        int bits_primer_byte = 8 - desplazamiento;
        unsigned char mascara1 = ((1 << bits_primer_byte) - 1) << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara1) |
                              ((valor & ((1 << bits_primer_byte) - 1)) << desplazamiento);

        // Validamos que exista el segundo byte antes de escribir
        if (indice_byte + 1 < bytes_totales) {
            int bits_segundo_byte = 3 - bits_primer_byte;
            unsigned char mascara2 = (1 << bits_segundo_byte) - 1;
            buffer[indice_byte + 1] = (buffer[indice_byte + 1] & ~mascara2) |
                                      (valor >> bits_primer_byte);
        }
    }
}

// ------------------------------------------------------------
// Leer una ficha del buffer (3 bits)
// ------------------------------------------------------------
unsigned char obtenerFicha(const unsigned char* buffer, int posicion, int bytes_totales) {
    int bit_inicial = posicion * 3;
    int indice_byte = bit_inicial / 8;
    int desplazamiento = bit_inicial % 8;

    // Validación de límites
    if (indice_byte >= bytes_totales) return 0;

    if (desplazamiento <= 5) {
        return (buffer[indice_byte] >> desplazamiento) & 0x07;
    } else {
        int bits_primer_byte = 8 - desplazamiento;
        int bits_segundo_byte = 3 - bits_primer_byte;

        unsigned char parte1 = (buffer[indice_byte] >> desplazamiento) &
                               ((1 << bits_primer_byte) - 1);

        unsigned char parte2 = 0;
        // Validamos que exista el segundo byte antes de leer
        if (indice_byte + 1 < bytes_totales) {
            parte2 = (buffer[indice_byte + 1] &
                      ((1 << bits_segundo_byte) - 1)) << bits_primer_byte;
        }
        return parte1 | parte2;
    }
}

// ------------------------------------------------------------
// Crear el tablero
// ------------------------------------------------------------
void crearTablero(unsigned char*& buffer, int filas, int columnas, int& total_posiciones, int& bytes_totales, unsigned int& semilla) {
    total_posiciones = filas * columnas;
    bytes_totales = calcular_bytes_necesarios(filas, columnas);

    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }

    buffer = new unsigned char[bytes_totales]();

    for (int i = 0; i < total_posiciones; i++) {
        int ficha = (siguienteAleatorio(semilla) % 6) + 1;
        asignarFicha(buffer, i, ficha, bytes_totales);
    }
}

// ------------------------------------------------------------
// Mostrar el tablero en pantalla
// ------------------------------------------------------------
void mostrarTableroFichas(const unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    cout << "\n=== TABLERO (FICHAS) ===\n";
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            int posicion = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, posicion, bytes_totales);

            if (valor == 0) cout << ".  ";
            else if (valor == 7) cout << "X  ";
            else cout << char('A' + valor - 1) << "  ";
        }
        cout << "\n";
    }
    cout << "========================\n";
}

//-----------------------------------------
// Aporte de Yudis Apreciado: Muestra la memoria en binario
//-----------------------------------------
void mostrarMemoriaBinaria(const unsigned char* buffer, int bytes_totales) {
    cout << "\n=== ESTADO EN MEMORIA (BINARIO) ===\n";
    cout << "Total Bytes reservados: " << bytes_totales << "\n";

    for(int i = 0; i < bytes_totales; i++) {
        cout << "Byte [" << i << "]: ";
        for(int bit = 7; bit >= 0; bit--) {
            cout << ((buffer[i] >> bit) & 1);
        }
        cout << "\n";
    }
    cout << "===================================\n";
}

// ------------------------------------------------------------
// Detecta combinaciones de 3 o más fichas
// ------------------------------------------------------------
void detectarCombinaciones(const unsigned char* buffer, int filas, int columnas,
                           int total_posiciones, bool* marcados, int bytes_totales) {
    for (int i = 0; i < total_posiciones; i++) marcados[i] = false;

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
                for (int k = 0; k < longitud; k++) marcados[fila * columnas + (columna + k)] = true;
            }
            columna += longitud;
        }
    }

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
                for (int k = 0; k < longitud; k++) marcados[(fila + k) * columnas + columna] = true;
            }
            fila += longitud;
        }
    }
}

// ------------------------------------------------------------
// Elimina las fichas marcadas
// ------------------------------------------------------------
void eliminarFichasMarcadas(unsigned char* buffer, int total_posiciones, bool* marcados, int bytes_totales) {
    for (int i = 0; i < total_posiciones; i++) {
        if (marcados[i]) asignarFicha(buffer, i, 0, bytes_totales);
    }
}

// ------------------------------------------------------------
// Aplica gravedad
// ------------------------------------------------------------
void aplicarGravedad(unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    for (int c = 0; c < columnas; c++) {
        int destino = filas - 1;
        for (int f = filas - 1; f >= 0; f--) {
            int pos_lectura = f * columnas + c;
            unsigned char valor = obtenerFicha(buffer, pos_lectura, bytes_totales);
            if (valor != 0) {
                int pos_destino = destino * columnas + c;
                asignarFicha(buffer, pos_destino, valor, bytes_totales);
                if (pos_lectura != pos_destino) asignarFicha(buffer, pos_lectura, 0, bytes_totales);
                destino--;
            }
        }
    }
}

// ------------------------------------------------------------
// Rellena los huecos
// ------------------------------------------------------------
void rellenarHuecos(unsigned char* buffer, int filas, int columnas, unsigned int& semilla, int bytes_totales) {
    for (int c = 0; c < columnas; c++) {
        for (int f = 0; f < filas; f++) {
            int pos = f * columnas + c;
            unsigned char valor = obtenerFicha(buffer, pos, bytes_totales);
            if (valor == 0) {
                int ficha_nueva = (siguienteAleatorio(semilla) % 6) + 1;
                asignarFicha(buffer, pos, ficha_nueva, bytes_totales);
            }
        }
    }
}

// ------------------------------------------------------------
// Procesar Cascadas Automáticas con PASO A PASO
// ------------------------------------------------------------
void procesarCascadas(unsigned char* buffer, int filas, int columnas, int total_posiciones,
                      int bytes_totales, unsigned int& semilla, int& puntuacion, int& total_cascadas) {
    bool* marcados = new bool[total_posiciones];
    bool hay_combinaciones = true;
    int cascadas_en_turno = 0;

    cout << "\n--- INICIANDO CASCADAS AUTOMATICAS ---\n";

    while (hay_combinaciones) {
        detectarCombinaciones(buffer, filas, columnas, total_posiciones, marcados, bytes_totales);

        int fichas_eliminadas = 0;
        for (int i = 0; i < total_posiciones; i++) {
            if (marcados[i]) fichas_eliminadas++;
        }

        if (fichas_eliminadas == 0) {
            hay_combinaciones = false;
        } else {
            cascadas_en_turno++;
            cout << "\n=== CASCADA NUMERO " << cascadas_en_turno << " ===\n";
            cout << "Se detectaron " << fichas_eliminadas << " fichas para eliminar.\n";

            // Paso 1: Eliminar
            eliminarFichasMarcadas(buffer, total_posiciones, marcados, bytes_totales);
            cout << "[Paso 1] Despues de eliminar (huecos = '.'):\n";
            mostrarTableroFichas(buffer, filas, columnas, bytes_totales);

            // Paso 2: Gravedad
            aplicarGravedad(buffer, filas, columnas, bytes_totales);
            cout << "[Paso 2] Despues de aplicar gravedad (fichas cayeron):\n";
            mostrarTableroFichas(buffer, filas, columnas, bytes_totales);

            // Paso 3: Rellenar
            rellenarHuecos(buffer, filas, columnas, semilla, bytes_totales);
            cout << "[Paso 3] Despues de rellenar huecos (nuevas fichas):\n";
            mostrarTableroFichas(buffer, filas, columnas, bytes_totales);

            puntuacion += fichas_eliminadas * 10; // 10 puntos por ficha
        }
    }

    total_cascadas += cascadas_en_turno;
    if (cascadas_en_turno > 0) {
        cout << "--- FIN DE CASCADAS (Tablero estable) ---\n";
    } else {
        cout << "--- NO HUBO CASCADAS EN ESTE TURNO ---\n";
    }
    delete[] marcados;
}

// ------------------------------------------------------------
// Función principal con Menú Interactivo
// ------------------------------------------------------------
int main() {
    int filas, columnas;
    unsigned int semilla = 1;
    int total_posiciones = 0;
    int bytes_totales = 0;
    unsigned char* tablero = nullptr;

    int puntuacion = 0;
    int total_cascadas = 0;
    int opcion = 0;

    cout << "=== SWEET CRUSH - INICIO ===\n";
    cout << "Filas: "; cin >> filas;
    cout << "Columnas: "; cin >> columnas;
    cout << "Semilla: "; cin >> semilla;

    crearTablero(tablero, filas, columnas, total_posiciones, bytes_totales, semilla);

    // Bucle del menú principal
    do {
        cout << "\n=== MENU PRINCIPAL ===\n";
        cout << "Puntuacion: " << puntuacion << " | Cascadas totales: " << total_cascadas << "\n";
        cout << "1. Eliminar ficha (por coordenadas)\n";
        cout << "2. Mostrar tablero y memoria binaria \n";
        cout << "3. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            int f, c;
            cout << "Ingrese fila (1 a " << filas << "): ";
            cin >> f;
            cout << "Ingrese columna (1 a " << columnas << "): ";
            cin >> c;

            if (f >= 1 && f <= filas && c >= 1 && c <= columnas) {
                int pos = (f - 1) * columnas + (c - 1);
                cout << "\n[JUGADA] Eliminando ficha en (" << f << ", " << c << ")...\n";

                // Ponemos la ficha en 0 (vacío) manualmente
                asignarFicha(tablero, pos, 0, bytes_totales);

                // Aplicamos gravedad y rellenamos para que caigan las de arriba
                aplicarGravedad(tablero, filas, columnas, bytes_totales);
                rellenarHuecos(tablero, filas, columnas, semilla, bytes_totales);

                cout << "[JUGADA] Tablero despues de la jugada manual:\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

                // ¡Aquí ocurre la magia de las cascadas automáticas paso a paso!
                procesarCascadas(tablero, filas, columnas, total_posiciones, bytes_totales, semilla, puntuacion, total_cascadas);

                cout << "\n[TABLERO FINAL ESTABLE]:\n";
                mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            } else {
                cout << "Coordenadas invalidas. Intente de nuevo.\n";
            }
        }
        else if (opcion == 2) {
            mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
            mostrarMemoriaBinaria(tablero, bytes_totales); // Aporte de Yudis
        }
    } while (opcion != 3);

    // Liberar memoria al salir
    if (tablero != nullptr) {
        delete[] tablero;
        tablero = nullptr;
    }

    cout << "\n[INFO] Juego terminado. Puntuacion final: " << puntuacion << "\n";
    return 0;
}