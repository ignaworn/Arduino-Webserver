// ---------------------------------------------------
//                   LED_RGB-slave
// ---------------------------------------------------

// Streaming library by Mikal Hart: http://arduiniana.org/libraries/streaming/
#include "Streaming.h"
// SoftPWM library: https://code.google.com/p/rogue-code/wiki/SoftPWMLibraryDocumentation
#include "SoftPWM.cpp"

#include <EEPROM.h>

#include <Wire.h>


// Arduino Master Adress
const   byte     I2CMaster        = 1;
// Arduino Slave_Led Adress
const   byte     I2CSlave_Led     = 2;

// I/O Pin declaration
const   byte     ANALOG[]         = {};
const   byte     INPUTS[]         = {};
const   byte     OUTPUTS[]        = {A1,A2};
const   byte     PWM[]            = {2,3,4,5,6,7,8,9,10,11,12,13,A0};
// I/O Pin counts
const   byte     SizeAnalog       = sizeof(ANALOG)/sizeof(ANALOG[0]);
const   byte     SizeInputs       = sizeof(INPUTS)/sizeof(INPUTS[0]);
const   byte     SizeOutputs      = sizeof(OUTPUTS)/sizeof(OUTPUTS[0]);
const   byte     SizePWM          = sizeof(PWM)/sizeof(PWM[0]);
const   byte     SizeParameter    = 1;
// Output PWM Values
        byte     _PWM[SizePWM]  = {0xFF,0xFF,0xFF,0xFF};

// Request Page, controlled by Master.
        byte     ReqPage;
        
// Debug messages
const   boolean  debug            = false;
// Store and read Parameters from EEPROM
const   boolean  UseEEPROM        = false;
// Disable Arduino TWI Internal Resistor (true to disable) Only in ATmega328P
const   boolean  TWI_IntResistor  = true;

// Parameters and EEPROM Parameters are stored in the array.
        byte     Parameter[SizeParameter] = {0x00};
        

void setup() {

    if (debug) Serial.begin(115200);
    
    // Start Outputs
    for (int i=0; i<SizeOutputs; i++) {
        if (debug) Serial << "Pin: " << OUTPUTS[i] << " set as Output" << endl;
        pinMode(OUTPUTS[i], OUTPUT);
    }
    
    // Update Parameters
    UpdateParameters(false);
    
    // Start SoftPWM 
    SoftPWMBegin();
    
    // Set PWM Values
    SetPower();
    
    // Start I2C com - Adress 2
    Wire.begin(I2CSlave_Led);
    
    // Disable internal pull-up resistors. Only in ATmega328P
    if (TWI_IntResistor) {
      #ifndef cbi
        #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
      #endif
      cbi(PORTC, 4);
      cbi(PORTC, 5);
    }
    
    // Start Request Event Interrupt
    Wire.onRequest(RequestEvent);
    
    // Start Receive Event Interrupt
    Wire.onReceive(ReceiveEvent);
    
}

void loop() {
    delay(5000);
    
    UpdateParameters(true);
}


