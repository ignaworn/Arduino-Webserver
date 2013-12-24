// ---------------------------------------------------
//           WEB SERVER & CLIENT MANAGEMENT
// ---------------------------------------------------

// size of buffer used to capture HTTP requests
const   int REQ_BUF_SZ = 48;

// buffered HTTP request stored as null terminated string
        char HTTP_req[REQ_BUF_SZ] = {0};

// index into HTTP_req buffer
        char req_index = 0;

void WebServer() {

  boolean currentLineIsBlank = true;
  boolean StartCheck = false;

  while (client.connected()) {

    if (client.available()) {   
      // Read the client data
      char c = client.read();    
      
      // check if the buffer is not full and if the ? char was found
      if ( StartCheck and (req_index < (REQ_BUF_SZ - 1) )  ) {
          // Store the request in a buffer
          HTTP_req[req_index] = c;     
          // The last character of the 8byte data
          if (c == '&')
            ParseReq();
          // continue adding the index
          req_index++;
      }

      // If the ? is read, means that the next is the parameters from HTTP
      if (c == '?') 
          StartCheck = true;      

      // last line of client request is blank and ends with \n
      // respond to client only after last line received
      if (c == '\n' && currentLineIsBlank) {
        // Send XML to the client
        ClientXML();
        // Reset Index
        req_index = 0;
        // Clear buffer
        StrClear(HTTP_req, REQ_BUF_SZ);
        
        break;
      }

      // every line of text received from the client ends with \r\n
      // last character on line of received text
      // starting new line with next character read
      if (c == '\n') 
        currentLineIsBlank = true;

      // a text character was received from client
      else if (c != '\r')      
        currentLineIsBlank = false;

    }
  }
  client.stop(); 
}









// ---------------------------------------------------
//           WEB SERVER - CLIENT XML
// ---------------------------------------------------
void ClientXML() {

    client << "HTTP/1.1 200 OK" << endl
      << "Content-Type: text/xml" << endl
      << "Connection: close" << endl
      << "Access-Control-Allow-Origin: *" << endl
      << endl;

    // Send Data
    client << "<?xml version = \"1.0\" ?>" << endl
           << "<arduino>" << endl;
        
        
   // Send Master Analog Pin
    for (int i=0; i<SizeAnalogs; i++)  
      client << "<a i2c='" << I2CMaster << "' pin='" << ANALOGS[i] << "'>" << lowByte(map(analogRead(ANALOGS[i]),0,1023,0,100)) << "</a>" << endl;

    // Send Master Input Pin
    for (int i=0; i<SizeInputs; i++) 
      client << "<i i2c='" << I2CMaster << "' pin='" << INPUTS[i] << "'>" << digitalRead(INPUTS[i]) << "</i>" << endl;

    // Send Master Output Pin
    for (int i=0; i<SizeOutputs; i++) 
      client << "<o i2c='" << I2CMaster << "' pin='" << OUTPUTS[i] << "'>" << digitalRead(OUTPUTS[i]) << "</o>" << endl;
      
    // Send Master PWM Pin
    for (int i=0; i<SizePWM; i++) 
      client << "<l i2c='" << I2CMaster << "' pin='" << PWM[i] << "'>" << _PWM[i] << "</l>" << endl;
    
    // Send Master Parameters
    for (int i=0; i<SizeParameters; i++) 
      client << "<p i2c='" << I2CMaster << "' id='" << i << "'>" << Parameters[i] << "</p>" << endl;
        

    //If Slave is Online
    if ( Slave.Status() ) {
     
      // Update Slave values
      Slave.Update();
      
      // Fetch I2CSlave_Led_Parameter data
      if (Slave.Size('P') > 0)
      for (int i=0; i < Slave.Size('P'); i++) 
        client << "<p i2c='" << Slave.Address() << "' id='" << Slave.ID('P', i ) << "'>" << Slave.Value('P', i ) << "</p>" << endl;
      
      // Fetch I2CSlave_Led_Analog data
      if (Slave.Size('A') > 0)
      for (int i = 0; i < Slave.Size('A'); i++)  
        client << "<a i2c='" << Slave.Address() << "' pin='" << Slave.ID('A', i ) << "'>" << Slave.Value('A', i ) << "</a>" << endl;
      
      // Fetch I2CSlave_Led_Inputs data
      if (Slave.Size('I') > 0)
      for (int i = 0; i < Slave.Size('I'); i++)  
        client << "<i i2c='" << Slave.Address() << "' pin='" << Slave.ID('I', i ) << "'>" << Slave.Value('I', i ) << "</i>" << endl;
      
      // Fetch I2CSlave_Led_Outputs data
      if (Slave.Size('O') > 0)
      for (int i = 0; i < Slave.Size('O'); i++)  
        client << "<o i2c='" << Slave.Address() << "' pin='" << Slave.ID('O', i ) << "'>" << Slave.Value('O', i ) << "</o>" << endl;
      
      // Fetch I2CSlave_Led_PWM data
      if (Slave.Size('L') > 0)
      for (int i = 0; i < Slave.Size('L'); i++)  
        client << "<l i2c='" << Slave.Address() << "' pin='" << Slave.ID('L', i ) << "'>" << Slave.Value('L', i ) << "</l>" << endl;
    }
    
    // Close XML Tag
    client << "</arduino>" << endl;

}














