/*
 * The GI1MIC $20 Dollar DSP Audio Filter
 * 
 * Visit https://gi1mic.github.io/ for information on how to use this code.
 * 
 * Email via GI1MIC at outlook.com
 * 
 * Modify "settings.h" to suit your needs 
 * 
 *  
 * This project is for installing into a Yaesu FT-817 but easily be adapted 
 * for many other radios.
 * 
 * What does it do?
 *    -  Act as multiple DSP filters based on hi pass, lo pass, band pass or band stop (filter points can be defined in Hz within the code)
 *    -  It uses voice messages to describe which filter has been selected
 *    -  It is a USB to CAT interface
 *    -  It emulates a USB sound card for receiving rig audio on a computer
 *    -  Performs Morse to voice decode and Morse to USB serial decode
 *    -  It can speak the radio config to assist the visually impaired (FT817 only)
 * 
 * 
 * Version 2.2
 *  Some code updates
 *  Added support for audio files held on a SD card attached via a Teensy WIZ820IO addon board
 *  New speech files
 *  Includes tools to regenerate the speech files. This should make it easy to convert to other languages
 *  Adds speech readout of rig controls - Requires the CAT interface to be connected
 * 
 * Version 2.1a
 *  Added a FIFO to the morse 2 speech decode to improve decoding when using SD card playback
 *  
 * Version 2.1
 *  Minor bug fixes
 *  Configuration options now held in "settings.h"
 *  Added option to say callsign on boot
 *  Added Morse to speech decode based on WB7FHC code. This works OK for locally generated Morse but not ready for primetime. 
 *        If you compile for the Teensy Audio board it will use WAV files on the card for playback.
 *        Otherwise it uses Texas Instruments speech synthesis architecture (Linear Predictive Coding) i.e. Speak & Spell voice synthesis
 *        Suspect the latter is taking too much time for Morse decode to work reliably
 *  Verified CAT control - dropped USB serial speed to 38400 to provide compatibility with some rig control software

 * Version 2.0
 *  Moved to using real-time calculated FIR filters based on Bob Maling code
 *  Added support for Teensy audio board for simplified integration with an amplified speaker this uses 
 *      a SD card fitted to the audio board to hold the speech files instead of the precompiled audio.
 *  Added preliminary support for USB -> CAT interface for rig control
 *  Added support for USB RX audio (Make sure you select Audio/Midi/Serial on the tools menu to compile without errors).
 *  USB TX may be added but it will require a Teensy 3.5/3.6 as they have dual DAC's (OK, it may be possible on the 3.2 using PWM...)
 * 
 * Version 1.2
 *  This update changes the Morse coefficients to an improved set based on a Windowed hamming FIR filter. 
 *  See the https://gi1mic.github.io/ for a MATLAB simulation of the filter. 
 *  Note: this is a very sharp filter - you may want to generate your own!
 * 
 * Version 1.1
 *  Code Tidy-up 
 *  Smaller (lower quality) voice prompts
 *  Debug and FT817 mode controlled via defines
 * 
 * Version 1.0 
 *  Initial release which had no version number
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Encoder.h>
#include <Bounce.h>
#include "settings.h"
#include "cat.h"
#include "dynamicFilters.h"
#include "dspfilter.h"
#include "morseDecode.h"
#include "speech.h"
#include "configSpeak.h"

bool toneDet = false;

//---------------------------------------------------------------
void setup() {
  Serial.begin(38400);
  delay(500);

  #ifdef DECODER
      morseInit();
  #endif
  
  AudioMemory(8);

  #ifdef SDCARD
      SPI.setMOSI(SDCARD_MOSI_PIN);
      SPI.setSCK(SDCARD_SCK_PIN);
  #ifdef DEBUG
        boolean status = card.init(SPI_FULL_SPEED, SDCARD_CS_PIN);
        if (status) {
            Serial.println("SD card is connected :-)");
        } else {
            Serial.println("SD card is not connected or unusable :-(");
            return;
        }
  #endif
      if (!(SD.begin(SDCARD_CS_PIN))) {
          // stop here, but print a message repetitively
          while (1) {
              Serial.println("Unable to access the SD card");
              delay(500);
          };
      };
  #endif

  #ifdef FT817
      audioOutput.analogReference(INTERNAL);
  #else
      // Down mix stereo to mono on input
      monoMixer.gain(0, 0.5);                 // Left
      monoMixer.gain(1, 0.5);                 // Right
      monoMixer.gain(2, 0.0);                 // Not used
      monoMixer.gain(3, 0.0);                 // Not used
      
      audioControl.enable();                  // Start the SGTL5000
      audioControl.inputSelect(AUDIO_INPUT_LINEIN);
      audioControl.lineInLevel(5);            // Set input level to 1.33V
      audioControl.volume(0.8);               // Set headset volume level

  #endif

    // Initialize the mixer (Normally the totals should add to 1.0 max)
    audioOutputMixer.gain(0, 0.9);    // Filtered radio audio
    audioOutputMixer.gain(1, 0.9);    // Audio Prompt (we get away with this since this input is not normally active)
    audioOutputMixer.gain(2, 0.9);    // Audio Prompt SD
    audioOutputMixer.gain(3, 0);      // PC Audio Input (Not used for now)
  
    // Start in passthru mode
    audioFilter(fir_active1, 
              NUM_COEFFICIENTS, 
              ID_BANDPASS, 
              W_HAMMING, 
              60.0, 
              20000.0 );
    myFilter.begin(fir_active1, NUM_COEFFICIENTS);
    //  myFilter.begin(FIR_PASSTHRU, 0);                       // passthru mode
              
    #ifdef FT817
        pinMode(SELECT_PIN, INPUT);
    #else
        pinMode(SELECT_PIN, INPUT_PULLUP);
    #endif

   // Morse detect stuff
   toneDetect.frequency(TONEFREQUENCY, TONEDURATION);
   toneDetect.threshold(TONETHRESHOLD);
   
    initCAT();
    initConfigSpeak();

    #ifdef CALLSIGN
        muteRadio();                                   // Mute while reading callsign
        speakChars(CALLSIGN);
        unMuteRadio();                                 // Reset the ausio level      
    #endif
    
    #ifdef DEBUG
        Serial.println("setup done");
    #endif
}


float toneState;
//---------------------------------------------------------------
void loop()
{

  filterSelect.update();                            // read the button state

  if (filterSelect.fallingEdge()) {                 // On button press - switch to next filter in the list
    filterIndex++;
    if (filterIndex >= (sizeof(filterList) / sizeof(filter)))
          filterIndex = 0;                          // if end of array, then loop to begining

    if (filterList[filterIndex].filterID == MORSEDECODE) {           
      lowerRadio();                                  // Lower the radio volume in Morse Decode mode to make it easier to hear
    }else {
      unMuteRadio();                                 // Reset the ausio level      
    }
    
    audioFilter(  fir_active1, 
                  filterList[filterIndex].coeff, 
                  filterList[filterIndex].filterType, 
                  filterList[filterIndex].window, 
                  filterList[filterIndex].freqLow, 
                  filterList[filterIndex].freqHigh );
    #ifdef SDCARD
        speakSD(filterList[filterIndex].filterName[0].c_str());
    #else 
        speak(filterList[filterIndex].audioSample);
    #endif
  }
  // Rotary Encoder - move the centerpoint of the filter
  long newKnob;
  newKnob = knob.read();
  if (newKnob != kPosition) {
    int diff = (newKnob - kPosition) * 4;
    #ifdef DEBUG
        Serial.print("Knob = ");
        Serial.print(diff);
        Serial.println();
    #endif
    filterList[filterIndex].freqLow  += diff;
    filterList[filterIndex].freqHigh += diff;
    audioFilter(fir_active1, 
                NUM_COEFFICIENTS, 
                filterList[filterIndex].filterType, 
                filterList[filterIndex].window, 
                filterList[filterIndex].freqLow, 
                filterList[filterIndex].freqHigh );
    kPosition = newKnob;
  }


  #ifdef SHOWPERF
    // print debug and resource usage
    if (millis() - last_time >= 2500) {
      if ( audioInputPeak.available() ) {
          Serial.print("Peak input level = ");
          Serial.print(audioInputPeak.readPeakToPeak());
          Serial.print(", ");    
      }
      Serial.print("Proc = ");
      Serial.print(AudioProcessorUsage());
      Serial.print(" (");    
      Serial.print(AudioProcessorUsageMax());
      Serial.print("),  Mem = ");
      Serial.print(AudioMemoryUsage());
      Serial.print(" (");    
      Serial.print(AudioMemoryUsageMax());
      Serial.println(")");
      last_time = millis();
    }
  #endif
  serviceCAT();
  serviceSpeech();
  serviceConfigSpeak();

  #ifdef DECODER
      if (filterList[filterIndex].filterID == MORSEDECODE) {    // Service morse decoding
           if (toneDetect) {
              morseKeyDown();
            }else {
              morseKeyUp();
            };
      };
    #endif
}





