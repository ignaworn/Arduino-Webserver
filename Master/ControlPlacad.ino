// ---------------------------------------------------
//                   CONTROL PLACARD
// ---------------------------------------------------

// Estado del sensor A - actual
        boolean SA1;
        
// Estado del sensor B - actual 
        boolean SB1;
        
// Control Placard Timer 
unsigned long CPTimer           = 0;
        
// Control Placard Timeout (mseg)
const   long CPTimeout          = 300000;

// Control Placard Input
const   int CPI[2]              = {2,3};

// Control Placard Output
const   int CPO[2]              = {5,6};

// Timer Active
const   byte* CPTimer_Active = &Parameters[1];
  
  
void ControlPlacard() {
    
  boolean SA2 = SA1;
  boolean SB2 = SB1;
  SA1 = digitalRead(CPI[0]); 
  SB1 = digitalRead(CPI[1]);
  
  if ( *CPTimer_Active and CPTimer > 0 and (millis() - CPTimer) >= CPTimeout )   {
      if(debug) Serial << endl << "Control Placard - Timeout" << endl;
      SetPin (CPO[0], LOW);
      SetPin (CPO[1], LOW);
  }
  
  if ( SA1 != SA2) {
    if ( SA1 ) {
      if(debug) Serial << endl << "Control Placard - Door closed" << endl;
      SetPin (CPO[0], LOW);
      SetPin (CPO[1], LOW);
    }
    else {
      if(debug) Serial << endl << "Control Placard - Door opened" << endl;
      SetPin(CPO[0], HIGH);
      if (*CPTimer_Active) {
        if(debug) Serial << "Control Placard - Started Timer" << endl;
        CPTimer = millis();
      }
    }
  }
  
  if (*CPTimer_Active and CPTimer > 0 and !digitalRead(CPO[0])) {
      if(debug) Serial << "Control Placard - Timer reset" << endl;
      CPTimer = 0;
  }
      
  if ( SB1 != SB2 ) {
    if ( SB1 and !SA1 and digitalRead(CPO[0]) and !digitalRead(CPO[1]) ) {
      if(debug) Serial << endl << "Control Placard - Door fully opened" << endl;
      SetPin(CPO[1], HIGH);
    }
  }  

}
