#include <Audio.h>
#include "settings.h"
#include "speech.h"

#ifdef FT817
  #include <Talkie.h>
  Talkie voice;
  extern AudioPlayMemory   audioPrompt;
  extern AudioMixer4       audioOutputMixer;
#else
  extern AudioPlaySdWav    audioPromptSD;
  extern AudioPlayMemory   audioPrompt;
  extern AudioMixer4       audioOutputMixer;  
#endif

/*
 * This is the Morse letter to speech lookup table
 * it is in the same order as the "mySet[]" array
 * ##TEMNAIOGKDWRUS##QZYCXBJP#L#FVH09#8###7#####/-61#######2###3#45
 */
//const uint8_t* speechLookup[] = {
//            NULL,       NULL,     spT,    spE,    spM,
//            spN,        spA,      spI,    spO,    spG,
//            spK,        spD,      spW,    spR,    spU,
//            spS,        NULL,     NULL,   spQ,    spZ,
//            spY,        spC,      spX,    spB,    spJ,
//            spP,        NULL,     spL,    NULL,   spF,
//            spV,        spH,      spZERO, spNINE, NULL,
//            spEIGHT,    NULL,     NULL,   NULL,   spSEVEN,
//            NULL,       NULL,     NULL,   NULL,   NULL,
//            spOPERATOR, spMINUS,  spSIX,  spONE,  NULL,
//            NULL,       NULL,     NULL,   NULL,   NULL,
//           NULL,       spTWO,    NULL,   NULL,   NULL,
//            spTHREE,    NULL,     spFOUR, spFIVE
//};


const uint8_t* ascii2lpc[] = {
            NULL,       NULL,     NULL,   NULL,   NULL,     //0
            NULL,       NULL,     NULL,   NULL,   NULL,     //5
            NULL,       NULL,     NULL,   spMINUS,NULL,     //10
            NULL,       NULL,     NULL,   NULL,   NULL,     //15
            NULL,       NULL,     NULL,   NULL,   NULL,     //20
            NULL,       NULL,     NULL,   NULL,   NULL,     //25
            NULL,       NULL,     NULL,   NULL,   NULL,     //30
            NULL,       NULL,     NULL,   NULL,   NULL,     //35
            NULL,       NULL,     NULL,   NULL,   NULL,     //40
            NULL,       NULL,  spOPERATOR,spZERO, spONE,    //45
            spTWO,      spTHREE,  spFOUR, spFIVE, spSIX,    //50
            spSEVEN,    spEIGHT,  spNINE, NULL,   NULL,     //55
            NULL,       NULL,     NULL,   NULL,   NULL,     //60
            spA,        spB,      spC,    spD,    spE,      //65
            spF,        spG,      spH,    spI,    spJ,      //70
            spK,        spL,      spM,    spN,    spO,      //75
            spP,        spQ,      spR,    spS,    spT,      //80
            spU,        spV,      spW,    spX,    spY,      //85
            spZ,        NULL,     NULL,   NULL,   NULL,     //90
            NULL,       NULL,     spA,    spB,    spC,      //95
            spD,        spE,      spF,    spG,    spH,      //100
            spI,        spJ,      spK,    spL,    spM,      //105
            spN,        spO,      spP,    spQ,    spR,      //110
            spS,        spT,      spU,    spV,    spW,      //115
            spX,        spY,      spZ,    NULL,   NULL,     //120
};



//---------------------------------------------------------------------------------
void speakChar(char c) {
    #ifdef FT817                        // Use LPC to say the character
   //     voice.say(ascii2lpc[(int)c]);
    #else                               // Otherwise we can use WAV files stored on a SD card
        char s[15];
        if (c == '/') {
            strcpy(s,"slash.wav");
        } else if (c == '-') {
            strcpy(s,"hyphen.wav");
        } else {
            strcpy(s,"X.wav");          // Setup dummy filename X
            s[0] = tolower(c);          // Replace X with our actual char
        }
        speakSD(s);
    #endif
}


//---------------------------------------------------------------------------------
void speakChars(const char callsign[]) {
    for (unsigned int i = 0; i < strlen(callsign) ; i++) {
         speakChar(callsign[i]);
    };
}
//---------------------------------------------------------------
void speak(const unsigned int* audioSample) {
        lowerRadio();
        audioPrompt.play(audioSample);
        do {delay(10);} while (audioPrompt.isPlaying());
        unMuteRadio();
}


//---------------------------------------------------------------
void speakSD(const char *filename) {
#ifndef FT817
        #ifdef DEBUG
            Serial.println(filename);
        #endif
        audioPromptSD.play(filename);
        delay(3);                       // This delay is important!
        while (audioPromptSD.isPlaying()) {
          delay(5);
        };
#endif
}


//---------------------------------------------------------------
void muteRadio() {
        audioOutputMixer.gain(0, 0.0); // Mute radio audio
}

//---------------------------------------------------------------
void unMuteRadio() {
        audioOutputMixer.gain(0, 1.0); // Unmute radio audio
}
//---------------------------------------------------------------
void lowerRadio() {
        audioOutputMixer.gain(0, 0.1); // Lower the radio volume to allow speach at same time
}


// Speech FIFO
#define SPEECH_BUF_SZ     8  
char speechBuffer[SPEECH_BUF_SZ];
int speechReadPointer   = 0;
int speechWritePointer  = 0;
int speechCount = 0;


//---------------------------------------------------------------------
void speechBuffer_push(char c) {

  if (speechCount >= SPEECH_BUF_SZ - 1) // If we are about to overflow the buffer, drop the char
      return;
  speechCount++;
  speechBuffer[speechWritePointer++] = c;
  speechWritePointer &= (SPEECH_BUF_SZ - 1);
}

//---------------------------------------------------------------------
char speechBuffer_pull() {
    speechReadPointer &= (SPEECH_BUF_SZ - 1);
    speechCount--;
    return speechBuffer[speechReadPointer++];
}

//---------------------------------------------------------------------
// Returns TRUE when buffer empty
bool speech_buffer_empty(void) {
  if(0 == speechCount) {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------
// Stop all buffered playback
void speech_Buffer_flush(void) {
   speechReadPointer = speechWritePointer;
}

//---------------------------------------------------------------------
// If the speech buffer is empty return or the audio system is busy - just return
// else read the next char in the buffer and start playing it.
void serviceSpeech() {
  if ( speech_buffer_empty() || audioPromptSD.isPlaying())
      return;

  char c = speechBuffer_pull();
  char s[15];
  if (c == '/') {
            strcpy(s,"slash.wav");
  } else if (c == '-') {
            strcpy(s,"hyphen.wav");
  } else {
            strcpy(s,"X.wav");          // Setup dummy filename X.wav
            s[0] = tolower(c);          // Replace X with our actual char
  }
  audioPromptSD.play(s);
  delay(3);                            // The delay is important!
}


