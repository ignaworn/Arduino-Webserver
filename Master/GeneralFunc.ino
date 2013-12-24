// ---------------------------------------------------
//                   SET DIGITAL PIN
// ---------------------------------------------------

void SetPin (int Pin, boolean State ) {
    
    // If toggle, invert previous value
    if (State == 0x02)
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
    for (int i=0; i<SizePWM; i++)
      if (PWM[i] == Pin)
        ID = i;
        
    // Set Pin value if power mode is on
    if ( Power() )
      SoftPWMSet(PWM[ID], Value);

    // Store the Value in Memory
    _PWM[ID] = Value;

    // Check if changed OK.
    if ( _PWM[ID] != Value )
      if (debug) Serial << "ERROR SET PWM PIN ";

    if (debug)  Serial << "PWM: " << PWM[ID] << " set to: " << Value << endl;
}


// ---------------------------------------------------
//             SET EEPROM PARAMETERS
// ---------------------------------------------------

void SetParameters(int ID, byte Value) {

    if (debug) Serial << "Changing parameter " << ID << " to value: " << Value;
    
    // If toggle, invert previous value
    if (Value == 0x02)
        Value = Parameters[ID] ? 0x00 : 0x01;
        
    // Set the new parameter value
    if (UseEEPROM) 
      if (EEPROM.read(ID) != Value)
        EEPROM.write(ID,Value);
    
    // Store the value in memory  
    Parameters[ID] = Value;  

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

void UpdateParameters() {

    if (debug) Serial << "Updating Parameters" << endl;

    if (UseEEPROM)
      for (int i=0; i<SizeParameters; i++) {
        Parameters[i] = EEPROM.read(i);

        // Debug: Show params and values
        if (debug) Serial << "Parameter: "<< i <<" - Value: " << Parameters[i] << endl;  
      }
}


// ---------------------------------------------------
//                   STR CLEAR
// ---------------------------------------------------

void StrClear(char *str, char length) {
  // Clear all positions in char array
  for (int i = 0; i < length; i++) 
    str[i] = 0;
}

// ---------------------------------------------------
//                   SET POWER
// ---------------------------------------------------
boolean Power() {
  if ( Parameters[0] )
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
