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

#include "_ini.h"



// If EEPROM Flag is defined, include EEPROM Library
#ifdef USE_EEPROM
    #include <EEPROM.h>
#endif

// If MASTER or SLAVE Flga is defined, include Wire Library
#if defined(MASTER) || defined(SLAVE)
    #include <Wire.h>
#endif

// Streaming library by Mikal Hart: http://arduiniana.org/libraries/streaming/
#include "lib_Streaming.h"

// SoftPWM library: https://code.google.com/p/rogue-code/wiki/SoftPWMLibraryDocumentation
#ifdef PWM_CONTROL
    #include "lib_SoftPWM.h"
#endif



// Initialize and setup Ethernet Connection
#ifdef USE_ETHERNET
    // Include the libraries
    #include <SPI.h>
    #include <Ethernet.h>

    // MAC address from Ethernet shield sticker under board
    byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    // IP address, may need to change depending on network
    IPAddress ip(192, 168, 2, 40);

    //  Start Webserver
    #ifdef WEBSERVER
        // Create a server at port 80
        EthernetServer server(80);
        // Create the client
        EthernetClient client;
    #endif

    // Start NTP
    #ifdef NTP_TIME
        // Timer library: http://playground.arduino.cc/Code/Time
        #include "lib_Time.h"
        #include "lib_TimeAlarms.h"

        #include <EthernetUdp.h>
        // Local port for UDP packets
        unsigned int NTP_Port = 8888;
        // NTP Server
        IPAddress NTP_Server(66,60,22,202); // 0.ar.pool.ntp.org
        // NTP time stamp is in the first 48 bytes of the message
        const int NTP_PACKET_SIZE = 48;
        // NTP Buffer to hold incoming packets
        byte NTP_buffer[ NTP_PACKET_SIZE];
        // A UDP instance to let us send and receive packets over UDP
        EthernetUDP Udp;
        // Timezone correction from UTC (GMT+0) in seconds
        const  long NTP_TimeZone = +10800L; // GMT-3 Buenos Aires
        // Interval between Time Re-Syncing
        const  int  NTP_sync_interval = 1800;
    #endif
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
#ifdef SLAVE
        byte     ReqPage;
#endif

// Define the debug var
#ifdef DEBUG
    const bool debug = true;
#endif

// Create the EEPROM buffer
#ifdef USE_EEPROM
    int EEPROM_buffer[32];
    int EEPROM_cursor = 0;
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
  
  
    // Read Params from EEPROM memory
    ReadParameters();

    // Start SoftPWM
    #ifdef PWM_CONTROL
        SoftPWMBegin();

        // Set PWM Values
        SetPower();
    #endif


    // Start Ethernet Connection
    #ifdef USE_ETHERNET
        Ethernet.begin(mac, ip);

        // Start WEBSERVER
        #ifdef WEBSERVER
            server.begin();
        #endif

        // Start NTP
        #ifdef NTP_TIME
            if (debug) Serial << "Syncing time via NTP: ";
            Udp.begin(NTP_Port);
            setSyncProvider(getNtpTime);
            setSyncInterval(NTP_sync_interval);
            long NTP_timeout = millis();

             // wait until the time is set by the sync provider
            while(timeStatus() == timeNotSet)
                if (NTP_timeout + 10000 > millis()) {
                    if (debug)  Serial << "Could not connect to NTP Server" << endl;
                    break;
                }
            if (timeStatus() == timeSet) {
                if (debug) Serial << hour() << ":" << minute() << " - " << day() << "/" << month() << "/" << year() << endl;

                // Setup TimerAlarms (ONLY IF TIME WAS SE CORRECTLY)
                Alarm.alarmRepeat(7,30,0, MorningAlarm);  // 7:30am every day
            }
        #endif
    #endif


    // If MASTER or SLAVE Flag is defined, Start I2C Communication
    #if defined(MASTER) || defined(SLAVE)
        Wire.begin(TWIAddr);

        // Set TWBR to 12kHz. Source ( http://www.gammon.com.au/forum/?id=10896 )
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

        // Setup confection with SLAVE_LED
        #ifdef SLAVE_LED
            if (debug) Serial << "Establishing connection with SLAVE #" << Slave.Address() << ": ";
            Slave.Setup();

            if (Slave.Status() ) {
              if (debug) Serial << "Success" << endl;
            }
            else
              if (debug) Serial << "Offline" << endl;
        #endif

        // Define the Slave Interrupt Events
        #ifdef SLAVE
            // Start Request Event Interrupt
            Wire.onRequest(RequestEvent);

            // Start Receive Event Interrupt
            Wire.onReceive(ReceiveEvent);
        #endif
    #endif

    if(debug) Serial << "Startup done" << endl << endl;
    
}


// ---------------------------------------------------
//                   LOOP
// ---------------------------------------------------

void loop() {
    // Web Server
    WebServer();

    // Control Placard
    ControlPlacard();

    // Store Modified Parameters in EEPROM
    StoreParameters();

    // Set the Alarm delay check
    #ifdef NTP_TIME
        Alarm.delay(1);
    #endif
}
