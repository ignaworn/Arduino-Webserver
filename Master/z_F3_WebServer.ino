// ---------------------------------------------------
//           WEB SERVER & CLIENT MANAGEMENT
// ---------------------------------------------------

// size of buffer used to capture HTTP requests
const   int REQ_BUF_SZ = 48;

// buffered HTTP request stored as null terminated string
        char HTTP_req[REQ_BUF_SZ] = {0};

// index into HTTP_req buffer
        char req_index = 0;

void WebServer() {

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
}
