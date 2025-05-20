#ifndef COMPRA_VENTA_H
#define COMPRA_VENTA_H
#include <stdbool.h>
#include "productos.h"
#ifdef _WIN32
#include <windows.h>
#endif

void comprarProducto(Producto productos[], int numProductos, HANDLE hComm);
bool cobrar(float precio, HANDLE hComm);

#endif // COMPRA_VENTA_H