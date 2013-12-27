#ifndef INI_H
    #define INI_H

    // Define the device ID
    //#define DEVICE 1

    // Automatic Device detection, since Arduino MEGA is Master and Arduino UNO is slave
    #if defined(__AVR_ATmega328P__)
        #define DEVICE 2
    #elif defined(__AVR_ATmega2560__)
        #define DEVICE 1
    #endif

    // Debug messages
    #define DEBUG

    // Serial BAUD
    #define SERIAL_BAUD 115200

    // Store and read Parameters from EEPROM
    //#define USE_EEPROM

    // Disable Arduino TWI Internal Pull-up Resistor (comment to enable)
    #define DISABLE_PULLUP

    // Configuration for device 1
    #if DEVICE == 1
        // Define flags
        #define MASTER
        #define WEBSERVER
        #define CONTROL_PLACARD
        #define PWM_CONTROL

        // Define the IIC Address
        const   byte     TWIAddr = 0x01;

        //I/O Pin declaration
        const   byte     ANALOGS[]          = {  A2, A3, A6, A7, A8 , A9, A10, A11, A12, A13, A14, A15 };
        const   byte     INPUTS[]           = {  2, 3, 5, 7, 8, 9, 14, 15, 16, 17, 18, 19 };
        const   byte     OUTPUTS[]          = {  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 };

        // PWM Pin Declaration
        #ifdef PWM_CONTROL
            const   byte     PWM[]          = {  40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52 };
        #endif

        // Number of parameters used in this device
        #define SIZE_PARAMETERS 2

        // Create objects for each Slave
        #define SLAVE_LED
        Slave   Slave(0x02);

    #endif


    // Configuration for device 2
    #if DEVICE == 2
        // Define flags
        #define SLAVE
        #define RECEIVE_EVENT
        #define REQUEST_EVENT
        #define PWM_CONTROL

        // Define the IIC Address
        const   byte     TWIAddr = 0x02;

        // I/O Pin declaration
        const   byte     ANALOG[]         = {};
        const   byte     INPUTS[]         = {};
        const   byte     OUTPUTS[]        = {A1, A2};

        // PWM Pin Declaration
        #ifdef PWM_CONTROL
            const   byte     PWM[]        = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0};
        #endif

        // Number of parameters used in this device
        #define SIZE_PARAMETERS 1
    #endif

#endif