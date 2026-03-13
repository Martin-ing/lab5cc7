#!/bin/bash

echo "Compilando version Linux..."
gcc -Wall -Wextra -std=c11 main_linux.c log_processor.c -o log_analyzer -lpthread
python3 access_log_file_generator.py

if [ $? -ne 0 ]; then
    echo "Error al compilar."
    exit 1
fi

echo "Ejecutando programa..."
./log_analyzer