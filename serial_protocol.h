#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdbool.h>
#include <stddef.h>

HANDLE serial_abrir_puerto(const char* puertoCOM, int baudRate);
void serial_cerrar_puerto(void);
void serial_limpiar_buffer(HANDLE hComm);
void serial_protocolo_automatico(HANDLE hComm, float fondo, float precio, float importe, char* respuesta, size_t maxlen);

#endif // SERIAL_PROTOCOL_H