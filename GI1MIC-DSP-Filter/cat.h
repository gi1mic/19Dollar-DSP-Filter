#ifndef CAT_H
#define CAT_H

// We may as well use the USB port to connect to the rigs CAT interface
// This code uses Serial2 which are pins 9 RX and 10 TX on a Teensy 3.2

#define CAT_BAUD    38400                       // Set to match rigs CAT Rate


void initCAT(){
  #ifdef CAT
    Serial2.begin(CAT_BAUD);
  #endif  
}


void serviceCAT() {
  #ifdef CAT

    // USB->CAT
    int c = Serial.read();                    // read from USB
    if (c != -1) {                            // got anything?
        Serial2.write(c);                   // Yes write char to UART (CAT)
    }

    // CAT -> USB
    c = Serial2.read();                       // read from UART
    if (c != -1) {                            // got anything?
      Serial.write(c);                        // Yes write ro USB
    }
  #endif  
}
#endif

