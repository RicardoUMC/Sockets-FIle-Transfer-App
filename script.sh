#!/bin/bash
gcc servidor.c -o ./bin/servidor
gcc cliente.c -o ./bin/cliente
clear
./bin/servidor
