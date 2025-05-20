/**
 * @file productos_manejador.c
 * @brief Implementa las funciones para cargar, guardar y mostrar productos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "productos_manejo.h"
#include "productos.h"

#define NOMBRE_ARCHIVO_PRODUCTOS "productos.txt"

void cargarProductos(Producto productos[], int *numProductos) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO_PRODUCTOS, "r");
    if (!archivo) {
        printf("Archivo '%s' no encontrado. Se creará uno nuevo.\n", NOMBRE_ARCHIVO_PRODUCTOS);
        archivo = fopen(NOMBRE_ARCHIVO_PRODUCTOS, "w");
        if (!archivo) {
            perror("Error crítico: No se pudo crear el archivo de productos");
            *numProductos = 0;
            return;
        }
        fclose(archivo);
        printf("Archivo '%s' creado vacío.\n", NOMBRE_ARCHIVO_PRODUCTOS);
        *numProductos = 0;
        return;
    }

    *numProductos = 0;
    while (*numProductos < MAX_PRODUCTOS &&
           fscanf(archivo, "%49[^,],%f,%d\n",
                  productos[*numProductos].nombre,
                  &productos[*numProductos].precio,
                  &productos[*numProductos].stock) == 3)
    {
        (*numProductos)++;
    }

    if (!feof(archivo) && *numProductos >= MAX_PRODUCTOS) {
         printf("Advertencia: Se alcanzó el límite máximo de %d productos. No se cargaron más.\n", MAX_PRODUCTOS);
    } else if (ferror(archivo)) {
        perror("Error durante la lectura del archivo de productos");
    }

    fclose(archivo);
    printf("Cargados %d productos desde '%s'.\n", *numProductos, NOMBRE_ARCHIVO_PRODUCTOS);
}

void guardarProductos(Producto productos[], int numProductos) {
    FILE *archivo = fopen(NOMBRE_ARCHIVO_PRODUCTOS, "w");
    if (!archivo) {
        perror("Error al abrir el archivo para guardar productos");
        return;
    }

    for (int i = 0; i < numProductos; i++) {
        fprintf(archivo, "%s,%.2f,%d\n",
                productos[i].nombre,
                productos[i].precio,
                productos[i].stock);
    }
    fclose(archivo);
}

void mostrarProductos(Producto productos[], int numProductos) {
    printf("\n--- Productos Disponibles ---\n");
    if (numProductos == 0) {
        printf("No hay productos disponibles en este momento.\n");
        printf("-----------------------------\n");
        return;
    }

    for (int i = 0; i < numProductos; i++) {
        printf("%d. %-20s - €%5.2f (Stock: %d)\n",
               i + 1,
               productos[i].nombre,
               productos[i].precio,
               productos[i].stock > 0 ? productos[i].stock : 0);
    }
    printf("-----------------------------\n");
    printf("0. Cancelar / Volver atrás\n");
    printf("-----------------------------\n");
}

void mostrarProductosConStock(Producto productos[], int numProductos) {
    if (numProductos > 0) {
        printf("--- Stock de Productos ---\n");
        for (int i = 0; i < numProductos; i++) {
            printf("%d. %s - Stock: %d\n", i + 1, productos[i].nombre, productos[i].stock);
        }
        printf("---------------------------\n");
    } else {
        printf("No hay productos en el sistema.\n");
    }
}