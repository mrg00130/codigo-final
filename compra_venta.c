#include "compra_venta.h"
#include "serial_protocol.h"
#include "pagos.h"
#include "productos_manejo.h"
#include "fondo_manejo.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

extern float fondoMaquina;
extern float gananciasDia;
extern char puertoCOM[10];
extern int baudRate;
extern void limpiarBuffer(void);

void espera_inicio_stm32(HANDLE hComm) {
    char buffer[256];
    DWORD bytesRead = 0;
    int intentos = 300;
    while (intentos--) {
        int i = 0;
        char c = 0;
        while (i < (int)sizeof(buffer) - 1) {
            if (!ReadFile(hComm, &c, 1, &bytesRead, NULL) || bytesRead != 1) {
#ifdef _WIN32
                Sleep(5);
#endif
                continue;
            }
            if (c == '\n' || c == '\r') break;
            buffer[i++] = c;
        }
        buffer[i] = '\0';
        if (strstr(buffer, "Esperando comando de inicio")) {
            break;
        }
    }
}

bool cobrar(float precio, HANDLE hComm) {
    float dineroIngresado = 0.0f;
    bool resultado = false;
    static int primera_vez = 1;

    while (1) {
        mostrarMenuPago(precio);
        int metodoPago = obtenerOpcionPago();
        limpiarBuffer();

        if (metodoPago == PAGO_EFECTIVO) {
            printf("Inserte dinero (puede ser menos que el precio, la máquina le dirá si es suficiente): ");
            if (scanf("%f", &dineroIngresado) != 1 || dineroIngresado <= 0) {
                printf("Cantidad inválida. Intente de nuevo.\n");
                limpiarBuffer();
                continue;
            }
            limpiarBuffer();

            if (!primera_vez) {
                DWORD bytesWritten = 0;
                WriteFile(hComm, "NEXT\r\n", 6, &bytesWritten, NULL);
                espera_inicio_stm32(hComm);
            }
            primera_vez = 0;

            DWORD bytesWritten = 0;
            WriteFile(hComm, "START\r\n", 7, &bytesWritten, NULL);

            char respuesta[128] = {0};
            serial_protocolo_automatico(hComm, fondoMaquina, precio, dineroIngresado, respuesta, sizeof(respuesta));

            if (strstr(respuesta, "dinero insuficiente")) {
                mostrarError("Dinero insuficiente. Por favor, inserte más dinero.");
                resultado = false;
            } else if (strstr(respuesta, "importe exacto")) {
                mostrarMensaje("Importe exacto. Compra aceptada.");
                resultado = true;
            } else if (strstr(respuesta, "No hay cambio disponible")) {
                mostrarError("No hay cambio disponible.");
                resultado = false;
            } else if (strstr(respuesta, "Compra posible")) {
                mostrarMensaje("Compra aceptada. ¡Gracias!");
                resultado = true;
            } else {
                mostrarError("Respuesta desconocida del sistema de cobro.");
                resultado = false;
            }

            if (resultado) {
                fondoMaquina += precio;
                gananciasDia += precio;
                float cambio = dineroIngresado - precio;
                if (cambio > 0) fondoMaquina -= cambio;
                guardarFondoMaquina(fondoMaquina);
                break;
            }

        } else if (metodoPago == PAGO_TARJETA) {
            mostrarMensaje("Procesando pago con tarjeta...");
#ifdef _WIN32
            Sleep(1000);
#endif
            mostrarMensaje("Pago realizado correctamente.");
            fondoMaquina += precio;
            gananciasDia += precio;
            guardarFondoMaquina(fondoMaquina);
            resultado = true;
            break;
        } else if (metodoPago == PAGO_CANCELAR) {
            mostrarMensaje("Operación cancelada");
            return false;
        } else {
            mostrarError("Opción no válida");
        }
    }
    return resultado;
}

void comprarProducto(Producto productos[], int numProductos, HANDLE hComm) {
    if (numProductos == 0) {
        printf("No hay productos disponibles en este momento.\n");
        return;
    }

    int opcion;
    while (1) {
        mostrarProductos(productos, numProductos);
        printf("Seleccione el número del producto a comprar (0 para cancelar): ");

        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida. Por favor, ingrese un número.\n");
            limpiarBuffer();
            continue;
        }
        limpiarBuffer();

        if (opcion == 0) {
            printf("Operación cancelada. Volviendo al menú principal.\n");
            return;
        }

        if (opcion < 1 || opcion > numProductos) {
            printf("Selección inválida. Por favor, elija un número entre 1 y %d.\n", numProductos);
            continue;
        }

        int indiceProducto = opcion - 1;

        if (productos[indiceProducto].stock <= 0) {
            printf("Lo sentimos, el producto '%s' está agotado.\n", productos[indiceProducto].nombre);
            continue;
        }

        if (cobrar(productos[indiceProducto].precio, hComm)) {
            productos[indiceProducto].stock--;
            printf("¡Compra exitosa! Disfrute su %s\n", productos[indiceProducto].nombre);
            guardarProductos(productos, numProductos);
            guardarFondoMaquina(fondoMaquina);
            return;
        } else {
            printf("La compra no pudo completarse. Por favor, intente de nuevo.\n");
            continue;
        }
    }
}