#!/bin/bash

# Variables para los nombres de los archivos de salida
SERVER_BIN="./server/server"
CLIENT_BIN="./client/client"

# Eliminar el archivo binario del servidor si existe
if [ -f "$SERVER_BIN" ]; then
    rm "$SERVER_BIN"
fi

# Compilar el programa servidor
gcc ./server/server.c -o "$SERVER_BIN"
if [ $? -ne 0 ]; then
    echo "Err./clienteor al compilar el programa servidor."
    exit 1
fi

# Compilar el programa cliente
gcc ./client/client.c -o "$CLIENT_BIN"
if [ $? -ne 0 ]; then
    echo "Error al compilar el programa cliente."
    exit 1
fi

# Limpiar la pantalla
clear

# Ejecutar el programa servidor solo si la compilación fue exitosa
if [ -x "$SERVER_BIN" ]; then
    "$SERVER_BIN"
else
    echo "No se puede ejecutar el programa servidor debido a un error de compilación."
fi

