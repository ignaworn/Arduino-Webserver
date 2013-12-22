// ---------------------------------------------------
//           WEB SERVER - CLIENT XML
// ---------------------------------------------------
void ClientXML() {

    client  << "HTTP/1.1 200 OK" << endl
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
    client << "<p i2c='" << I2CMaster << "' id='" << i << "' txt='" << TextParameters(I2CMaster, i) << "'>" << Parameters[i] << "</p>" << endl;
        

    //If Slave is Online
    if ( Slave.Status() ) {
     
      // Update Slave values
      Slave.Update();
      
      // Fetch I2CSlave_Led_Parameter data
      if (Slave.Size('P') > 0)
      for (int i=0; i < Slave.Size('P'); i++) 
        client << "<p i2c='" << Slave.Address() << "' id='" << Slave.ID('P', i ) << "' txt='" << TextParameters(Slave.Address(), Slave.ID('P', i )) << "'>" << Slave.Data('P', i ) << "</p>" << endl;
      
      // Fetch I2CSlave_Led_Analog data
      if (Slave.Size('A') > 0)
      for (int i = 0; i < Slave.Size('A'); i++)  
        client << "<a i2c='" << Slave.Address() << "' pin='" << Slave.ID('A', i ) << "'>" << Slave.Data('A', i ) << "</a>" << endl;
      
      // Fetch I2CSlave_Led_Inputs data
      if (Slave.Size('I') > 0)
      for (int i = 0; i < Slave.Size('I'); i++)  
        client << "<i i2c='" << Slave.Address() << "' pin='" << Slave.ID('I', i ) << "'>" << Slave.Data('I', i ) << "</i>" << endl;
      
      // Fetch I2CSlave_Led_Outputs data
      if (Slave.Size('O') > 0)
      for (int i = 0; i < Slave.Size('O'); i++)  
        client << "<o i2c='" << Slave.Address() << "' pin='" << Slave.ID('O', i ) << "'>" << Slave.Data('O', i ) << "</o>" << endl;
      
      // Fetch I2CSlave_Led_PWM data
      if (Slave.Size('L') > 0)
      for (int i = 0; i < Slave.Size('L'); i++)  
        client << "<l i2c='" << Slave.Address() << "' pin='" << Slave.ID('L', i ) << "'>" << Slave.Data('L', i ) << "</l>" << endl;
    }
    
    // Close XML Tag
    client << "</arduino>" << endl;

}
