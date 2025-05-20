#include "pagos.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

void mostrarMenuPago(float precio) {
    printf("\n--- Método de Pago ---\n");
    printf("Precio a pagar: €%.2f\n", precio);
    printf("1. Efectivo (Hardware)\n");
    printf("2. Tarjeta (Simulado)\n");
    printf("0. Volver atrás\n");
    printf("Ingrese su opción: ");
}

int obtenerOpcionPago(void) {
    int opcion;
    scanf("%d", &opcion);
    return opcion;
}

void mostrarError(const char* mensaje) {
    printf("\nError: %s\n", mensaje);
#ifdef _WIN32
    Sleep(2000);
#endif
}

void mostrarMensaje(const char* mensaje) {
    printf("\n%s\n", mensaje);
}

void mostrarCambio(float cambio) {
    printf("Cambio a devolver: %.2f EUR\n", cambio);
}

// Puedes implementar aquí la lógica real si quieres guardar en disco o sumar a variables globales
void actualizarFondos(float cantidad) {
    // Implementación dummy
    (void)cantidad;
}

// Simula la inserción de dinero por el usuario
float simularInsercionDinero(float precio) {
    float total = 0.0f;
    float ingreso = 0.0f;
    int continuar = 1;

    printf("\n--- Inserción de Dinero (Simulación) ---\n");
    printf("Introduce dinero hasta alcanzar al menos el precio del producto (%.2f €)\n", precio);

    while (total < precio && continuar) {
        printf("Introduce cantidad (ej. 1, 2, 0 para finalizar): ");
        if (scanf("%f", &ingreso) != 1 || ingreso < 0) {
            printf("Entrada inválida. Se cancela la operación.\n");
            break;
        }
        if (ingreso == 0) {
            printf("Operación cancelada por el usuario.\n");
            total = 0.0f;
            break;
        }
        total += ingreso;
        printf("Total entregado: %.2f €\n", total);
        if (total < precio) {
            printf("Falta por entregar: %.2f €\n", precio - total);
        }
    }
    // Limpieza de buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    return total;
}