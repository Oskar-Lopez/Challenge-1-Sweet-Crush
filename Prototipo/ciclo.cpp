// ============================================================
// Sweet Crush - Prototipo: Juego con tablero y fichas
// Estudiantes: Yudis Apreciado, Oscar Lopez
// Curso: Informática 2 C++
// ============================================================
#include <iostream>
using namespace std;

// ------------------------------------------------------------
// Función para generar números pseudoaleatorios
// Usa una fórmula matemática llamada LCG (Generador Lineal Congruencial)
// Cada vez que la llamamos, nos da un número "aleatorio" basado en la semilla
// ------------------------------------------------------------
unsigned int siguienteAleatorio(unsigned int& semilla) {
    semilla = semilla * 1103515245u + 12345u;
    return (semilla >> 16) & 0x7FFF;
}

// ------------------------------------------------------------
// Aporte de Yudis Apreciado: Calcula cuántos bytes necesitamos reservar
// Cada ficha usa 3 bits, así que multiplicamos por 3
// El 7 ayuda a redondear al byte completo más cercano
// Agregamos 2 bytes extra de seguridad para evitar errores de memoria
// ------------------------------------------------------------
int calcular_bytes_necesarios(int filas, int columnas) {
    int total_fichas = filas * columnas;
    int total_bits = total_fichas * 3;
    // Sumamos 2 bytes extra para que no haya problemas cuando
    // una ficha se divide entre dos bytes al final del buffer
    return (total_bits + 7) / 8 + 2;
}

// ------------------------------------------------------------
// Función para guardar una ficha en el buffer (3 bits)
// Parámetros:
//   - buffer: la memoria donde guardamos todo
//   - posicion: número de ficha (0, 1, 2, ...)
//   - valor: qué ficha es (1-6, 0=vacío, 7=marcado)
//   - bytes_totales: tamaño del buffer para no salirnos
// ------------------------------------------------------------
void asignarFicha(unsigned char* buffer, int posicion, unsigned char valor, int bytes_totales) {
    // Limitamos el valor a 3 bits (0-7)
    valor = valor & 0x07;

    // Calculamos en qué bit empieza esta ficha
    int bit_inicial = posicion * 3;

    // Calculamos en qué byte está ese bit
    int indice_byte = bit_inicial / 8;

    // Calculamos el desplazamiento dentro del byte
    int desplazamiento = bit_inicial % 8;

    // Verificamos que no nos salgamos del buffer
    if (indice_byte >= bytes_totales) {
        return;  // Salimos si la posición es inválida
    }

    // CASO 1: La ficha cabe completa en un solo byte
    if (desplazamiento <= 5) {
        // Creamos una máscara para limpiar los 3 bits
        unsigned char mascara = 0x07 << desplazamiento;
        // Limpiamos y escribimos el nuevo valor
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara) | (valor << desplazamiento);
    }
    // CASO 2: La ficha se divide entre dos bytes
    else {
        // Cuántos bits van en el primer byte
        int bits_primer_byte = 8 - desplazamiento;

        // Limpiamos y escribimos en el primer byte
        unsigned char mascara1 = ((1 << bits_primer_byte) - 1) << desplazamiento;
        buffer[indice_byte] = (buffer[indice_byte] & ~mascara1) |
                              ((valor & ((1 << bits_primer_byte) - 1)) << desplazamiento);

        // Verificamos que exista el segundo byte antes de escribir
        if (indice_byte + 1 < bytes_totales) {
            // Cuántos bits van en el segundo byte
            int bits_segundo_byte = 3 - bits_primer_byte;

            // Escribimos los bits restantes en el siguiente byte
            unsigned char mascara2 = (1 << bits_segundo_byte) - 1;
            buffer[indice_byte + 1] = (buffer[indice_byte + 1] & ~mascara2) |
                                      (valor >> bits_primer_byte);
        }
    }
}

// ------------------------------------------------------------
// Función para leer una ficha del buffer (3 bits)
// Retorna el valor de la ficha en la posición indicada
// ------------------------------------------------------------
unsigned char obtenerFicha(const unsigned char* buffer, int posicion, int bytes_totales) {
    int bit_inicial = posicion * 3;
    int indice_byte = bit_inicial / 8;
    int desplazamiento = bit_inicial % 8;

    // Verificamos que no nos salgamos del buffer
    if (indice_byte >= bytes_totales) {
        return 0; // Retornamos 0 si la posición es inválida
    }

    // CASO 1: La ficha está en un solo byte
    if (desplazamiento <= 5) {
        return (buffer[indice_byte] >> desplazamiento) & 0x07;
    }
    // CASO 2: La ficha está dividida en dos bytes
    else {
        int bits_primer_byte = 8 - desplazamiento;
        int bits_segundo_byte = 3 - bits_primer_byte;

        // Leemos la primera parte
        unsigned char parte1 = (buffer[indice_byte] >> desplazamiento) &
                               ((1 << bits_primer_byte) - 1);

        // Leemos la segunda parte (si existe)
        unsigned char parte2 = 0;
        if (indice_byte + 1 < bytes_totales) {
            parte2 = (buffer[indice_byte + 1] &
                      ((1 << bits_segundo_byte) - 1)) << bits_primer_byte;
        }

        // Combinamos las dos partes
        return parte1 | parte2;
    }
}

