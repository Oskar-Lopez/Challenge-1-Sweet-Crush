// ============================================================
// Sweet Crush - Prototipo: Juego Completo con Modificación Estructural
// Estudiantes: Yudis Apreciado, Oscar Lopez
// Curso: Informática 2 C++
// ============================================================
#include <iostream>
using namespace std;

// ============================================================
// 1. GENERADOR PSEUDOALEATORIO
// ============================================================
unsigned int siguienteAleatorio(unsigned int& semilla) {
    semilla = semilla * 1103515245u + 12345u;
    return (semilla >> 16) & 0x7FFF;
}

// Genera un entero uniforme en [1, 6] con descarte de valores
// para evitar el sesgo del módulo.
int generarFichaUniforme(unsigned int& semilla) {
    const unsigned int RANGO  = 32768u;
    const unsigned int LIMITE = RANGO - (RANGO % 6u);
    unsigned int valor;
    do {
        valor = siguienteAleatorio(semilla);
    } while (valor >= LIMITE);
    return (valor % 6) + 1;
}

// ============================================================
// 2. CÁLCULO DE MEMORIA Y MANIPULACIÓN DE BITS
// ============================================================
int calcular_bytes_necesarios(int filas, int columnas) {
    int total_fichas = filas * columnas;
    int total_bits   = total_fichas * 3;
    return (total_bits + 7) / 8;
}

// Guardar una ficha (3 bits) en el buffer
void asignarFicha(unsigned char* buffer, int posicion, unsigned char valor, int bytes_totales) {
    valor = valor & 0x07;
    int bit_inicial     = posicion * 3;
    int indice_byte     = bit_inicial / 8;
    int desplazamiento  = bit_inicial % 8;

    if (indice_byte >= bytes_totales) return;

    if (desplazamiento <= 5) {
        unsigned char mascara = 0x07 << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara) | (valor << desplazamiento);
    } else {
        int bits_primer_byte = 8 - desplazamiento;
        unsigned char mascara1 = ((1 << bits_primer_byte) - 1) << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara1) |
                              ((valor & ((1 << bits_primer_byte) - 1)) << desplazamiento);

        if (indice_byte + 1 < bytes_totales) {
            int bits_segundo_byte = 3 - bits_primer_byte;
            unsigned char mascara2 = (1 << bits_segundo_byte) - 1;
            buffer[indice_byte + 1] = (buffer[indice_byte + 1] & ~mascara2) |
                                      (valor >> bits_primer_byte);
        }
    }
}

// Leer una ficha (3 bits) del buffer
unsigned char obtenerFicha(const unsigned char* buffer, int posicion, int bytes_totales) {
    int bit_inicial    = posicion * 3;
    int indice_byte    = bit_inicial / 8;
    int desplazamiento = bit_inicial % 8;

    if (indice_byte >= bytes_totales) return 0;

    if (desplazamiento <= 5) {
        return (buffer[indice_byte] >> desplazamiento) & 0x07;
    } else {
        int bits_primer_byte  = 8 - desplazamiento;
        int bits_segundo_byte = 3 - bits_primer_byte;

        unsigned char parte1 = (buffer[indice_byte] >> desplazamiento) &
                               ((1 << bits_primer_byte) - 1);

        unsigned char parte2 = 0;
        if (indice_byte + 1 < bytes_totales) {
            parte2 = (buffer[indice_byte + 1] &
                      ((1 << bits_segundo_byte) - 1)) << bits_primer_byte;
        }
        return parte1 | parte2;
    }
}

// ============================================================
// 3. VISUALIZACIÓN
// ============================================================
void mostrarTableroFichas(const unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    cout << "\n=== TABLERO (FICHAS) ===\n";
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            int posicion = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, posicion, bytes_totales);

            if (valor == 0)      cout << ".  ";
            else if (valor == 7) cout << "X  ";
            else                 cout << char('A' + valor - 1) << "  ";
        }
        cout << "\n";
    }
    cout << "========================\n";
}

// Aporte de Yudis Apreciado: mostrar la memoria en binario
void mostrarMemoriaBinaria(const unsigned char* buffer, int bytes_totales) {
    cout << "\n=== ESTADO EN MEMORIA (BINARIO) ===\n";
    cout << "Total Bytes reservados: " << bytes_totales << "\n";

    for (int i = 0; i < bytes_totales; i++) {
        cout << "Byte [" << i << "]: ";
        for (int bit = 7; bit >= 0; bit--) {
            cout << ((buffer[i] >> bit) & 1);
        }
        cout << "\n";
    }
    cout << "===================================\n";
}

// ============================================================
// 4. CREACIÓN DEL TABLERO
// ============================================================
void crearTablero(unsigned char*& buffer, int filas, int columnas,
                  int& total_posiciones, int& bytes_totales, unsigned int& semilla) {
    total_posiciones = filas * columnas;
    bytes_totales    = calcular_bytes_necesarios(filas, columnas);

    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }

    buffer = new unsigned char[bytes_totales]();

    for (int i = 0; i < total_posiciones; i++) {
        asignarFicha(buffer, i, generarFichaUniforme(semilla), bytes_totales);
    }
}

// ============================================================
// 5. LÓGICA DEL JUEGO
// ============================================================

// Detectar combinaciones de 3 o más fichas (horizontal y vertical)
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

// Eliminar las fichas marcadas
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

// Aplicar gravedad (las fichas caen al fondo de cada columna)
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

// Rellenar huecos con fichas nuevas
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
// 6. REGLA DEL 65%
// Compara la memoria necesaria contra la memoria reservada
// ANTES de la operación. Solo reduce si el uso cae por debajo
// del 65% del tamaño previo.
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

            puntuacion            += fichas_eliminadas * 10;
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

// ============================================================
// 8. MODIFICACIÓN ESTRUCTURAL
// ============================================================

// Agregar una fila en la posición indicada
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

// Eliminar una fila en la posición indicada
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

// Agregar una columna en la posición indicada
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

// Eliminar una columna en la posición indicada
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