@echo off
echo Compilando version Windows con MSVC...
cl /W4 /EHsc main_windows.c log_processor.c
py access_log_file_generator.py

if errorlevel 1 (
    echo Error al compilar.
    pause
    exit /b 1
)

echo Ejecutando programa...
.\main_windows.exe