// ------------------------------------------------------------
// Función para crear el tablero y llenarlo con fichas aleatorias
// Usa memoria dinámica (new) para guardar el buffer
// ------------------------------------------------------------
void crearTablero(unsigned char*& buffer, int filas, int columnas, int& total_posiciones, int& bytes_totales, unsigned int& semilla) {
    total_posiciones = filas * columnas;
    bytes_totales = calcular_bytes_necesarios(filas, columnas);

    // Si ya existe un buffer, lo liberamos primero
    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }

    // Reservamos memoria y la inicializamos en 0
    buffer = new unsigned char[bytes_totales]();

    // Llenamos el tablero con fichas del 1 al 6
    for (int i = 0; i < total_posiciones; i++) {
        int ficha = (siguienteAleatorio(semilla) % 6) + 1;
        asignarFicha(buffer, i, ficha, bytes_totales);
    }
}

// ------------------------------------------------------------
// Muestra el tablero en pantalla con letras (A-F)
// . = vacío, X = marcado, A-F = fichas
// ------------------------------------------------------------
void mostrarTableroFichas(const unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    cout << "\n=== TABLERO (FICHAS) ===\n";
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            int posicion = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, posicion, bytes_totales);

            if (valor == 0) {
                cout << ".  ";       // Vacío
            } else if (valor == 7) {
                cout << "X  ";       // Marcado
            } else {
                // Convertimos 1-6 a letras A-F
                cout << char('A' + valor - 1) << "  ";
            }
        }
        cout << "\n";
    }
    cout << "========================\n";
}

//-----------------------------------------
// Aporte de Yudis Apreciado: Muestra la memoria en binario
// Esto nos ayuda a ver cómo se guardan realmente los bits
//-----------------------------------------
void mostrarMemoriaBinaria(const unsigned char* buffer, int bytes_totales) {
    cout << "\n=== ESTADO EN MEMORIA (BINARIO) ===\n";
    cout << "Total Bytes reservados: " << bytes_totales << "\n";

    for(int i = 0; i < bytes_totales; i++) {
        cout << "Byte [" << i << "]: ";
        // Mostramos cada bit del byte (de 7 a 0)
        for(int bit = 7; bit >= 0; bit--) {
            cout << ((buffer[i] >> bit) & 1);
        }
        cout << "\n";
    }
    cout << "===================================\n";
}

// ------------------------------------------------------------
// Detecta combinaciones de 3 o más fichas iguales
// Marca las posiciones en el arreglo 'marcados'
// Busca en horizontal y vertical
// ------------------------------------------------------------
void detectarCombinaciones(const unsigned char* buffer, int filas, int columnas,
                           int total_posiciones, bool* marcados, int bytes_totales) {
    // Inicializamos todos en false (sin marcar)
    for (int i = 0; i < total_posiciones; i++) {
        marcados[i] = false;
    }

    // BUSCAR EN HORIZONTAL (filas)
    for (int fila = 0; fila < filas; fila++) {
        int columna = 0;
        while (columna < columnas - 2) {
            int pos = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, pos, bytes_totales);

            // Saltamos si está vacío o ya marcado
            if (valor == 0 || valor == 7) {
                columna++;
                continue;
            }

            // Contamos cuántas fichas iguales hay seguidas
            int longitud = 1;
            while (columna + longitud < columnas) {
                int pos_sig = fila * columnas + (columna + longitud);
                if (obtenerFicha(buffer, pos_sig, bytes_totales) == valor) {
                    longitud++;
                } else {
                    break;
                }
            }

            // Si hay 3 o más, las marcamos todas
            if (longitud >= 3) {
                for (int k = 0; k < longitud; k++) {
                    marcados[fila * columnas + (columna + k)] = true;
                }
            }
            columna = columna + longitud;
        }
    }

    // BUSCAR EN VERTICAL (columnas)
    for (int columna = 0; columna < columnas; columna++) {
        int fila = 0;
        while (fila < filas - 2) {
            int pos = fila * columnas + columna;
            unsigned char valor = obtenerFicha(buffer, pos, bytes_totales);

            if (valor == 0 || valor == 7) {
                fila++;
                continue;
            }

            int longitud = 1;
            while (fila + longitud < filas) {
                int pos_sig = (fila + longitud) * columnas + columna;
                if (obtenerFicha(buffer, pos_sig, bytes_totales) == valor) {
                    longitud++;
                } else {
                    break;
                }
            }

            if (longitud >= 3) {
                for (int k = 0; k < longitud; k++) {
                    marcados[(fila + k) * columnas + columna] = true;
                }
            }
            fila = fila + longitud;
        }
    }
}

