#include <stdio.h>
#include <stdlib.h>

#define NOMBRE_ARCHIVO_FONDO "fondo.txt"

void guardarFondoMaquina(float fondo) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO_FONDO, "w");
    if (archivo) {
        fprintf(archivo, "%.2f\n", fondo);
        fclose(archivo);
    } else {
        perror("No se pudo guardar el fondo de la máquina");
    }
}

float cargarFondoMaquina(void) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO_FONDO, "r");
    float fondo = 100.0f; // valor por defecto si no existe
    if (archivo) {
        if (fscanf(archivo, "%f", &fondo) != 1) {
            fondo = 100.0f;
        }
        fclose(archivo);
    }
    return fondo;
}