#include "bits.h"

// ============================================================
// 1. GENERADOR PSEUDOALEATORIO
// ============================================================
unsigned int siguienteAleatorio(unsigned int& semilla) {
    semilla = semilla * 1103515245u + 12345u;
    return (semilla >> 16) & 0x7FFF;
}

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