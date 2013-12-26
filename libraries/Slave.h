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



Slave::Slave(byte Address) {
  _Address = Address;
}

byte Slave::Address() {
  return _Address;
}
bool Slave::Status() {
  return _Status;
}
byte Slave::Size(char Type) {
  switch(Type) {
    case 'P':   return SizeParameters; break;
    case 'I':       return SizeInputs; break;
    case 'A':      return SizeAnalogs; break;
    case 'O':      return SizeOutputs; break;
    case 'L':         return SizePWM; break;
  }  
}
byte Slave::Value(char Type, byte ID) {
  switch(Type) {
    case 'P':   return _Parameters[ID]; break;
    case 'I':       return _INPUTS[ID]; break;
    case 'A':      return _ANALOGS[ID]; break;
    case 'O':      return _OUTPUTS[ID]; break;
    case 'L':         return _PWM[ID]; break;
  }  
}
byte Slave::ID(char Type, byte ID) {
  switch(Type) {
    case 'P':   return Parameters[ID]; break;
    case 'I':       return INPUTS[ID]; break;
    case 'A':      return ANALOGS[ID]; break;
    case 'O':      return OUTPUTS[ID]; break;
    case 'L':         return PWM[ID]; break;
  }  
}
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

void Slave::TestConnection() {
    // Start the I2C Communication
    Wire.beginTransmission(_Address);
    // Send a Random Byte
    Wire.write(0xFF);
    // Check response
    if (Wire.endTransmission() == 0)
      _Status = true;
}




void Slave::GetPage(byte Page, byte ReqSize, byte Store[]) {
    // Dont call if there is nothing to show
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
    
    Serial.begin(115200);
    for (int i=0; i<ReqSize; i++) 
      Serial.println(Store[i]);
    Serial.end();
}


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
    
    // Retreive Pin Numbers
    GetPage(0x01, SizeAnalogs, ANALOGS);
    GetPage(0x03, SizeInputs, INPUTS);
    GetPage(0x05, SizeOutputs, OUTPUTS);
    GetPage(0x07, SizePWM, PWM);
    GetPage(0x09, SizeParameters, Parameters);
}


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

