#if DEVICE_NAME == 'Master'

    void ReceiveEvent(int HowMany)  {

        // Read Information Type
        byte Type = Wire.read();
        byte Addr = Wire.read();

        // Receive a signal sent from a Slave to startup
        if (Type == 0xFF) {
            // Store the Address in a buffer
            SLAVE_cursor++;
            SLAVE_buffer[SLAVE_cursor] = Addr;
        }


    }

    void RequestEvent() {

    }

#endif