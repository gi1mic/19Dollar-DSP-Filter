#ifndef CAT_H
#define CAT_H

// Since we have a spare serial port we may as well use the USB port to
// connect to the rigs CAT interface.
// This code uses Serial2 which are pins 9 RX and 10 TX on a Teensy 3.2


// Function prototypes
void initCAT();
void serviceCAT();

#endif

