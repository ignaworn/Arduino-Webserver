// ---------------------------------------------------
//                   SET DIGITAL PIN
// ---------------------------------------------------

void SetPin (int Pin, boolean State ) {
    // If toggle, invert value
    if (State == 2)
      State = digitalRead(Pin) ? LOW : HIGH;
      
    // Set Pin value
    digitalWrite(Pin, State);

    // Check if changed OK.
    if ( digitalRead(Pin) != State )
        if (debug)  Serial << "ERROR SETPIN ";

    if (debug)  Serial << "Pin: " << Pin << " set to: " << State << endl;
}


// ---------------------------------------------------
//                   SET PWM PIN
// ---------------------------------------------------

void SetPWMPin (int Pin, byte Value ) {
    int ID;
    
    // Find Pin ID
    for (int i=0; i<SizePWM; i++)
      if (Pin == PWM[i])
        ID = i;
    
    // Set Pin value if power mode is on
    if ( Power() )
      SoftPWMSet(PWM[ID], Value);

    // Store the Value in Memory
    _PWM[ID] = Value;

    if (debug)  Serial << "PWM: " << PWM[ID] << " set to: " << Value << endl;
}


// ---------------------------------------------------
//             SET EEPROM PARAMETERS
// ---------------------------------------------------

void SetParameters(int ID, byte Value, boolean noDelay) {

    if (debug) Serial << "Changing parameter " << ID << " to value: " << Value;
    
    // If toggle, invert value
    if (Value == 2)
      Value = Parameter[ID] ? 0x00 : 0x01;
      
    // Set the new parameter value
    if (UseEEPROM && noDelay) 
      if (EEPROM.read(ID) != Value)
        EEPROM.write(ID,Value);
    
    // Store the value in the memory  
    Parameter[ID] = Value;  

    // Check if the change was made succesfully
    if ( UseEEPROM )
      if (EEPROM.read(ID) == Value) 
        if (debug) Serial << ", done successfully" << endl;   
       
    // Call setpower
    if (ID == 0)
      SetPower(); 
}


// ---------------------------------------------------
//             UPDATE EEPROM PARAMETERS
// ---------------------------------------------------

void UpdateParameters(boolean Force) {

    if (debug) Serial << "Updating Parameters" << endl;

    if (UseEEPROM)
      for (int i=0; i<SizeParameter; i++) {
        
        if (!Force)
          Parameter[i] = EEPROM.read(i);
        if (Force) {
          SetParameters(i,Parameter[i], true);
        }  
        // Debug: Show params and values
        if (debug) Serial << "Parameter: "<< i <<" - Value: " << Parameter[i] << endl;  
      }
}


// ---------------------------------------------------
//                   SET POWER
// ---------------------------------------------------
boolean Power() {
  if ( Parameter[0] )
    return true;
  else
    return false;
}

void SetPower() {
    // Set Power Mode to the value sent
    
    for (int i=0; i<SizePWM; i++) {
        if ( Power() ) 
            SoftPWMSet(PWM[i], _PWM[i]);
        else  
            SoftPWMSet(ALL, 0);
    }
}




