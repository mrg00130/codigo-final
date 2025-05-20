/**
 * @file administrador.c
 * @brief Implementa las funciones para el modo administrador de la máquina expendedora.
 * @author mrg00130
 * @date 2025-05-15 17:32:48
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "administrador.h"
#include "productos_manejo.h"
#include "productos.h"
#include "serial_protocol.h"
#include "pagos.h"
#include "fondo_manejo.h"

// Variables globales (definidas en main.c)
extern HANDLE hComm; // Esto está en main.c
extern float fondoMaquina;
extern float gananciasDia;
extern char puertoCOM[10];
extern int baudRate;
extern void limpiarBuffer(void);

// Funciones existentes permanecen igual hasta la nueva función gestionarFondoMaquina


/**
 * @brief Gestiona el fondo de la máquina expendedora
 *
 * Permite añadir o retirar dinero del fondo de la máquina.
 * Mantiene un registro de las operaciones realizadas.
 *
 * @param pFondoMaquina Puntero al fondo actual de la máquina
 */


void anadirProducto(Producto productos[], int *numProductos) {
    if (*numProductos >= MAX_PRODUCTOS) {
        printf("Error: No se pueden añadir más productos. Límite alcanzado.\n");
        return;
    }

    printf("\n=== Añadir Nuevo Producto ===\n");

    Producto nuevo;
    printf("Nombre del producto (máx %d caracteres): ", MAX_NOMBRE - 1);
    limpiarBuffer();
    if (fgets(nuevo.nombre, MAX_NOMBRE, stdin) != NULL) {
        nuevo.nombre[strcspn(nuevo.nombre, "\n")] = 0;
    }

    printf("Precio (€): ");
    if (scanf("%f", &nuevo.precio) != 1 || nuevo.precio <= 0) {
        printf("Error: Precio inválido.\n");
        limpiarBuffer();
        return;
    }

    printf("Stock inicial: ");
    if (scanf("%d", &nuevo.stock) != 1 || nuevo.stock < 0) {
        printf("Error: Stock inválido.\n");
        limpiarBuffer();
        return;
    }

    productos[*numProductos] = nuevo;
    (*numProductos)++;

    // ¡GUARDA después de añadir!
    guardarProductos(productos, *numProductos);

    printf("\nProducto añadido con éxito y guardado.\n");
}

/**
 * @brief Elimina un producto del sistema
 */
void eliminarProducto(Producto productos[], int *numProductos) {
    if (*numProductos <= 0) {
        printf("No hay productos para eliminar.\n");
        return;
    }

    mostrarProductos(productos, *numProductos);
    printf("\nSeleccione el número del producto a eliminar (0 para cancelar): ");

    int opcion;
    if (scanf("%d", &opcion) != 1 || opcion < 0 || opcion > *numProductos) {
        printf("Opción inválida.\n");
        limpiarBuffer();
        return;
    }

    if (opcion == 0) {
        printf("Operación cancelada.\n");
        return;
    }

    // Desplazar productos
    for (int i = opcion - 1; i < *numProductos - 1; i++) {
        productos[i] = productos[i + 1];
    }
    (*numProductos)--;

        guardarProductos(productos, *numProductos);

    printf("Producto eliminado con éxito.\n");
}

/**
 * @brief Modifica el precio de un producto
 */
