@echo off

>nul chcp 65001

cd %CD%

set /p FILES_NAME=Введи назву файлу (*.ino):

echo ===============================

echo Об'єднання розпочато

echo ===============================

python -m esptool --chip ESP32 merge_bin -o %FILES_NAME%.bin --flash_mode dio --flash_size 4MB 0x1000 %FILES_NAME%.ino.bootloader.bin 0x8000 %FILES_NAME%.ino.partitions.bin 0xe000 boot.bin 0x10000 %FILES_NAME%.ino.bin

echo ===============================

if %errorlevel% equ 0 (
    echo Об'єднано успішно
) else (
    echo Виникла помилка при об'єднанні
)

echo ===============================

pause