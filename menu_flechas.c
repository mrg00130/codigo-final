/**
 * @file menu_navegacion.c
 * @brief Implementa la lógica de navegación con flechas y selección numérica para el menú principal.
 */

#include "menu_flechas.h" // Incluye el encabezado de navegación del menú
#include <stdio.h> // Para printf, getchar
#ifdef _WIN32
#include <conio.h> // Para _getch() en Windows
#include <windows.h> // Para system("cls") en Windows
#else
// Includes para sistemas Unix-like (termios.h para manipulación de terminal)
#include <termios.h>
#include <unistd.h> // Para read
#endif

// Función auxiliar para obtener una tecla sin eco (para sistemas Unix-like)
// Adaptada de ejemplos comunes para simular _getch()
#ifndef _WIN32
static struct termios oldt, newt;

// Función para preparar la terminal para getch-like
void initTermios(int echo) {
    tcgetattr(STDIN_FILENO, &oldt); // Guarda la configuración actual de la terminal
    newt = oldt; // Copia la configuración actual
    newt.c_lflag &= ~ICANON; // Deshabilita el modo canónico (entrada línea por línea)
    newt.c_lflag &= echo ? TCOFLUSH : ~ECHO; // Habilita/Deshabilita el eco
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica la nueva configuración inmediatamente
}

// Función para restaurar la configuración original de la terminal
void resetTermios(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restaura la configuración original
}

// Función para obtener un carácter sin eco (bloqueante)
int getch_unix(void) {
    int ch;
    initTermios(0); // Deshabilita el eco
    ch = getchar(); // Lee un carácter
    resetTermios(); // Restaura la configuración original
    return ch;
}

// Función para obtener un carácter con eco (bloqueante)
int getche_unix(void) {
    int ch;
    initTermios(1); // Habilita el eco
    ch = getchar(); // Lee un carácter
    resetTermios(); // Restaura la configuración original
    return ch;
}
#endif

/**
 * @brief Muestra el menú principal resaltando la opción seleccionada.
 *
 * Imprime las opciones del menú, colocando un indicador (-->) junto a la opción actual.
 * @param selectedOption Índice de la opción actualmente seleccionada (0 para Comprar, 1 para Admin, 2 para Salir).
 * @param fondoMaquina Valor actual del fondo de la máquina (se muestra en el menú).
 * @param gananciasDia Valor actual de las ganancias del día (se muestra en el menú).
 */
void mostrarMenuConFlechas(int selectedOption, float fondoMaquina, float gananciasDia) {
    // Limpia la pantalla antes de mostrar el menú
#ifdef _WIN32
    system("cls");
#else
    // Usa códigos de escape ANSI para limpiar la pantalla en sistemas Unix-like
    printf("\033[2J\033[H"); // Limpia la pantalla y mueve el cursor al inicio
#endif

    printf("========================================\n");
    printf("   ~ MÁQUINA EXPENDEDORA ~   \n");
    printf("========================================\n");
    printf("----------------------------------------\n");
    // Mostrar información del estado de la máquina (fondo y ganancias)
    // printf("  Fondo: €%.2f | Ganancias: €%.2f\n", fondoMaquina, gananciasDia); // Opción para mostrar siempre
    printf("\n"); // Espacio para mejor formato

    // Imprime las opciones del menú, resaltando la seleccionada
    printf("%s 1. Comprar producto\n", selectedOption == 0 ? "-->" : "   ");
    printf("%s 2. Modo Administrador\n", selectedOption == 1 ? "-->" : "   ");
    printf("%s 0. Salir\n", selectedOption == 2 ? "-->" : "   "); // La opción 0 está en el índice 2

    printf("----------------------------------------\n");
    printf("\n");
    printf("  Use las flechas ↑ y ↓ para navegar, Enter o número para seleccionar.\n");
}

/**
 * @brief Maneja la navegación del menú principal usando flechas o selección numérica.
 *
 * Muestra el menú principal y espera la entrada del usuario (flechas, Enter, números).
 * Permite seleccionar una opción usando las flechas arriba/abajo y Enter,
 * o ingresando el número correspondiente (1, 2, 0).
 * Devuelve el índice de la opción seleccionada (0, 1, 2) al menú principal.
 * @param fondoMaquina Valor actual del fondo de la máquina.
 * @param gananciasDia Valor actual de las ganancias del día.
 * @param numOpciones Número total de opciones en el menú (en este caso, 3: Comprar, Admin, Salir).
 * @return El índice de la opción seleccionada (0 para Comprar, 1 para Admin, 2 para Salir). Retorna -1 si hay un error grave (aunque el bucle está diseñado para no salir con -1).
 */
