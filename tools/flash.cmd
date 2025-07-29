@echo off

>nul chcp 65001

set /p FILES_NAME=Введи назву файлу (*.bin): 

echo ===============================

echo Прошивку розпочато

echo ===============================

python -m esptool --chip esp32 write_flash 0x0 %FILES_NAME%.bin

echo ===============================

if %errorlevel% equ 0 (
    echo Прошивку успішно завершено
) else (
    echo Помилка при прошивці
)

echo ===============================

pause