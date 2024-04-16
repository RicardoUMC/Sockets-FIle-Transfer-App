#!/bin/bash

SERVER_SRC="./../server/server.c"
CLIENT_SRC="./../client/client.c"
SERVER_BIN="./../server/server"
CLIENT_BIN="./../client/client"

# Eliminar el archivo binario del servidor si existe
if [ -f "$SERVER_BIN" ]; then
  rm "$SERVER_BIN"
fi

# Compilar el programa servidor
gcc "$SERVER_SRC" -o "$SERVER_BIN"
if [ $? -ne 0 ]; then
  echo "Error al compilar el programa servidor."
  exit 1
fi

# Compilar el programa cliente
gcc "$CLIENT_SRC" -o "$CLIENT_BIN"
if [ $? -ne 0 ]; then
  echo "Error al compilar el programa cliente."
  exit 1
fi

# Limpiar la pantalla
clear

# Verificar la existencia y ejecutar el programa servidor
if [ -f "$SERVER_BIN" ]; then
  "$SERVER_BIN"
else
  echo "Error: No se pudo crear el programa servidor correctamente."
fi