int manejarNavegacionMenu(float fondoMaquina, float gananciasDia, int numOpciones) {
    int selectedOption = 0; // Inicia en la primera opción (índice 0)
    int opcion = -1; // Variable para la opción devuelta (inicialmente inválida)

    // Bucle que continúa hasta que el usuario selecciona una opción válida (no -1)
    while (opcion == -1) {
        // Muestra el menú con la opción actual resaltada
        mostrarMenuConFlechas(selectedOption, fondoMaquina, gananciasDia);

        int input; // Variable para almacenar la tecla presionada

        // Lee la tecla presionada sin necesidad de presionar Enter (getch-like)
#ifdef _WIN32
        input = _getch(); // Usa _getch() en Windows
#else
        input = getch_unix(); // Usa nuestra función getch-like en Unix
#endif

        // Verifica si la entrada es un número válido (0, 1, 2 en este caso)
        if (input >= '0' && input < '0' + numOpciones) {
            // Mapea el carácter numérico a su valor entero y lo asigna a 'opcion'
            // '0' -> 0, '1' -> 1, '2' -> 2
            opcion = input - '0';

            // Ajustar el valor devuelto para que coincida con los casos lógicos de main()
            // Si presionó '1' (indice 0 visual), queremos retornar el valor 1 para el switch de main
            // Si presionó '2' (indice 1 visual), queremos retornar el valor 2 para el switch de main
            // Si presionó '0' (indice 2 visual), queremos retornar el valor 0 para el switch de main
            // Esto significa que el índice visual (0, 1, 2) corresponde directamente a selectedOption
            // Y el 'opcion' que retornamos debe ser 1 (si input='1'), 2 (si input='2'), 0 (si input='0')
            // El mapeo correcto es: input '1' -> selectedOption=0; input '2' -> selectedOption=1; input '0' -> selectedOption=2.
            // Y queremos que el switch en main actúe así: case 1 (para comprar), case 2 (para admin), case 0 (para salir).
            // La función debe devolver el índice visual seleccionado (0, 1, 2)
            // El mapeo en main.c se encargará de convertir el índice visual (0, 1, 2) al caso lógico (1, 2, 0).
            // Por lo tanto, aquí simplemente devolvemos el índice visual basado en el número presionado.
             switch (input) {
                case '1': return 0; // '1' corresponde al primer elemento (indice 0)
                case '2': return 1; // '2' corresponde al segundo elemento (indice 1)
                case '0': return 2; // '0' corresponde al tercer elemento (indice 2)
                default: break; // No debería llegar aquí
            }


        }
#ifdef _WIN32
        // Manejo de teclas especiales en Windows (flechas, Enter)
        else if (input == 224) { // Código para teclas extendidas (flechas, F-keys, etc.)
            input = _getch(); // Leer el segundo código de la tecla extendida
            switch (input) {
                case 72: // Flecha arriba
                    // Mueve la selección hacia arriba, envolviendo si llega al principio
                    selectedOption = (selectedOption - 1 + numOpciones) % numOpciones;
                    break;
                case 80: // Flecha abajo
                    // Mueve la selección hacia abajo, envolviendo si llega al final
                    selectedOption = (selectedOption + 1) % numOpciones;
                    break;
                // Puedes añadir casos para otras flechas si es necesario (ej. 75: Izquierda, 77: Derecha)
            }
        } else if (input == 13) { // Tecla Enter
            // Si se presiona Enter, la opción seleccionada es el índice actual
            opcion = selectedOption;
             // Retorna el índice seleccionado (0, 1, o 2)
            return opcion;
        }
#else
        // Manejo de teclas especiales en sistemas Unix-like (flechas)
        else if (input == 27) { // Código de escape (inicio de secuencia ANSI)
            // Leer los siguientes dos caracteres para identificar la tecla de flecha
            if (getchar() == '[') {
                switch (getchar()) {
                    case 'A': // Flecha arriba
                        // Mueve la selección hacia arriba, envolviendo
                        selectedOption = (selectedOption - 1 + numOpciones) % numOpciones;
                        break;
                    case 'B': // Flecha abajo
                        // Mueve la selección hacia abajo, envolviendo
                        selectedOption = (selectedOption + 1) % numOpciones;
                        break;
                    // Puedes añadir casos para otras flechas si es necesario ('C': Derecha, 'D': Izquierda)
                    // case 'C': // Right
                    //     break;
                    // case 'D': // Left
                    //     break;
                }
            }
        } else if (input == '\n') { // Tecla Enter en Unix es '\n'
             // Si se presiona Enter, la opción seleccionada es el índice actual
            opcion = selectedOption;
            // Retorna el índice seleccionado (0, 1, o 2)
            return opcion;
        }
#endif
        // Si la entrada no fue un número válido o una tecla de navegación, 'opcion' sigue siendo -1,
        // y el bucle while continua.
    }

    // Este código solo se alcanzaría si el bucle terminara de forma inesperada,
    // pero con el 'return' dentro del bucle, normalmente no se ejecuta.
    return opcion;
}