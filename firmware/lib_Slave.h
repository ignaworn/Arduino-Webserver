#ifndef SLAVE_H
    #define SLAVE_H

    #if defined(ARDUINO) && ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "WProgram.h"
    #endif

    #include "Wire.h"

    class Slave {

      private:
        void TestConnection();
        void PinData();
        void GetPage(byte Page, byte ReqSize, byte Store[]);

        byte _Address;
        bool _Status;
        bool _Setup;
        byte SizeParameters, SizeAnalogs, SizeInputs, SizeOutputs, SizePWM;
        byte Parameters[32], ANALOGS[32], INPUTS[32], OUTPUTS[32], PWM[32];
        byte _Parameters[32], _ANALOGS[32], _INPUTS[32], _OUTPUTS[32], _PWM[32];

      public:
        Slave(byte Address);
        void Update();
        void Setup();
        byte Address();
        bool Status();

        void SendData(int Mode, byte Pin, byte Value);

        byte Size(char Type);
        byte Value(char Type, byte ID);
        byte ID(char Type, byte ID);

    };


    // Constructor
    Slave::Slave(byte Address) {
      _Address = Address;
    }

    // Return Slave Address
    byte Slave::Address() {
      return _Address;
    }

    // Return Slave Online Status
    bool Slave::Status() {
      return _Status;
    }

    // Return Slave Size data
    byte Slave::Size(char Type) {
      switch(Type) {
        case 'P':   return SizeParameters; break;
        case 'I':       return SizeInputs; break;
        case 'A':      return SizeAnalogs; break;
        case 'O':      return SizeOutputs; break;
        case 'L':         return SizePWM; break;
      }
    }

    // Return Slave PIN value
    byte Slave::Value(char Type, byte ID) {
      switch(Type) {
        case 'P':   return _Parameters[ID]; break;
        case 'I':       return _INPUTS[ID]; break;
        case 'A':      return _ANALOGS[ID]; break;
        case 'O':      return _OUTPUTS[ID]; break;
        case 'L':         return _PWM[ID]; break;
      }
    }

    // Return Slave PIN ID
    byte Slave::ID(char Type, byte ID) {
      switch(Type) {
        case 'P':   return Parameters[ID]; break;
        case 'I':       return INPUTS[ID]; break;
        case 'A':      return ANALOGS[ID]; break;
        case 'O':      return OUTPUTS[ID]; break;
        case 'L':         return PWM[ID]; break;
      }
    }

    // Test connection and retrieve resource parameters
    void Slave::Setup() {
       // Dont execute it twice.
       if (_Setup)
         return;

       // Test Connection
       TestConnection();

       // If is online
       if (_Status) {
         // Get Pin Data
         PinData();
         // Update Pin values
         Update();
       }

       _Setup = true;
    }

    // Test connection to Slave and check status
    void Slave::TestConnection() {
        // Start the I2C Communication
        Wire.beginTransmission(_Address);
        // Send a Random Byte
        Wire.write(0xFF);
        // Check response
        if (Wire.endTransmission() == 0)
          _Status = true;
    }

    // Get the desired Page from Slave from 0x00 to 0x0A
    void Slave::GetPage(byte Page, byte ReqSize, byte Store[]) {
        // Don't call if there is nothing to show
        if (ReqSize == 0)
          return;

        // Start I2C Communication with Slave
        Wire.beginTransmission(_Address);

        // Write in buffer the command
        Wire.write(Page);

        // Send de data to the I2C Slave
        Wire.endTransmission();

        // Start Handler for receive data.
        Wire.requestFrom(_Address, ReqSize);

        // Store response in a byte array
        for (int i=0; i<ReqSize; i++)
          Store[i] = Wire.read();

    }

    // Set the Slave Pin Data
    void Slave::PinData() {
        // Create a temporary buffer
        byte PinData[5];
        // Fetch page 0x00, contains pin size
        GetPage(0x00, 5, PinData);

        // Store in object memory
        SizeAnalogs    = PinData[0];
        SizeInputs     = PinData[1];
        SizeOutputs    = PinData[2];
        SizePWM        = PinData[3];
        SizeParameters = PinData[4];

        // Retrieve Pin Numbers
        GetPage(0x01, SizeAnalogs, ANALOGS);
        GetPage(0x03, SizeInputs, INPUTS);
        GetPage(0x05, SizeOutputs, OUTPUTS);
        GetPage(0x07, SizePWM, PWM);
        GetPage(0x09, SizeParameters, Parameters);
    }

    // Update all Pin Data Values
    void Slave::Update() {
        // Update Analog values
        GetPage(0x02, SizeAnalogs, _ANALOGS);

        // Update Inputs values
        GetPage(0x04, SizeInputs, _INPUTS);

        // Update Outputs values
        GetPage(0x06, SizeOutputs, _OUTPUTS);

        // Update PWM values
        GetPage(0x08, SizePWM, _PWM);

        // Update Parameters values
        GetPage(0x0A, SizeParameters, _Parameters);
    }

    // Send data to Slave
    void Slave::SendData(int Mode, byte Pin, byte Value) {
       // Begin I2C Communication
       Wire.beginTransmission(_Address);

       byte Type;
       switch(Mode) {
         case 'P': Type = 0xA0; break;
         case 'M': Type = 0xA1; break;
         case 'L': Type = 0xA2; break;
       }

       // Send the Mode
       Wire.write(Type);
       // Send Pin Number
       Wire.write(Pin);
       // Send Digital Value
       Wire.write(Value);

       // End I2C Communication, send TWI Buffer to Slave
       Wire.endTransmission();
    }

#endif