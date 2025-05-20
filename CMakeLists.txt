cmake_minimum_required(VERSION 3.30)
project(Programa_informatica_final C)

set(CMAKE_C_STANDARD 11)

add_executable(Programa_informatica_final main.c
        administrador.c
        administrador.h
        productos_manejo.c
        productos_manejo.h
        productos.h
        compra_venta.c
        compra_venta.h
        menu_flechas.c
        menu_flechas.h
        serial_protocol.c
        serial_protocol.h
        pagos.c
        pagos.h
        fondo_manejo.c
        fondo_manejo.h
)
