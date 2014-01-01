#if DEVICE_NAME == "Master"

    void ReceiveEvent(int HowMany)  {

        // Read Information Type
        byte Type = Wire.read();
        byte Addr = Wire.read();

        // Receive a signal sent from a Slave to restart the Master
        if (Type == 0xFF) {

            // Start SLAVE_LED
            #ifdef SLAVE_LED
                if (Addr == Slave.Address()) {
                    if (debug) Serial << "Received presence from SLAVE #" << Slave.Address() << ": ";
                    Slave.Setup();
                    if (Slave.Status() ) {
                      if (debug) Serial << "Success" << endl;
                    }
                    else
                      if (debug) Serial << "Offline" << endl;
                }
            #endif


        }


    }

    void RequestEvent() {

    }

#endif