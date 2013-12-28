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
    // Don't run if SoftPWM is disabled
    #ifndef PWM_CONTROL
        return;
    #endif

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

    // If toggle, invert value
    if (Value == 2)
      Value = Parameters[ID] ? 0x00 : 0x01;

    if (debug) Serial << "Changing parameter " << ID << " to value: " << Value << endl;

    // Store the value in the memory
    Parameters[ID] = Value;

    // Store the ID in a buffer to store it in EEPROM memory
    #ifdef USE_EEPROM
        // Add a position in the cursor
        EEPROM_cursor++;
        // And store the ID
        EEPROM_buffer[EEPROM_cursor] = ID;
    #endif

    // Call setpower
    if (ID == 0)
      SetPower();
}


// ---------------------------------------------------
//             READ PARAMETERS FROM EEPROM
// ---------------------------------------------------

void ReadParameters() {
    // Don't run if EEPROM is disabled
    #ifndef USE_EEPROM
        return;
    #endif

    if (debug) Serial << "Reading parameters from EEPROM memory" << endl;

    for (int i=0; i<SizeParameters; i++) {
        // Debug: Show params and values
        if (debug) Serial << "Parameter: "<< i <<" - Value: " << Parameters[i] << endl;

        Parameters[i] = EEPROM.read(i);
    }
}



// ---------------------------------------------------
//             STORE PARAMETERS IN EEPROM
// ---------------------------------------------------

void StoreParameters() {
    // Don't run if EEPROM is disabled
    #ifndef USE_EEPROM
        return;
    #endif

    // Check if there is something in buffer
    if (EEPROM_cursor > 0) {
        if (debug) Serial << "Saving parameters in EEPROM memory" << endl;

        for (int i = 1; i <= EEPROM_cursor; i++) {
            // Store the buffered parameter in EEPROM memory
            EEPROM.write(EEPROM_buffer[i], Parameters[EEPROM_buffer[i]]);

            // Debug: Show params and values
            if (debug) Serial << "Parameter: "<< EEPROM_buffer[i] <<" - Value: " << Parameters[EEPROM_buffer[i]] << " saved."<< endl;

            // Clear the buffer
            EEPROM_buffer[i] = -1;
        }

        // Set the cursor to zero
        EEPROM_cursor = 0;
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
    // Don't run if SoftPWM is disabled
    #ifndef PWM_CONTROL
        return;
    #endif

    if ( Parameters[0] )
        return true;
    else
        return false;
}

void SetPower() {
    // Don't run if SoftPWM is disabled
    #ifndef PWM_CONTROL
        return;
    #endif

    // Set Power Mode to the value sent
    for (int i=0; i<SizePWM; i++) {
        if ( Power() )
            SoftPWMSet(PWM[i], _PWM[i]);
        else
            SoftPWMSet(ALL, 0);
    }
}
