// ---------------------------------------------------
//           WEB SERVER & CLIENT MANAGEMENT
// ---------------------------------------------------
#ifdef WEBSERVER

    // size of buffer used to capture HTTP requests
    const   int REQ_BUF_SZ = 48;

    // buffered HTTP request stored as null terminated string
            char HTTP_req[REQ_BUF_SZ] = {0};

            // index into HTTP_req buffer
            char req_index = 0;

#endif



void WebServer() {
    // Do not run if WEBSERVER Flag is not active
    #ifdef WEBSERVER
        // Get active clients
        client = server.available();

        // Dont execute it if there is no client
        if (!client)
            return;

        boolean currentLineIsBlank = true;
        boolean StartCheck = false;

        while (client.connected()) {

            if (client.available()) {
                // Read the client data
                char c = client.read();

                // check if the buffer is not full and if the ? char was found
                if ( StartCheck and (req_index < (REQ_BUF_SZ - 1) )  ) {
                    // Store the request in a buffer
                    HTTP_req[req_index] = c;
                    // The last character of the 8byte data
                    if (c == '&')
                        ParseReq();
                    // continue adding the index
                    req_index++;
                }

                // If the ? is read, means that the next is the parameters from HTTP
                if (c == '?')
                    StartCheck = true;

                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // Send XML to the client
                    ClientXML();
                    // Reset Index
                    req_index = 0;
                    // Clear buffer
                    StrClear(HTTP_req, REQ_BUF_SZ);

                    break;
                }

                // every line of text received from the client ends with \r\n
                // last character on line of received text
                // starting new line with next character read
                if (c == '\n')
                    currentLineIsBlank = true;

                // a text character was received from client
                else if (c != '\r')
                    currentLineIsBlank = false;

            }
        }
        client.stop();
    #endif
}







// ---------------------------------------------------
//           WEB SERVER - CLIENT XML
// ---------------------------------------------------
void ClientXML() {
    // Do not run if WEBSERVER Flag is not active
    #ifdef WEBSERVER

        client << "HTTP/1.1 200 OK" << endl
                << "Content-Type: text/xml" << endl
                << "Connection: close" << endl
                << "Access-Control-Allow-Origin: *" << endl
                << endl;

        // Send Data
        client << "<?xml version = \"1.0\" ?>" << endl
                << "<arduino>" << endl;


        // Send Master Analog Pin
        for (int i=0; i<SizeAnalogs; i++)
            client << "<a i2c='" << TWIAddr << "' pin='" << ANALOGS[i] << "'>" << lowByte(map(analogRead(ANALOGS[i]),0,1023,0,100)) << "</a>" << endl;

        // Send Master Input Pin
        for (int i=0; i<SizeInputs; i++)
            client << "<i i2c='" << TWIAddr << "' pin='" << INPUTS[i] << "'>" << digitalRead(INPUTS[i]) << "</i>" << endl;

        // Send Master Output Pin
        for (int i=0; i<SizeOutputs; i++)
            client << "<o i2c='" << TWIAddr << "' pin='" << OUTPUTS[i] << "'>" << digitalRead(OUTPUTS[i]) << "</o>" << endl;

        // Send Master PWM Pin
        #ifdef PWM_CONTROL
            for (int i=0; i<SizePWM; i++)
                client << "<l i2c='" << TWIAddr << "' pin='" << PWM[i] << "'>" << _PWM[i] << "</l>" << endl;
        #endif

        // Send Master Parameters
        for (int i=0; i<SizeParameters; i++)
            client << "<p i2c='" << TWIAddr << "' id='" << i << "'>" << Parameters[i] << "</p>" << endl;


        //If Slave is Online
        #ifdef SLAVE_LED
            if ( Slave.Status() ) {

                // Update Slave values
                Slave.Update();

                // Fetch I2CSlave_Led_Parameter data
                if (Slave.Size('P') > 0)
                    for (int i=0; i < Slave.Size('P'); i++)
                        client << "<p i2c='" << Slave.Address() << "' id='" << Slave.ID('P', i ) << "'>" << Slave.Value('P', i ) << "</p>" << endl;

                // Fetch I2CSlave_Led_Analog data
                if (Slave.Size('A') > 0)
                    for (int i = 0; i < Slave.Size('A'); i++)
                        client << "<a i2c='" << Slave.Address() << "' pin='" << Slave.ID('A', i ) << "'>" << Slave.Value('A', i ) << "</a>" << endl;

                // Fetch I2CSlave_Led_Inputs data
                if (Slave.Size('I') > 0)
                    for (int i = 0; i < Slave.Size('I'); i++)
                        client << "<i i2c='" << Slave.Address() << "' pin='" << Slave.ID('I', i ) << "'>" << Slave.Value('I', i ) << "</i>" << endl;

                // Fetch I2CSlave_Led_Outputs data
                if (Slave.Size('O') > 0)
                    for (int i = 0; i < Slave.Size('O'); i++)
                        client << "<o i2c='" << Slave.Address() << "' pin='" << Slave.ID('O', i ) << "'>" << Slave.Value('O', i ) << "</o>" << endl;

                // Fetch I2CSlave_Led_PWM data
                #ifdef PWM_CONTROL
                    if (Slave.Size('L') > 0)
                        for (int i = 0; i < Slave.Size('L'); i++)
                            client << "<l i2c='" << Slave.Address() << "' pin='" << Slave.ID('L', i ) << "'>" << Slave.Value('L', i ) << "</l>" << endl;
                #endif
            }
        #endif

        // Close XML Tag
        client << "</arduino>" << endl;
    #endif

}









