#include <Audio.h>
#include "settings.h"
#include "cat.h"


void initCAT(){
  #ifdef CAT
    Serial2.begin(CAT_BAUD, SERIAL_8N2);
  #endif  
}


void serviceCAT() {
  #ifdef CAT
    // USB->CAT
    int c = Serial.read();                  // read from USB
    if (c != -1) {                          // got anything?
        Serial2.write(c);                   // Yes write char to UART (CAT)
    }

    // CAT -> USB
    c = Serial2.read();                       // read from UART
    if (c != -1) {                            // got anything?
      Serial.write(c);                        // Yes write ro USB
    }
  #endif  
}
