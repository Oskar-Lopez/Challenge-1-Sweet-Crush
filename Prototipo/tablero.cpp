// ============================================================
// Sweet Crush - Prototipo: crear y mostrar el tablero
// ============================================================
#include <iostream>

using namespace std;

// ------------------------------------------------------------
// Estado global del tablero
// ------------------------------------------------------------
unsigned char* g_buffer = 0;   // memoria dinámica con los bits
int g_filas = 0;
int g_columnas = 0;
int g_totalPos = 0;
int g_bytesTot = 0;

// ------------------------------------------------------------
// Generador pseudoaleatorio simple
// Semilla definida por el usuario.
// ------------------------------------------------------------
unsigned int g_semilla = 1;

unsigned int siguienteAleatorio() {
    g_semilla = g_semilla * 1103515245u + 12345u;
    return (g_semilla >> 16) & 0x7FFF;
}

// ------------------------------------------------------------
// Escribe una ficha (3 bits)
// ------------------------------------------------------------
void asignarFicha(int pos, unsigned char valor) {
    valor &= 0x07;
    int bitIni  = pos * 3;
    int byteIdx = bitIni / 8;
    int offset  = bitIni % 8;

    if (offset <= 5) {
        unsigned char m = 0x07 << offset;
        g_buffer[byteIdx] = (g_buffer[byteIdx] & ~m) | (valor << offset);
    } else {
        int bitsPrimero = 8 - offset;
        int bitsSegundo = 3 - bitsPrimero;

        unsigned char m1 = ((1 << bitsPrimero) - 1) << offset;
        g_buffer[byteIdx] = (g_buffer[byteIdx] & ~m1) |
                            ((valor & ((1 << bitsPrimero) - 1)) << offset);

        unsigned char m2 = (1 << bitsSegundo) - 1;
        g_buffer[byteIdx + 1] = (g_buffer[byteIdx + 1] & ~m2) |
                                (valor >> bitsPrimero);
    }
}

// ------------------------------------------------------------
// Leer una ficha (3 bits) de la posición lógica indicada
// ------------------------------------------------------------
unsigned char obtenerFicha(int pos) {
    int bitIni  = pos * 3;
    int byteIdx = bitIni / 8;
    int offset  = bitIni % 8;

    if (offset <= 5) {
        return (g_buffer[byteIdx] >> offset) & 0x07;
    } else {
        int bitsPrimero = 8 - offset;
        int bitsSegundo = 3 - bitsPrimero;

        unsigned char p1 = (g_buffer[byteIdx] >> offset) &
                           ((1 << bitsPrimero) - 1);
        unsigned char p2 = (g_buffer[byteIdx + 1] &
                            ((1 << bitsSegundo) - 1)) << bitsPrimero;
        return p1 | p2;
    }
}

// ------------------------------------------------------------
// Crear el tablero y generar fichas pseudoaleatorias (0..5)
// ------------------------------------------------------------
void crearTablero(int filas, int columnas) {
    g_filas    = filas;
    g_columnas = columnas;
    g_totalPos = filas * columnas;
    g_bytesTot = (3 * g_totalPos + 7) / 8;

    if (g_buffer != 0) {
        delete[] g_buffer;
        g_buffer = 0;
    }

    g_buffer = new unsigned char[g_bytesTot]();
    for (int i = 0; i < g_bytesTot; i++) g_buffer[i] = 0;

    for (int i = 0; i < g_totalPos; i++) {
        asignarFicha(i, siguienteAleatorio() % 6);
    }
}

// ------------------------------------------------------------
// Mostrar el tablero en formato fichas
// ------------------------------------------------------------
void mostrarTablero() {
    cout << "\n=== TABLERO ===\n";
    for (int f = 0; f < g_filas; f++) {
        for (int c = 0; c < g_columnas; c++) {
            int pos = f * g_columnas + c;
            cout << (char)('A' + obtenerFicha(pos)) << " ";
        }
        cout << "\n";
    }
}

// ------------------------------------------------------------
// main: pide filas, columnas y semilla, luego crea y muestra
// ------------------------------------------------------------
int main() {
    int filas, columnas;

    cout << "Filas: ";    cin >> filas;
    cout << "Columnas: "; cin >> columnas;
    cout << "Semilla: ";  cin >> g_semilla;

    crearTablero(filas, columnas);
    mostrarTablero();

    if (g_buffer != 0) {
        delete[] g_buffer;
        g_buffer = 0;
    }
    return 0;
}