#include "serial_protocol.h"
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
static HANDLE hCommGlobal = INVALID_HANDLE_VALUE;
#endif

HANDLE serial_abrir_puerto(const char* puertoCOM, int baudRate) {
#ifdef _WIN32
    if (hCommGlobal != INVALID_HANDLE_VALUE) {
        return hCommGlobal;
    }
#endif
    char nombrePuerto[32];
    snprintf(nombrePuerto, sizeof(nombrePuerto), "\\\\.\\%s", puertoCOM);
    HANDLE hComm = CreateFileA(
        nombrePuerto,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL
    );
    if (hComm == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        printf("No se pudo abrir el puerto %s (error %lu)\n", nombrePuerto, err);
        return INVALID_HANDLE_VALUE;
    }
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hComm, &dcbSerialParams);
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;
    SetCommState(hComm, &dcbSerialParams);
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hComm, &timeouts);
#ifdef _WIN32
    hCommGlobal = hComm;
#endif
    return hComm;
}

void serial_cerrar_puerto(void) {
#ifdef _WIN32
    if (hCommGlobal != INVALID_HANDLE_VALUE) {
        CloseHandle(hCommGlobal);
        hCommGlobal = INVALID_HANDLE_VALUE;
    }
#endif
}

void serial_limpiar_buffer(HANDLE hComm) {
#ifdef _WIN32
    DWORD errors;
    COMSTAT status;
    char basura[256];
    DWORD bytesRead;
    if (ClearCommError(hComm, &errors, &status)) {
        while (status.cbInQue > 0) {
            DWORD toRead = (status.cbInQue < sizeof(basura)) ? status.cbInQue : sizeof(basura);
            ReadFile(hComm, basura, toRead, &bytesRead, NULL);
            ClearCommError(hComm, &errors, &status);
        }
    }
#endif
}
void serial_protocolo_automatico(HANDLE hComm, float fondo, float precio, float importe, char* respuesta, size_t maxlen) {
    char buffer[256];
    char linea[64];
    DWORD bytesRead = 0, bytesWritten = 0;
    int etapa = 0;
    int encontrada_respuesta = 0;
    int maxIntentos = 1000;
    respuesta[0] = '\0';

    while (maxIntentos--) {
        int i = 0;
        char c = 0;
        while (i < (int)sizeof(buffer) - 1) {
            if (!ReadFile(hComm, &c, 1, &bytesRead, NULL) || bytesRead != 1) {
#ifdef _WIN32
                Sleep(5);
#endif
                continue;
            }
            buffer[i++] = c;
            if (c == '\n' || c == '\r') break;
        }
        buffer[i] = '\0';

        if (strstr(buffer, "Dinero disponible en maquina:") && etapa == 0) {
            snprintf(linea, sizeof(linea), "%.2f\r\n", fondo);
            WriteFile(hComm, linea, strlen(linea), &bytesWritten, NULL);
            etapa = 1;
        } else if (strstr(buffer, "Precio del producto:") && etapa == 1) {
            snprintf(linea, sizeof(linea), "%.2f\r\n", precio);
            WriteFile(hComm, linea, strlen(linea), &bytesWritten, NULL);
            etapa = 2;
        } else if (strstr(buffer, "Importe entregado:") && etapa == 2) {
            snprintf(linea, sizeof(linea), "%.2f\r\n", importe);
            WriteFile(hComm, linea, strlen(linea), &bytesWritten, NULL);
            etapa = 3;
        }

        if (strstr(buffer, "dinero insuficiente") ||
            strstr(buffer, "importe exacto") ||
            strstr(buffer, "No hay cambio disponible") ||
            strstr(buffer, "Compra posible")) {
            strncpy(respuesta, buffer, maxlen-1);
            respuesta[maxlen-1] = '\0';
            encontrada_respuesta = 1;
        }

        if (strstr(buffer, "-----"))
            break;
    }

    if (!encontrada_respuesta) {
        strncpy(respuesta, "Error: No se recibió respuesta válida del hardware.", maxlen-1);
        respuesta[maxlen-1] = '\0';
    }
}