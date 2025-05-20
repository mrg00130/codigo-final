/**
 * @file main.c
 * @brief Programa principal de la máquina expendedora.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "administrador.h"
#include "productos.h"
#include "productos_manejo.h"
#include "menu_flechas.h"
#include "compra_venta.h"
#include "serial_protocol.h"
#include "fondo_manejo.h"
#include "pagos.h"

// Constantes
#define NUM_MENU_OPTIONS 3

// Variables globales
float fondoMaquina = 0.0;
float gananciasDia = 0.0;
char puertoCOM[10] = "COM3";
int baudRate = 9600;
HANDLE hComm = INVALID_HANDLE_VALUE;

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Inicializa la configuración regional y la consola
static void inicializarSistema(void) {
    setlocale(LC_ALL, "es_es.UTF-8");
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

// Mapea el índice del menú a la opción lógica
static int mapearIndiceAOpcion(int menu_selection_index) {
    switch (menu_selection_index) {
        case 0: return 1; // Comprar
        case 1: return 2; // Administrador
        case 2: return 0; // Salir
        default: return -1;
    }
}

// Lógica para manejar la opción seleccionada
static bool manejarOpcionMenu(int opcion, Producto productos[], int* numProductos) {
    switch (opcion) {
        case 1: // Comprar producto
            comprarProducto(productos, *numProductos, hComm);
            break;

        case 2: // Modo Administrador
            menuAdministrador(productos, numProductos, &fondoMaquina, &gananciasDia);
            break;

        case 0: // Salir
            printf("Guardando estado final...\n");
            guardarProductos(productos, *numProductos);
            printf("Gracias por usar la máquina expendedora. ¡Hasta luego!\n");
            return false;

        default:
            printf("Error interno en el manejo de opciones.\n");
#ifdef _WIN32
            Sleep(2000);
#endif
    }

    if (opcion != 0) {
        printf("\nPresione Enter para continuar...");
        limpiarBuffer();
#ifdef _WIN32
        system("cls");
#endif
    }

    return true;
}

int main(void) {
    inicializarSistema();
    fondoMaquina = cargarFondoMaquina();

    Producto productos[MAX_PRODUCTOS];
    int numProductos = 0;

    cargarProductos(productos, &numProductos);

    if (numProductos == 0) {
        printf("No se encontraron productos. Puede añadir productos desde el modo administrador.\n");
        printf("Presione Enter para continuar...\n");
        limpiarBuffer();
    }

    // Abre el puerto serie solo una vez al principio
    hComm = serial_abrir_puerto(puertoCOM, baudRate);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("No se pudo abrir el puerto serie\n");
        return 1;
    }

    int opcion;
    bool continuar = true;

    do {
        int menu_selection_index = manejarNavegacionMenu(fondoMaquina, gananciasDia, NUM_MENU_OPTIONS);
        opcion = mapearIndiceAOpcion(menu_selection_index);

        if (opcion != -1) {
#ifdef _WIN32
            system("cls");
#endif
            continuar = manejarOpcionMenu(opcion, productos, &numProductos);
        } else {
            printf("Error: Selección de menú no reconocida.\n");
#ifdef _WIN32
            Sleep(1500);
            system("cls");
#endif
        }

    } while (continuar);

    // Cierra el puerto serie antes de salir
    if (hComm != INVALID_HANDLE_VALUE) {
        serial_cerrar_puerto();
        hComm = INVALID_HANDLE_VALUE;
    }

    return 0;
}