// ------------------------------------------------------------
// Elimina las fichas marcadas poniéndolas en 0 (vacío)
// ------------------------------------------------------------
void eliminarFichasMarcadas(unsigned char* buffer, int total_posiciones, bool* marcados, int bytes_totales) {
    for (int i = 0; i < total_posiciones; i++) {
        if (marcados[i]) {
            asignarFicha(buffer, i, 0, bytes_totales);
        }
    }
}

// ------------------------------------------------------------
// Hace caer las fichas hacia abajo (gravedad)
// Las fichas vacías quedan arriba
// Recorremos por filas y columnas, no necesitamos total_posiciones
// ------------------------------------------------------------
void aplicarGravedad(unsigned char* buffer, int filas, int columnas, int bytes_totales) {
    // Recorremos cada columna
    for (int c = 0; c < columnas; c++) {
        int destino = filas - 1;  // Empezamos desde abajo

        // Recorremos de abajo hacia arriba
        for (int f = filas - 1; f >= 0; f--) {
            int pos_lectura = f * columnas + c;
            unsigned char valor = obtenerFicha(buffer, pos_lectura, bytes_totales);

            // Si hay una ficha (no está vacía), la bajamos
            if (valor != 0) {
                int pos_destino = destino * columnas + c;
                asignarFicha(buffer, pos_destino, valor, bytes_totales);

                // Si la movimos, limpiamos la posición original
                if (pos_lectura != pos_destino) {
                    asignarFicha(buffer, pos_lectura, 0, bytes_totales);
                }
                destino--;  // Bajamos el destino para la siguiente
            }
        }
    }
}

// ------------------------------------------------------------
// Rellena los espacios vacíos con fichas nuevas aleatorias
// Recorremos por filas y columnas, no necesitamos total_posiciones
// ------------------------------------------------------------
void rellenarHuecos(unsigned char* buffer, int filas, int columnas, unsigned int& semilla, int bytes_totales) {
    for (int c = 0; c < columnas; c++) {
        for (int f = 0; f < filas; f++) {
            int pos = f * columnas + c;
            unsigned char valor = obtenerFicha(buffer, pos, bytes_totales);

            // Si está vacío (0), ponemos una ficha nueva
            if (valor == 0) {
                int ficha_nueva = (siguienteAleatorio(semilla) % 6) + 1;
                asignarFicha(buffer, pos, ficha_nueva, bytes_totales);
            }
        }
    }
}

// ------------------------------------------------------------
// Función principal - Aquí empieza el programa
// ------------------------------------------------------------
int main() {
    int filas, columnas;
    unsigned int semilla = 1;
    int total_posiciones = 0;
    int bytes_totales = 0;
    unsigned char* tablero = nullptr;

    cout << "=== SWEET CRUSH - PROTOTIPO ===\n";
    cout << "Filas: ";
    cin >> filas;
    cout << "Columnas: ";
    cin >> columnas;
    cout << "Semilla: ";
    cin >> semilla;

    // Creamos el tablero con las dimensiones y semilla dadas
    crearTablero(tablero, filas, columnas, total_posiciones, bytes_totales, semilla);

    cout << "\n[1] TABLERO INICIAL:\n";
    mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

    // Mostramos la memoria en binario (aporte de Yudis)
    mostrarMemoriaBinaria(tablero, bytes_totales);

    // Creamos un arreglo para marcar las combinaciones
    bool* marcados = new bool[total_posiciones];

    // Detectamos combinaciones
    detectarCombinaciones(tablero, filas, columnas, total_posiciones, marcados, bytes_totales);

    // Contamos cuántas fichas se marcaron
    int total_marcadas = 0;
    for (int i = 0; i < total_posiciones; i++) {
        if (marcados[i]) {
            total_marcadas++;
        }
    }

    if (total_marcadas > 0) {
        cout << "\n[2] FICHAS DETECTADAS: " << total_marcadas << "\n";

        // Paso 1: Eliminar fichas
        eliminarFichasMarcadas(tablero, total_posiciones, marcados, bytes_totales);
        cout << "\n[3] DESPUES DE ELIMINAR (huecos = '.'):\n";
        mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

        // Paso 2: Aplicar gravedad
        aplicarGravedad(tablero, filas, columnas, bytes_totales);
        cout << "\n[4] DESPUES DE GRAVEDAD (fichas cayeron):\n";
        mostrarTableroFichas(tablero, filas, columnas, bytes_totales);

        // Paso 3: Rellenar con fichas nuevas
        rellenarHuecos(tablero, filas, columnas, semilla, bytes_totales);
        cout << "\n[5] DESPUES DE RELLENAR (nuevas fichas arriba):\n";
        mostrarTableroFichas(tablero, filas, columnas, bytes_totales);
    } else {
        cout << "\n[INFO] No se encontraron combinaciones de 3+ fichas.\n";
        cout << "       Intenta con otra semilla o tablero más grande.\n";
    }

    // Liberamos la memoria que reservamos
    delete[] marcados;
    if (tablero != nullptr) {
        delete[] tablero;
        tablero = nullptr;
    }

    cout << "\n[INFO] Fin del programa.\n";
    return 0;
}