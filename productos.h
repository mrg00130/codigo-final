/**
* @file Productos.h
 * @brief Define la estructura para representar un producto.
 */
#ifndef PRODUCTOS_H
#define PRODUCTOS_H

// Define el tamaño máximo para el nombre del producto (incluyendo el carácter nulo final)
#define MAX_NOMBRE 50
// Define el número máximo de productos que puede almacenar la máquina
#define MAX_PRODUCTOS 100

// Estructura para representar un producto con nombre, precio y stock
typedef struct {
    char nombre[MAX_NOMBRE]; // Nombre del producto
    float precio;           // Precio del producto
    int stock;              // Cantidad disponible en stock
} Producto;

#endif // PRODUCTOS_H