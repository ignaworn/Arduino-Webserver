Arduino-Webserver
=================

Description:
Servidor Web con Arduino que permite monitorear y modificar los valores de las entradas y salidas del Arduino Maestro. Controlar un Arduino esclavo con 6 Led RGB desde un dispositivo remoto conectado a la red LAN.
Es necesario abrir el archivo .htm desde un terminal.

Hardware:
- Arduino Mega2560 y Arduino Ethernet shield.
- Arduino Mini para controlar los led RGB.
- BC548 y Resistencias de 1k ohm para cada salida RGB.
- Resistencias Pull-UP 6.8k ohm (SCA & SCL)

Software:
- Desarrollado usando Arduino 1.5.4r2 beta
- JetBrains PHPStorm v7.1
- Compatible con Arduino 1.0 +

References:
- WebServer example by David A. Mellis and modified by Tom Igoe
- Ethernet library documentation: http://arduino.cc/en/Reference/Ethernet
- SoftPWM library documentation: https://code.google.com/p/rogue-code/wiki/SoftPWMLibraryDocumentation
- Streaming library documentation: http://arduiniana.org/libraries/streaming/
- FlexiColorPicker js library documentation: http://www.daviddurman.com/flexi-color-picker

Fecha:
- 4 April 2013

Modificado:
- 27 December 2013

Autor:
- I. Worn