void modificarProducto(Producto productos[], int numProductos) {
    if (numProductos <= 0) {
        printf("No hay productos para modificar.\n");
        return;
    }

    mostrarProductos(productos, numProductos);
    printf("\nSeleccione el número del producto a modificar (0 para cancelar): ");
    int opcion;
    if (scanf("%d", &opcion) != 1 || opcion < 0 || opcion > numProductos) {
        printf("Opción inválida.\n");
        limpiarBuffer();
        return;
    }
    if (opcion == 0) {
        printf("Operación cancelada.\n");
        return;
    }
    Producto *p = &productos[opcion-1];

    printf("¿Qué desea modificar?\n");
    printf("1. Nombre (actual: %s)\n", p->nombre);
    printf("2. Precio (actual: €%.2f)\n", p->precio);
    printf("0. Cancelar\n");
    printf("Seleccione una opción: ");
    int subopcion;
    if (scanf("%d", &subopcion) != 1) {
        printf("Opción inválida.\n");
        limpiarBuffer();
        return;
    }
    limpiarBuffer();

    switch(subopcion) {
        case 1:
            printf("Nuevo nombre (máx %d caracteres): ", MAX_NOMBRE-1);
        if (fgets(p->nombre, MAX_NOMBRE, stdin) != NULL) {
            p->nombre[strcspn(p->nombre, "\n")] = 0;
            printf("Nombre actualizado con éxito.\n");
        }
        break;
        case 2:
            printf("Nuevo precio (€): ");
        float nuevoPrecio;
        if (scanf("%f", &nuevoPrecio) == 1 && nuevoPrecio > 0) {
            p->precio = nuevoPrecio;
            printf("Precio actualizado con éxito.\n");
        } else {
            printf("Precio inválido.\n");
        }
        limpiarBuffer();
        break;
        case 0:
            printf("Cancelando modificación.\n");
        break;
        default:
            printf("Opción inválida.\n");
    }
    guardarProductos(productos, numProductos);
}
void modificarPrecio(Producto productos[], int numProductos) {
    if (numProductos <= 0) {
        printf("No hay productos para modificar.\n");
        return;
    }

    mostrarProductos(productos, numProductos);
    printf("\nSeleccione el número del producto a modificar (0 para cancelar): ");

    int opcion;
    if (scanf("%d", &opcion) != 1 || opcion < 0 || opcion > numProductos) {
        printf("Opción inválida.\n");
        limpiarBuffer();
        return;
    }

    if (opcion == 0) {
        printf("Operación cancelada.\n");
        return;
    }

    printf("Precio actual: €%.2f\n", productos[opcion-1].precio);
    printf("Nuevo precio (€): ");

    float nuevoPrecio;
    if (scanf("%f", &nuevoPrecio) != 1 || nuevoPrecio <= 0) {
        printf("Precio inválido.\n");
        limpiarBuffer();
        return;
    }

    productos[opcion-1].precio = nuevoPrecio;
    guardarProductos(productos, numProductos);
    printf("Precio actualizado con éxito.\n");
}

/**
 * @brief Recarga el stock de un producto
 */
void recargarStock(Producto productos[], int *numProductos) {
    if (*numProductos <= 0) {
        printf("No hay productos para recargar.\n");
        return;
    }

    mostrarProductos(productos, *numProductos);
    printf("\nSeleccione el número del producto a recargar (0 para cancelar): ");

    int opcion;
    if (scanf("%d", &opcion) != 1 || opcion < 0 || opcion > *numProductos) {
        printf("Opción inválida.\n");
        limpiarBuffer();
        return;
    }

    if (opcion == 0) {
        printf("Operación cancelada.\n");
        return;
    }

    printf("Stock actual: %d\n", productos[opcion-1].stock);
    printf("Cantidad a añadir: ");

    int cantidad;
    if (scanf("%d", &cantidad) != 1 || cantidad < 0) {
        printf("Cantidad inválida.\n");
        limpiarBuffer();
        return;
    }

    productos[opcion-1].stock += cantidad;
        guardarProductos(productos, *numProductos);
    printf("Stock actualizado con éxito.\n");
}

/**
 * @brief Muestra las ganancias actuales
 */
void verGanancias(void) {
    printf("\n=== Ganancias ===\n");
    printf("Ganancias del día: €%.2f\n", gananciasDia);
    printf("Fondo en máquina: €%.2f\n", fondoMaquina);
}

/**
 * @brief Resetea las ganancias del día
 */
void resetearGanancias(float *pGananciasDia) {
    printf("¿Está seguro de que desea resetear las ganancias? (1: Sí, 0: No): ");
    int confirmar;
    if (scanf("%d", &confirmar) == 1 && confirmar == 1) {
        *pGananciasDia = 0.0f;
        printf("Ganancias reseteadas.\n");
    } else {
        printf("Operación cancelada.\n");
    }
    limpiarBuffer();
}

