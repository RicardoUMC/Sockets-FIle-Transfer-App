#!/bin/bash

# Variables para los nombres de los archivos de salida
SERVIDOR_BIN="./bin/servidor"
CLIENTE_BIN="./bin/cliente"

# Eliminar el archivo binario del servidor si existe
if [ -f "$SERVIDOR_BIN" ]; then
    rm "$SERVIDOR_BIN"
fi

# Compilar el programa servidor
gcc servidor.c -o "$SERVIDOR_BIN"
if [ $? -ne 0 ]; then
    echo "Error al compilar el programa servidor."
    exit 1
fi

# Compilar el programa cliente
gcc cliente.c -o "$CLIENTE_BIN"
if [ $? -ne 0 ]; then
    echo "Error al compilar el programa cliente."
    exit 1
fi

# Limpiar la pantalla
clear

# Ejecutar el programa servidor solo si la compilación fue exitosa
if [ -x "$SERVIDOR_BIN" ]; then
    "$SERVIDOR_BIN"
else
    echo "No se puede ejecutar el programa servidor debido a un error de compilación."
fi

