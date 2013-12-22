class Slave {
  private:
    byte _Address;
    bool _Status;
    bool _Setup;

    void TestConnection();
    void PinData();
    void GetData(byte Size, byte Page, byte Pin[], byte Value[]);
    void GetPage(byte Page, byte ReqSize, byte Store[]);
     
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
    byte Data(char Type, byte ID);
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
byte Slave::Data(char Type, byte ID) {
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
}


void Slave::Update() {
    // Update Analog values
    GetData(SizeAnalogs,   0x01, ANALOGS, _ANALOGS);
    // Update Inputs values
    GetData(SizeInputs,    0x03, INPUTS, _INPUTS);
    // Update Outputs values
    GetData(SizeOutputs,   0x05, OUTPUTS, _OUTPUTS);
    // Update PWM values
    GetData(SizePWM,       0x07, PWM, _PWM);
    // Update Parameters values
    GetData(SizeParameters,0x09, Parameters, _Parameters);
}

void Slave::GetData(byte Size, byte Page, byte Pin[], byte Value[]) {
    // Check if size is null
    if (Size == 0)
      return;

    // Retreive Pin Numbers
    GetPage(Page, Size, Pin);

    // Retreive Pin Values
    GetPage(Page+1, Size, Value);
}

void Slave::SendData(int Mode, byte Pin, byte Value) {
   // Begin I2C Communication
   Wire.beginTransmission(_Address);
   
   byte Type;
   switch(Mode) {
     case 'P': Type = 0xA0;
     case 'M': Type = 0xA1;
     case 'L': Type = 0xA2;
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

