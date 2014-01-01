#if DEVICE_NAME == 'Slave Led'

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
           SetParameters(ID, Value);
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

    void RequestEvent() {

        // ---------------------------------------------------
        //           0x09        SEND PIN SIZE TO MASTER
        // ---------------------------------------------------

        if (ReqPage == 0x00) {

          // Create the buffer for sending multiple byte in a request event
          byte buf [5];
          buf[0] = SizeAnalogs;
          buf[1] = SizeInputs;
          buf[2] = SizeOutputs;
          buf[3] = SizePWM;
          buf[4] = SizeParameters;

          // Send data
          Wire.write(buf, 5);
        }


        // ---------------------------------------------------
        //         ReqPage 1-2    ANALOG ID AND VALUE
        // ---------------------------------------------------

        else if ( ReqPage == 0x01 )
            Wire.write(ANALOGS, SizeAnalogs);
        else if ( ReqPage == 0x02 )
            SendValue('a', SizeAnalogs);

        // ---------------------------------------------------
        //         ReqPage 3-4    INPUTS ID AND VALUE
        // ---------------------------------------------------

        else if ( ReqPage == 0x03)
            Wire.write(INPUTS, SizeInputs);
        else if ( ReqPage == 0x04 )
            SendValue('i', SizeInputs);

        // ---------------------------------------------------
        //         ReqPage 5-6    OUTPUTS ID AND VALUE
        // ---------------------------------------------------

        else if ( ReqPage == 0x05 )
            Wire.write(OUTPUTS, SizeOutputs);
        else if ( ReqPage == 0x06 )
            SendValue('o', SizeOutputs);

        // ---------------------------------------------------
        //         ReqPage 7-8    PWM ID AND VALUE
        // ---------------------------------------------------
        else if ( ReqPage == 0x07 )
            Wire.write(PWM, SizePWM);
        else if ( ReqPage == 0x08 )
            SendValue('l', SizePWM);

        // ---------------------------------------------------
        //         ReqPage 9-A   PARAMETERS ID AND VALUE
        // ---------------------------------------------------

        else if ( ReqPage == 0x09 )
            SendValue('P', SizeParameters);
        else if ( ReqPage == 0x0A )
            SendValue('p', SizeParameters);

    }


    void SendValue(char Type, byte Size) {
        // Limit buffer to 32 bytes
        if (Size > 32)
          Size = 32;

        // Create the buffer for sending multiple byte in a request event
        byte buf[Size];

        // Send 16 PWM Values per page
        for (int i=0; i<Size; i++) {
          switch(Type) {
              case 'a':            buf[i] = map(analogRead(ANALOGS[i]),0,1023,0,100);   break;
              case 'i':            buf[i] = digitalRead(INPUTS[i]);   break;
              case 'o':            buf[i] = digitalRead(OUTPUTS[i]);   break;
              case 'l':            buf[i] = _PWM[i];   break;
              case 'P':            buf[i] = i;   break;
              case 'p':            buf[i] = Parameters[i];  break;
           }
        }

        // Send data
        Wire.write(buf, Size);
    }

#endif