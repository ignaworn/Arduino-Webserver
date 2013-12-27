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

 -------------------------------------------------------------- */


// Include EEPROM Library
#include <EEPROM.h>

#include <Wire.h>


#include "Slave.h"

#include "_ini.h"



// Streaming library by Mikal Hart: http://arduiniana.org/libraries/streaming/
#include "Streaming.h"

// SoftPWM library: https://code.google.com/p/rogue-code/wiki/SoftPWMLibraryDocumentation
#ifdef PWM_CONTROL
    #include "SoftPWM.h"
#endif

// Define and include the Webserver Resources
#ifdef WEBSERVER
    // Include the libraries
    #include <SPI.h>
    #include <Ethernet.h>

    // MAC address from Ethernet shield sticker under board
        byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    // IP address, may need to change depending on network
        IPAddress ip(192, 168, 2, 40);
    // Create a server at port 80
        EthernetServer server(80);
    // Create the client
        EthernetClient client;
#endif

// I/O counts
const   byte     SizeAnalogs         = sizeof(ANALOGS)/sizeof(ANALOGS[0]);
const   byte     SizeInputs          = sizeof(INPUTS)/sizeof(INPUTS[0]);
const   byte     SizeOutputs         = sizeof(OUTPUTS)/sizeof(OUTPUTS[0]);
const   byte     SizePWM             = sizeof(PWM)/sizeof(PWM[0]);

// Output PWM Values
        byte     _PWM[SizePWM];

// Parameters and EEPROM Parameters are stored in the array.
const   byte     SizeParameters = SIZE_PARAMETERS;
        byte     Parameters[SizeParameters];

// Request Page, used by the Slave in functions RequestEvent & ReceiveEvent.

        byte     ReqPage;


// Define the debug var
#ifdef DEBUG
    const bool debug = true;
#endif

// ------------------------------------------------------------


void setup() {

    // Start Serial communication
    if(debug) Serial.begin(SERIAL_BAUD);

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
    UpdateParameters(false);

    // Start SoftPWM
    #ifdef PWM_CONTROL
        SoftPWMBegin();

        // Set PWM Values
        SetPower();
    #endif
    

    // Start I2C Communication
    Wire.begin(TWIAddr);

    // Set TWBR to 12kHz http://www.gammon.com.au/forum/?id=10896
    TWBR = 158;  
    TWSR |= _BV (TWPS0);

    // Disable internal pull-up resistors.
    #ifdef DISABLE_PULLUP
        #ifndef cbi
            #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
        #endif
        #if defined(__AVR_ATmega2560__)
            cbi(PORTC, 20);
            cbi(PORTC, 21);
        #endif
        #if defined(__AVR_ATmega328P__)
            cbi(PORTC, 4);
            cbi(PORTC, 5);
        #endif
    #endif
    
    // Test I2C Communicatoin with LED SLAVE
    // And read the Pin I/O Status
    // Wait 1 sec to Startup
    #ifdef SLAVE_LED
        delay(1000);
        if (debug) Serial << "Establishing connection with SLAVE #" << Slave.Address() << " ... ";
        Slave.Setup();

        if (Slave.Status() )
          if (debug) Serial << "Success" << endl;
    #endif

    // Start Ethernet Connection and Start Server
    #ifdef WEBSERVER
        Ethernet.begin(mac, ip);
        server.begin();
    #endif


    // Define the Slave Interrupt Events
    #ifdef SLAVE
        // Start Request Event Interrupt
        Wire.onRequest(RequestEvent);

        // Start Receive Event Interrupt
        Wire.onReceive(ReceiveEvent);
    #endif


    if(debug) Serial << "Startup done" << endl << endl;
    
}


// ---------------------------------------------------
//                   LOOP
// ---------------------------------------------------

void loop() {
    // Web Server
    #ifdef WEBSERVER
        client = server.available();
        if (client) WebServer();
    #endif

    // Control Placard
    #ifdef CONTROL_PLACARD
        ControlPlacard();
    #endif

    // Store Parameters in EEPROM
    UpdateParameters(true);
}
