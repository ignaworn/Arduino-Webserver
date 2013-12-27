#ifdef SLAVE

    void ReceiveEvent(int HowMany) {

        // Read Information Type
        byte Type = Wire.read();



        // ---------------------------------------------------
        //           0x01-0x0F   SET REQUEST PAGE
        // ---------------------------------------------------

        if (Type < 0x10 && Type >= 0x00)
          ReqPage = Type;




        // ---------------------------------------------------
        //           0xFF        SEND PRESENSE TO MASTER
        // ---------------------------------------------------

        //else if (Type == 0xFF)
          //if (debug) Serial << "Sending presence to master" << endl;




        // ---------------------------------------------------
        //           0xA0        CHANGE PARAMETER
        // ---------------------------------------------------

        else if (Type == 0xA0) {
           byte ID = Wire.read();
           byte Value = Wire.read();

           // Set Parameter
           SetParameters(ID, Value, false);
        }



        // ---------------------------------------------------
        //           0xA1        CHANGE DIGITAL VALUE
        // ---------------------------------------------------

        else if (Type == 0xA1  ) {
            byte Pin = Wire.read();
            byte Value = Wire.read();

            // Set Digital value
            SetPin(Pin,Value);
        }



        // ---------------------------------------------------
        //           0xA2        CHANGE RGB VALUE
        // ---------------------------------------------------

        else if (Type == 0xA2) {
            byte Pin = Wire.read();
            byte Value = Wire.read();

            // Set PWM Value
            SetPWMPin(Pin,Value);
        }


    }

#endif