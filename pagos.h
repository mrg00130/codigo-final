#ifndef VENDING_PAGOS_H
#define VENDING_PAGOS_H

// Tipos de pago
typedef enum {
    PAGO_EFECTIVO = 1,
    PAGO_TARJETA = 2,
    PAGO_CANCELAR = 0
} TipoPago;

// Funciones de interfaz de usuario
void mostrarMenuPago(float precio);
int obtenerOpcionPago(void);
void mostrarError(const char* mensaje);
void mostrarMensaje(const char* mensaje);
void mostrarCambio(float cambio);
void actualizarFondos(float cantidad);

// Nueva: simula la inserción de dinero y devuelve el importe total introducido
float simularInsercionDinero(float precio);

#endif // VENDING_PAGOS_H