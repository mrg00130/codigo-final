/**
* @file administrador.h
 * @brief Gestión del modo administrador de la máquina expendedora.
 * @author mrg00130
 * @date 2025-05-15 17:36:52
 */

#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include <stdbool.h>
#include "productos.h"

// Constantes
#define ADMIN_PASSWORD "123"  // Contraseña del administrador

// Prototipos de funciones del modo administrador
void menuAdministrador(Producto productos[], int *numProductos, float *pFondoMaquina, float *pGananciasDia);
void anadirProducto(Producto productos[], int *numProductos);
void eliminarProducto(Producto productos[], int *numProductos);
void modificarPrecio(Producto productos[], int numProductos);
void recargarStock(Producto productos[], int *numProductos);
void verGanancias(void);
void resetearGanancias(float *pGananciasDia);
void configurarPuertoSerial(void);
void gestionarFondoMaquina(float *pFondoMaquina);
void recargarTarjeta(void);
bool probarTarjeta(void);

#endif // ADMINISTRADOR_H