// ---------------------------------------------------
//           WEB SERVER - PARSE REQUEST
// ---------------------------------------------------
void ParseReq() {
    char _data[14] = { '0', 'x' ,HTTP_req[req_index-7], HTTP_req[req_index-6], ' ', '0', 'x', HTTP_req[req_index-4], HTTP_req[req_index-3], ' ', '0', 'x', HTTP_req[req_index-2], HTTP_req[req_index-1] };
    char * pEnd;

    byte  Addr   = strtol(_data, &pEnd, 0);
    int   Mode   = HTTP_req[req_index-5];
    byte  Pin    = strtol(pEnd, &pEnd, 0);
    byte  Value  = strtol(pEnd, &pEnd, 0);
    
    if (debug) Serial << endl << "Parsing Request" << endl << "Address: " << Addr << " - Mode: " << char(Mode) << " - Pin: " << Pin << " - Value: " << Value << endl;
        
        
    
  // ------------------------------------------------------------------------
  //
  //                         ARDUINO   MASTER
  //
  // ------------------------------------------------------------------------

   if ( Addr == I2CMaster ) {
      
        // ---------------------------------------------------
        //           CHANGE MASTER OUTPUTS
        // ---------------------------------------------------
    
        if (Mode == 'M') {
            if (debug) Serial << "Changing Master Output State" << endl;
    
            // Set pin value
            SetPin(Pin, Value); 
        }
        
        
        
        // ---------------------------------------------------
        //           CHANGE MASTER PWM
        // ---------------------------------------------------

        else if (Mode == 'L') {
            if (debug) Serial << "Changing Master PWM State" << endl;
    
            // Set pin value
            SetPWMPin(Pin, Value); 
        }
    
        
        
        // ---------------------------------------------------
        //           CHANGE MASTER DIGITAL PARAMETERS
        // ---------------------------------------------------
    
        else if (Mode == 'P') {
          if (debug) Serial << "Changing Master Parameter State" << endl;
          
          // Set the new parameter value
          SetParameters(Pin, Value); 
    
          // Update the Parameters value
          UpdateParameters();
        }
    }
        
        
        
      // ------------------------------------------------------------------------
      //
      //                         I2CSlave_LED
      //
      // ------------------------------------------------------------------------

   else if ( Addr == Slave.Address() ) {
     
       // Check if the Slave is connected
       if (!Slave.Status() ) {
         if (debug) Serial << "Attempted to communicate with an offline I2C Slave #" << Addr << endl;
         return;
       }      
       
       if (debug) Serial << "Sending instructions to: SLAVE #" << Addr << " ... ";
       
       // Send data to Slave
       Slave.SendData( Mode, Pin, Value );
       
       if (debug) Serial << "Success" << endl;
    }
}