/**
 * @brief Configura el puerto serial
 */
void configurarPuertoSerial(void) {
    extern HANDLE hComm; // Añade esto al principio de la función
    printf("\n=== Configuración Puerto Serial ===\n");
    printf("Puerto actual: %s\n", puertoCOM);
    printf("Baudios actual: %d\n", baudRate);

    printf("\n1. Cambiar puerto\n");
    printf("2. Cambiar baudios\n");
    printf("0. Volver\n");

    int opcion;
    printf("Seleccione una opción: ");
    if (scanf("%d", &opcion) != 1) {
        printf("Opción inválida.\n");
        limpiarBuffer();
        return;
    }
    limpiarBuffer();

    int actualizar = 0; // Bandera para saber si hay que reabrir el puerto

    switch (opcion) {
        case 1:
            printf("Nuevo puerto (ej. COM1): ");
        if (fgets(puertoCOM, sizeof(puertoCOM), stdin) != NULL) {
            puertoCOM[strcspn(puertoCOM, "\n")] = 0;
            printf("Puerto actualizado.\n");
            actualizar = 1;
        }
        break;

        case 2:
            printf("Nuevos baudios (9600, 19200, 38400, 57600, 115200): ");
        if (scanf("%d", &baudRate) == 1) {
            printf("Baudios actualizados.\n");
            actualizar = 1;
        }
        limpiarBuffer();
        break;

        case 0:
            printf("Volviendo...\n");
        break;

        default:
            printf("Opción inválida.\n");
    }

    // Si hubo cambio, cierra y vuelve a abrir el puerto serie
    if (actualizar) {
        if (hComm != INVALID_HANDLE_VALUE) {
            serial_cerrar_puerto();
            hComm = INVALID_HANDLE_VALUE;
        }
        hComm = serial_abrir_puerto(puertoCOM, baudRate);
        if (hComm == INVALID_HANDLE_VALUE) {
            printf("¡Error al abrir el nuevo puerto!\n");
        } else {
            printf("Puerto reconfigurado correctamente.\n");
        }
    }
}
void gestionarFondoMaquina(float *pFondoMaquina) {
    int opcion;
    float cantidad;

    do {
        printf("\n=== Gestión de Fondo de Máquina ===\n");
        printf("Fondo actual: €%.2f\n", *pFondoMaquina);
        printf("1. Añadir fondo\n");
        printf("2. Retirar fondo\n");
        printf("0. Volver\n");
        printf("================================\n");
        printf("Seleccione una opción: ");

        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida.\n");
            limpiarBuffer();
            continue;
        }
        limpiarBuffer();

        switch (opcion) {
            case 1:
                printf("Ingrese la cantidad a añadir: €");
                if (scanf("%f", &cantidad) == 1 && cantidad > 0) {
                    *pFondoMaquina += cantidad;
                    guardarFondoMaquina(*pFondoMaquina); // Guarda el fondo actualizado
                    printf("Fondo actualizado exitosamente.\n");
                    printf("Nuevo fondo: €%.2f\n", *pFondoMaquina);
                } else {
                    printf("Error: Cantidad inválida.\n");
                }
                limpiarBuffer();
                break;

            case 2:
                printf("Ingrese la cantidad a retirar: €");
                if (scanf("%f", &cantidad) == 1 && cantidad > 0) {
                    if (cantidad <= *pFondoMaquina) {
                        *pFondoMaquina -= cantidad;
                        guardarFondoMaquina(*pFondoMaquina); // Guarda el fondo actualizado
                        printf("Retiro exitoso.\n");
                        printf("Nuevo fondo: €%.2f\n", *pFondoMaquina);
                    } else {
                        printf("Error: Fondo insuficiente para retirar esa cantidad.\n");
                    }
                } else {
                    printf("Error: Cantidad inválida.\n");
                }
                limpiarBuffer();
                break;

            case 0:
                printf("Volviendo al menú principal...\n");
                break;

            default:
                printf("Opción inválida.\n");
        }

    } while (opcion != 0);
}