// ---------------------------------------------------
//           WEB SERVER - PARSE REQUEST
// ---------------------------------------------------
void ParseReq() {
    // Do not run if WEBSERVER Flag is not active
    #ifdef WEBSERVER
        char _data[14] = { '0', 'x' ,HTTP_req[req_index-7], HTTP_req[req_index-6], ' ', '0', 'x', HTTP_req[req_index-4], HTTP_req[req_index-3], ' ', '0', 'x', HTTP_req[req_index-2], HTTP_req[req_index-1] };
        char * pEnd;

        byte  Addr   = strtol(_data, &pEnd, 0);
        int   Mode   = HTTP_req[req_index-5];
        byte  Pin    = strtol(pEnd, &pEnd, 0);
        byte  Value  = strtol(pEnd, &pEnd, 0);

        if (debug) Serial << endl << "New Request: Address: " << Addr << " - Mode: " << char(Mode) << " - Pin: " << Pin << " - Value: " << Value << endl;



        // ------------------------------------------------------------------------
        //
        //                         ARDUINO   MASTER
        //
        // ------------------------------------------------------------------------

        if ( Addr == TWIAddr ) {
            if (debug) Serial << "*Master ";

            // ---------------------------------------------------
            //           CHANGE MASTER OUTPUTS
            // ---------------------------------------------------

            if (Mode == 'M') {

                // Set pin value
                SetPin(Pin, Value);
            }



            // ---------------------------------------------------
            //           CHANGE MASTER PWM
            // ---------------------------------------------------
            #ifdef PWM_CONTROL
                else if (Mode == 'L') {

                    // Set pin value
                    SetPWMPin(Pin, Value);
                }
            #endif


            // ---------------------------------------------------
            //           CHANGE MASTER DIGITAL PARAMETERS
            // ---------------------------------------------------

            else if (Mode == 'P') {

                // Set the new parameter value
                SetParameters(Pin, Value);
            }
        }



        // ------------------------------------------------------------------------
        //
        //                         I2CSlave_LED
        //
        // ------------------------------------------------------------------------
        #ifdef SLAVE_LED
            else if ( Addr == Slave.Address() ) {

                // Check if the Slave is connected
                if (!Slave.Status() ) {
                    if (debug) Serial << "*Attempted to communicate with an offline I2C Slave #" << Addr << endl;
                    return;
                }

                if (debug) Serial << "*Sending instructions to SLAVE #" << Addr << ": ";

                // Send data to Slave
                Slave.SendData( Mode, Pin, Value );

                if (debug) Serial << "Success" << endl;
            }
        #endif

        // ---------------------------------------------------
        //              UNKNOWN SLAVE DEVICE
        // ---------------------------------------------------
        else {
            if (debug) Serial << "*Attempted to communicate with an unknown I2C Slave #" << Addr << endl;
        }
    #endif
}