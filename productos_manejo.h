/**
* @file productos_manejador.h
 * @brief Declara las funciones para cargar, guardar y mostrar productos.
 */
//
// Created by m4nol on 05/04/2025. // Comentario opcional del IDE
//

#ifndef PRODUCTOS_MANEJADOR_H
#define PRODUCTOS_MANEJADOR_H

#include "productos.h" // Necesario para la definición de la estructura Producto y MAX_PRODUCTOS

/* --- Prototipos de Funciones para la Gestión de Productos --- */

/**
 * @brief Carga la lista de productos desde el archivo predeterminado.
 * @param productos Array donde se cargarán los productos.
 * @param numProductos Puntero al contador de productos (se actualizará).
 */
void cargarProductos(Producto productos[], int *numProductos);

/**
 * @brief Guarda la lista actual de productos en el archivo predeterminado.
 * @param productos Array de productos a guardar.
 * @param numProductos Número de productos en el array.
 */
void guardarProductos(Producto productos[], int numProductos);

/**
 * @brief Muestra la lista de productos disponibles al usuario.
 * @param productos Array de productos a mostrar.
 * @param numProductos Número de productos en el array.
 */
void mostrarProductos(Producto productos[], int numProductos);
void mostrarProductosConStock(Producto productos[], int numProductos);


#endif // PRODUCTOS_MANEJADOR_H