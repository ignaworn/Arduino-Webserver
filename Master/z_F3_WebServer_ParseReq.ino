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
