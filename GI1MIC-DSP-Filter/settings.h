// Define FT817 to compile code for Teensy 3.2 installed inside FT817 using ADC and DAC for audio.
// Uncomment to use with amplified speaker this enables pullups for filter selection via a button
// and also sets the audio inputs/outputs to use a Teensy Audio Board.
#define FT817
//#define SDCARD   // Undefine to use the SD card on a "Teensy WIZ820IO Ethernet & Micro SD Card Adaptor"


// Replace with your callsign or comment out to stop spoken callsign on boot
#define CALLSIGN  "GI1MIC"


// Uncoment the following to allow speech assisted opperation of the rig. 
// This feature requires a SD card to hold the WAV files
// Checkout http://www.sarcnet.org/files/Rachel/Rachel.pdf for org project details
//#define SPEECHCONFIG
#define METERSPIN 22         //Meters switch pin.Press it to speak the meter values
#define CONFIGPIN 23         //Config switch pin. Press it to speak all menu settings

// Uncomment to enable debug information doe not use with CAT!
// DEBUG mode will also enable the onboard LED to showing tone detection for morse code
#define DEBUG

// Display the calculated filter coeff in .coe format for import into MATLAB - do not use with CAT
// #define SHOWCOEFF

// Number of coefficients. Must be an even number, 4 or higher and no greater than 200
// To compile for a CPU Speed of 24Mhz it will be necessary to reduce this value to a
// value of around 120. Running at 24Mhz reduces power consumption of the Teensy to ~20ma
// Lower values reduce the quality of the filter and will require less CPU effort.
//
// To use more than 200 coefficients you have to change the "#define FIR_MAX_COEFFS" in the
// Teensy audio library source. This is located in 
// C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\Audio\filter_fir.h
#define NUM_COEFFICIENTS 200


// Display performance information - do not use with CAT
// #define SHOWPERF

// Uncomment to enable morse to speech decode. This feature requires a SD card plus WAV files
//#define DECODER
#define DECODER_SERIAL      //allows morse the decoded morse to be viewable via USB. Do not enable with CAT!


// Enable USB -> CAT Interface for rig control. CAT interface should be connected to Serial2
// which uses pin9 for RX and pin 10 for TX. See "cat.h" for more info
// Do not try to use the CAT interface with the follow enabled DEBUG, SHOWCOEFF, SHOWPERF or DECODER_SERIAL
// #define CAT

// Set to match rigs CAT Rate
#define CAT_BAUD    38400

#ifdef FT817
  // You may need to tweak these settings
  #define TONETHRESHOLD     0.02               // Minimum tone level
  #define TONEFREQUENCY   600                 // Frequency of the tone we are trying to detect
  #define TONEDURATION     8                 // Number of tone cycles required for detection like the noise filter in org code
  #define DECODE_COEFF     20                  // Number of coefficients to use for the morse decode filter
  #define MORSE_LED       13                 // Onboard LED used to show morse tone dectection
#else
  // You may need to tweak these settings
  #define TONETHRESHOLD     0.04               // Minimum tone level
  #define TONEFREQUENCY   600                 // Frequency of the tone we are trying to detect
  #define TONEDURATION     16                 // Number of tone cycles required for detection like the noise filter in org code
  #define DECODE_COEFF     20                  // Number of coefficients to use for the morse decode filter
  #define MORSE_LED       15                 // External LED used to show morse tone dectection as onboard conflicts with audio shield
#endif


// This is for a yet to be implemented twin passband filter mode which will require two rotary encoders
// #define TPB

#define ENCODER1a 1                         // Rotary encoder one attached to pins
#define ENCODER1b 2
#define ENCODER1s 3                         // Rotary encoder one sw input pin

#define ENCODER2a 4                         // Rotary encoder two attached to pins
#define ENCODER2b 5
#define ENCODER2s 6                         // Rotary two encoder sw input pin



