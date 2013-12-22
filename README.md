Arduino-Webserver
=================

 Description:   Servidor Web con Arduino que permite monitorear y modificar los valores
                de las entradas y salidas del Arduino Maestro. Controlar un Arduino 
                esclavo con 6 Led RGB desde un dispositivo remoto conectado a la red
                LAN.
                Es necesario abrir el archivo .htm desde un terminal.

 Hardware:      Arduino Uno y Arduino Ethernet shield. Es posible que funcione con
                equivalentes.
                Arduino Mini para controlar los led RGB.
                Resistencias Pull-UP 2x4.7k ohm (SCA & SCL)
                Las entradas y salidas se configuran en el proyecto.

 Software:      Desarrollado usando Arduino 1.5.4 beta
                Compatible con Arduino 1.0 +

 References:    - WebServer example by David A. Mellis and modified by Tom Igoe
                - Ethernet library documentation: http://arduino.cc/en/Reference/Ethernet
                - SD Card library documentation: http://arduino.cc/en/Reference/SD

 Fecha:         4 April 2013
 Modificado:    19 June 2013 - removed use of the String class

 Autor:         I. Worn
