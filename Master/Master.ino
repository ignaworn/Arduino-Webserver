/*--------------------------------------------------------------
 Programa:      Ethernet Webserver & Slave Control

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
 
 --------------------------------------------------------------
Arduino MEGA Master. 

 A0: used by Ethernet shield

 A1: used by Ethernet shield

 D0: used by serial RX

 D1: used by serial TX

 D4: used by Ethernet shield

 D10 to D13 used by Ethernet shield

 --------------------------------------------------------------
EEPROM Parameters

0 - Byte - Set PWM Power (On/Off)
1 - Byte - Set Timer Control Placard (On/Off)

 --------------------------------------------------------------

*/

// Streaming library by Mikal Hart: http://arduiniana.org/libraries/streaming/
#include "libraries/Streaming.h"

// SoftPWM library: https://code.google.com/p/rogue-code/wiki/SoftPWMLibraryDocumentation
#include "libraries/SoftPWM.cpp"

#include <EEPROM.h>

#include <SPI.h>

#include <Ethernet.h>

#include <Wire.h>

#include "libraries/Slave.h"




// Arduino Master Address
const   byte     I2CMaster         = 0x01;

// Arduino Slave_Led Address
        Slave    Slave(0x02);

// MAC address from Ethernet shield sticker under board
        byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
        
// IP address, may need to change depending on network
        IPAddress ip(192, 168, 2, 40);        
        
// Create a server at port 80    
        EthernetServer server(80);    
        
// Create the client
        EthernetClient client;


// Master I/O Pin declaration
const   byte     ANALOGS[]          = {  A2, A3, A6, A7, A8 };//, A9, A10, A11, A12, A13, A14, A15 };

const   byte     INPUTS[]           = {  2,3,5,7,8,9};//,14,15,16,17,18,19 };

const   byte     OUTPUTS[]          = {  22,23,24,25,26,27};//,28,29,30,31,32,33,34,35,36,37,38,39 };

const   byte     PWM[]             = {  40,41,42,43,44,45};//,46,47,48,49,50,51,52 };


// I/O counts
const   byte     SizeAnalogs         = sizeof(ANALOGS)/sizeof(ANALOGS[0]);

const   byte     SizeInputs          = sizeof(INPUTS)/sizeof(INPUTS[0]);

const   byte     SizeOutputs         = sizeof(OUTPUTS)/sizeof(OUTPUTS[0]);

const   byte     SizePWM            = sizeof(PWM)/sizeof(PWM[0]);


// Output PWM Values
        byte     _PWM[SizePWM]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};//, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


// Debug messages
const   boolean  debug            = true;

// Store and read Parameters from EEPROM
const   boolean  UseEEPROM        = true;

// Disable Arduino TWI Internal Resistor (true to disable) Only in ATmega328P
const   boolean  TWI_IntResistor  = true;


// Parameters and EEPROM Parameters are stored in the array.
const   byte     SizeParameters    = 2;

        byte     Parameters[SizeParameters] = {0};
// ------------------------------------------------------------


void setup() {

    // Start Serial communication
    if(debug) Serial.begin(115200);

    if(debug) Serial << "Starting up..." << endl;


    // Start Digital Inputs Pin
    if (SizeInputs > 0) {
      if (debug) Serial << "Inputs: ";
      for (int i=0; i<SizeInputs; i++) {
          if (debug) Serial << INPUTS[i] << " ";
          pinMode(INPUTS[i], INPUT);
      }
      if (debug) Serial << endl;
    }
    
    // Start Digital Output Pin
    if (SizeOutputs > 0) {
      if (debug) Serial << "Outputs: ";
      for (int i=0; i<SizeOutputs; i++) {
          if (debug) Serial << OUTPUTS[i] << " ";
          pinMode(OUTPUTS[i], OUTPUT);
      }
      if (debug) Serial << endl;
    }
  
  
     // Read Params 
    UpdateParameters();

    // Start SoftPWM
    SoftPWMBegin();
    
    // Set PWM Values
    SetPower();
    

    // Start I2C Communication as Master
    Wire.begin(I2CMaster);

    // Set TWBR to 12kHz http://www.gammon.com.au/forum/?id=10896
    TWBR = 158;  
    TWSR |= _BV (TWPS0);

    // Disable internal pull-up resistors. Only in Arduino Mega
    if (TWI_IntResistor) {
      #ifndef cbi
        #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
      #endif
      cbi(PORTC, 20);
      cbi(PORTC, 21);
    }
    
    // Test I2C Communicatoin with LED SLAVE
    // And read the Pin I/O Status
    // Wait 1 sec to Startup
    delay(1000);
    if (debug) Serial << "Establishing connection with SLAVE #" << Slave.Address() << " ... "; 
    Slave.Setup();

    if (Slave.Status() )
      if (debug) Serial << "Success" << endl;
      
    // Start Ethernet Connection
    Ethernet.begin(mac, ip);  

    // Start Server
    server.begin();


    if(debug) Serial << "Startup done" << endl << endl;
    
}


// ---------------------------------------------------
//                   LOOP
// ---------------------------------------------------

void loop() {
    // Web Server
    client = server.available();
    if (client) WebServer();
    
    // Control Placard
    ControlPlacard();
}
