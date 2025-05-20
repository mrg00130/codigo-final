/**
* @file menu_navegacion.h
 * @brief Declara las funciones para la navegación del menú principal.
 */
#ifndef menu_flechas
#define menu_flechas

#include "productos.h" // Asumiendo que productos.h contiene definiciones necesarias (como Producto)

// Prototipo para mostrar el menú con la opción seleccionada resaltada
void mostrarMenuConFlechas(int selectedOption, float fondoMaquina, float gananciasDia);

// Prototipo para manejar la entrada del usuario (flechas, número, Enter) y devolver la opción seleccionada
// Devuelve el índice de la opción seleccionada (0 para la primera, 1 para la segunda, etc.)
int manejarNavegacionMenu(float fondoMaquina, float gananciasDia, int numOpciones);

#endif // menu_flechas