/**
 * @brief Simula la recarga de una tarjeta de pago
 *
 * Permite recargar una tarjeta con una cantidad específica.
 * Valida el número de tarjeta y la cantidad a recargar.
 */

// En la función menuAdministrador, añadir las nuevas opciones:
void menuAdministrador(Producto productos[], int *numProductos, float *pFondoMaquina, float *pGananciasDia) {
    char password[50];
    int opcionAdmin;  // Declaramos la variable aquí

    printf("--- Acceso Administrador ---\n");
    printf("Ingrese la contraseña (0 para cancelar): ");

    limpiarBuffer();
    if (fgets(password, sizeof(password), stdin) != NULL) {
        password[strcspn(password, "\n")] = 0;

        if (strcmp(password, "0") == 0) {
            printf("Acceso cancelado.\n");
            return;
        }

        if (strcmp(password, ADMIN_PASSWORD) != 0) {
            printf("Contraseña incorrecta. Acceso denegado.\n");
#ifdef _WIN32
            Sleep(1500);
#endif
            return;
        }

        printf("Acceso concedido.\n");
#ifdef _WIN32
        Sleep(1000);
        system("cls");
#endif

        do {
            printf("\n-----------------------------------------\n");
            printf("           ~ MENÚ ADMINISTRADOR ~         \n");
            printf("-----------------------------------------\n");
            printf("  Productos actuales: %d/%d\n", *numProductos, MAX_PRODUCTOS);
            printf("  Configuración Serial: Puerto %s, Baudios %d\n", puertoCOM, baudRate);
            printf("  Fondo máquina: €%.2f | Ganancias día: €%.2f\n", *pFondoMaquina, *pGananciasDia);
            printf("-----------------------------------------\n");
            printf("  1. Añadir producto\n");
            printf("  2. Eliminar producto\n");
            printf("  3. Modificar producto (nombre/precio)\n");
            printf("  4. Recargar stock\n");
            printf("  5. Ver ganancias del día\n");
            printf("  6. Ver fondo de la máquina\n");
            printf("  7. Ver stock de productos\n");
            printf("  8. Resetear ganancias día\n");
            printf("  9. Configurar Puerto Serial\n");
            printf("  10.Gestionar fondo de máquina\n");
            printf("  0. Salir del modo administrador\n");
            printf("-----------------------------------------\n");
            printf("  Seleccione una opción: ");

            if (scanf("%d", &opcionAdmin) != 1) {
                printf("Entrada inválida. Por favor, ingrese un número.\n");
                limpiarBuffer();
                opcionAdmin = -1;
#ifdef _WIN32
                Sleep(1500);
                system("cls");
#endif
                continue;
            }
            limpiarBuffer();

#ifdef _WIN32
            system("cls");
#endif

            switch (opcionAdmin) {
                case 1:
                    anadirProducto(productos, numProductos);
                    break;
                case 2:
                    eliminarProducto(productos, numProductos);
                    break;
                case 3:
                    modificarProducto(productos, *numProductos);
                break;

                case 4:
                    recargarStock(productos, numProductos);
                    break;
                case 5:
                    verGanancias();
                    break;
                case 6:
                    printf("Fondo actual en la máquina: €%.2f\n",  *pFondoMaquina);
                    break;
                case 7:
                    mostrarProductosConStock(productos, *numProductos);
                    break;
                case 8:
                    resetearGanancias(pGananciasDia);
                    break;
                case 9:
                    configurarPuertoSerial();
                    break;
                case 10:
                    gestionarFondoMaquina(pFondoMaquina);
                    break;
                case 0:
                    printf("Saliendo del modo administrador...\n");
                    break;
                default:
                    printf("Opción inválida. Intente de nuevo.\n");
            }

#ifdef _WIN32
            if (opcionAdmin != 0) {
                printf("\nPresione Enter para volver al menú de administrador...");
                getchar();
                system("cls");
            }
#endif

        } while (opcionAdmin != 0);
    } else {
        perror("Error al leer la contraseña");
    }
}