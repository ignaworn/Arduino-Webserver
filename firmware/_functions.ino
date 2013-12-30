// Create PWM Power Pointer
    const   byte* PWM_Power = &Parameters[0];

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
    // Do not run if PWM_CONTROL Flag is not active
    #ifdef PWM_CONTROL

        int ID;
        for (int i=0; i<SizePWM; i++)
          if (PWM[i] == Pin)
            ID = i;

        // Set Pin value if power mode is on
        if ( *PWM_Power )
          SoftPWMSet(PWM[ID], Value);

        // Store the Value in Memory
        _PWM[ID] = Value;

        // Check if changed OK.
        if ( _PWM[ID] != Value )
          if (debug) Serial << "ERROR SET PWM PIN ";

        if (debug)  Serial << "PWM: " << PWM[ID] << " set to: " << Value << endl;
    #endif
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
    #ifdef PWM_CONTROL
        if (ID == 0)
          SetPower();
    #endif
}


// ---------------------------------------------------
//             READ PARAMETERS FROM EEPROM
// ---------------------------------------------------

void ReadParameters() {
    // Do not run if USE_EEPROM Flag is not active
    #ifdef USE_EEPROM
        if (debug) Serial << "Reading parameters from EEPROM memory" << endl;

        for (int i=0; i<SizeParameters; i++) {
            // Debug: Show params and values
            if (debug) Serial << "Parameter: "<< i <<" - Value: " << Parameters[i] << endl;

            Parameters[i] = EEPROM.read(i);
        }
    #endif
}


// ---------------------------------------------------
//             STORE PARAMETERS IN EEPROM
// ---------------------------------------------------

void StoreParameters() {
    // Do not run if USE_EEPROM Flag is not active
    #ifdef USE_EEPROM
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
    #endif
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

void SetPower() {
    // Do not run if PWM_CONTROL Flag is not active
    #ifdef PWM_CONTROL
        // Set Power Mode to the value sent
        for (int i=0; i<SizePWM; i++) {
            if ( *PWM_Power )
                SoftPWMSet(PWM[i], _PWM[i]);
            else
                SoftPWMSet(ALL, 0);
        }
    #endif
}


// ---------------------------------------------------
//                  NTP FUNCTIONS
// ---------------------------------------------------
#ifdef NTP_TIME

    unsigned long getNtpTime() {

        sendNTPpacket(NTP_Server); // send an NTP packet to a time server

        // wait to see if a reply is available
        delay(1000);
        if ( Udp.parsePacket() ) {
            // We've received a packet, read the data from it
            Udp.read(NTP_buffer ,NTP_PACKET_SIZE);  // read the packet into the buffer

            //the timestamp starts at byte 40 of the received packet and is four bytes,
            // or two words, long. First, esxtract the two words:

            unsigned long highWord = word(NTP_buffer[40], NTP_buffer[41]);
            unsigned long lowWord = word(NTP_buffer[42], NTP_buffer[43]);
            // combine the four bytes (two words) into a long integer
            // this is NTP time (seconds since Jan 1 1900):
            unsigned long secsSince1900 = highWord << 16 | lowWord;

            // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
            const unsigned long seventyYears = 2208988800UL;
            // subtract seventy years:
            unsigned long epoch = secsSince1900 - seventyYears;

            return epoch - NTP_TimeZone;
        }
        return 0;
    }

    // send an NTP request to the time server at the given address
    unsigned long sendNTPpacket(IPAddress& address)
    {
      // set all bytes in the buffer to 0
      memset(NTP_buffer, 0, NTP_PACKET_SIZE);
      // Initialize values needed to form NTP request
      // (see URL above for details on the packets)
      NTP_buffer[0] = 0b11100011;   // LI, Version, Mode
      NTP_buffer[1] = 0;     // Stratum, or type of clock
      NTP_buffer[2] = 6;     // Polling Interval
      NTP_buffer[3] = 0xEC;  // Peer Clock Precision
      // 8 bytes of zero for Root Delay & Root Dispersion
      NTP_buffer[12]  = 49;
      NTP_buffer[13]  = 0x4E;
      NTP_buffer[14]  = 49;
      NTP_buffer[15]  = 52;

      // all NTP fields have been given values, now
      // you can send a packet requesting a timestamp:
      Udp.beginPacket(address, 123); //NTP requests are to port 123
      Udp.write(NTP_buffer,NTP_PACKET_SIZE);
      Udp.endPacket();
    }


#endif