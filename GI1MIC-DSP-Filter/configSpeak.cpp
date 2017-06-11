//Rachel.ino - A Speech Synthesiser for the Yaesu FT-817
//Copyright (c) 2016, Julie VK3FOWL and Joe VK3YSP
//Released under the GNU General Public License.
//For more information please visit http://www.sarcnet.org

#include <Audio.h>
#include <Bounce.h>
#include "settings.h"
#include "speech.h"


#define NUMBER 0xFF //Flag indicating data is a numeric type
#define NEGNUM 0xFE //Flag indicating data is a numeric type but contains negative number
#define DOUBLE 0xFD //Flag indicating data is a double byte
#define VFOREF 0xFC //Flag indicating that the data resisdes in a VFO register

#define MUTE false
#define UNMUTE true
#define CHECK true
#define UNCHECK false

//Thanks to Clint Turner KA7OEI for the following undocumented commands.
#define TXMETERS 0xBD //Get the TX meters: PWR, SWR, ALC and MOD  //Undocumented
#define TXSTATUS 0xF7 //Get the TX status: PTT
#define RXSTATUS 0xE7 //Get the RX status: S-meter
#define FREQMODE 0x03 //Get the frequency and mode
#define ADDRDATA 0xBB //Get EEPROM data at the specified address  //Undocumented

volatile bool metersSwitch = false; //Flag indicating the that the meters switch has been pressed
volatile bool configSwitch = false; //Flag indicating the that the config switch has been pressed
volatile bool cancel = false;       //Flag indicating the that the config switch has been pressed while playing
unsigned long lastTime;             //Timer containg the last system time in milliseconds

//Enumeration of all sound files in exactly the same order as on the micro SD card
enum Sound {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9,
            s10, s11, s12, s13, s14, s15, s10DB, s20DB, s30DB, s40DB,
            s50DB, s60DB, sSIG, sPWR, sSWR, sALC, sMOD, sLSB, sUSB, sCW,
            sCWR, sAM,  sFMW, sFM, sDIGITAL, sPACKET, sPOINT, sMHZ, sA, sB,
            sOFF, sON, sNORM, sREV, sAUTO, sFAST, sSLOW, sMEM, sVFO, sFRONT,
            sREAR, sHIGH, sMEDIUM, sLOW, sMIN, sUP, sDOWN, sAGC, sNB, sLOCK,
            sSPEED, sCWPADDLE, sM1, sM2, sM3, sM4, sM5, sM6, sM7, sM8,
            sM9, sM10, sM11, sM12, sOPFILTER, sSQLRFG, sRFGAIN, sSQUELCH, sVOX, sVOXGAIN,
            sVOXDELAY, sLCKMODE, sDIAL, sPANEL, sDIGMODE, sRTTY, sPSKUSB, sPSKLSB, sUSERUSB, sUSERLSB,
            sKEYER, sBREAKIN, sBEEPVOL, sBEEPTONE, sSSBMIC, sAMMIC, sFMMIC, sDIGMIC, sPKTMIC, sBATCHG,
            sVFOMODE, sCONTRAST, sCOLOR, sBKLIGHT, sSCANRESUME, sPKTRATE, sSCOPE, sCONT, sCHK, sCWID,
            sCWPITCH, sCWSPEED,  sMAINSTEP, sCOURSE, sFINE, sARTSBEEP, sRANGE, sALL, sFREQ , s430ARS,
            s144ARS, sCWDELAY, sAMFM, sEMERG, sCATRATE, s4K8, s9K6, s38K4, sAPOTIME, sMEMGRP,
            sTOTTIME, sDCSINV, sMICKEY, sMICSCAN, s9K6MIC, sDIGSHIFT, sAMSTEP, sANTENNA, sID, sRLSBCAR,
            sRUSBCAR, sTLSBCAR, sTUSBCAR, sCWWEIGHT, sDCSCODE, sDIGDISP, sFMSTEP, sMEMTAG, sRPTSHIFT, sSIDETONE,
            sSSBSTEP, sTONEFREQ, sEXTEND, sMENU, sKEYS, s1K2, sDW, sPRIORITY, sSCAN, sART,
            sPWRMETER, sVOLTS, sDISPLAY, sSSB, sCHARGE, sMINUS, sSPLIT, sIFSHIFT, sIPO, sATTEN,
            sANTHF, sANT6M, sANTFM, sANTAIR, sANT2M, sANTUHF, sTNRN, sTNRI, sTIRN, sTIRI,
            sTUNSPD
           };
//430ARS

// ths array needs to be in the same order as above and match the created filenames
const char  *soundFiles[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "10", "11", "12", "13", "14", "15", "10DB", "20DB", "30DB", "40DB",
            "50DB", "60DB", "SIGNAL", "POWER", "SWR", "ALC", "MOD", "LSB", "USB", "CW",
            "CWR", "AM",  "FM_WIDE", "FM", "DIGITAL", "PACKET", "POINT", "MHZ", "A", "B",
            "OFF", "ON", "NORMAL", "REVERSE", "AUTO", "FAST", "SLOW", "MEMORY", "VFO", "FRONT",
            "REAR", "HIGH", "MEDIUM", "LOW", "MIN", "UP", "DOWN", "AGC", "NOISEBLK", "LOCK",
            "SPEED", "CWPADD", "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8",
            "M9", "M10", "M11", "M12", "OPFLTER", "SQLRFG", "RF_GAIN", "SQUELCH", "VOX", "VOX_GAIN",
            "VOXDELAY", "LOCKMODE", "DIAL", "PANEL", "DIGMODE", "RTTY", "PSK_USB", "PSK_LSB", "USER_USB", "USER_LSB",
            "KEYER", "BREAKIN", "BEEPVOL", "BEEPTONE", "SSB_MIC", "AM_MIC", "FM_MIC", "DIGMIC", "PKT_MIC", "BATTERY",
            "VFO_MODE", "CONTRAST", "COLOR", "BACKLGHT", "SCANRES", "PCKTRATE", "SCOPE", "CONT", "CHK", "CW_ID",
            "CW_PITCH", "CW_SPEED",  "MNSTEP", "COURSE", "FINE", "ARTS", "RANGE", "ALL", "FREQ", "430A_R_S",
            "144A_R_S", "CW_DELAY", "AM_FM", "EMERGEN", "CAT_RATE", "4K8", "9K6", "38K4", "APO_TIME", "MEMGRP",
            "TOT_TIME", "DCS_INV", "MIC_KEY", "MIC_SCAN", "9K6MIC", "DIGSHIFT", "AM_STEP", "ANTENNA", "ID", "RLSBCAR",
            "RUSBCAR", "TLSBCAR", "TUSBCAR", "CWWGHT", "DCS_CODE", "DIGDSP", "FM_STEP", "MEM_TAG", "REPSHIFT", "SIDETONE",
            "SSB_STEP", "TONEFREQ", "EXTEND", "MENU", "KEYS", "1K2", "DW", "PRIORITY", "SCAN", "ART",
            "PWRMETER", "VOLTS", "DISPLAY", "SSB", "CHARGE", "MINUS", "SPLIT", "IF_SHIFT", "IPO", "ATTEN",
            "ANT_HF", "ANT_6M", "ANT_FM", "ANT_AIR", "ANT_2M", "ANT_UHF", "TNRN", "TNRI", "TIRN", "TIRI",
            "TUNSPD"
           };


//Special radio parameter arrays.
const byte MODE[] = {sLSB, sUSB, sCW, sCWR, sAM, 0, sFMW, 0, sFM, 0, sDIGITAL, 0, sPACKET};
//Frequency/Mode parameter array. Format: {Values (5), Command}
byte aFreqMode[] = {0x00, 0x00, 0x00, 0x00, 0x00, FREQMODE};
//RX Status parameter array. Format: {Command, values}
byte aRxMeter[]  {RXSTATUS, sSIG, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10DB, s20DB, s30DB, s40DB, s50DB, s60DB};
//TX Status parameter array. Format: {Command, values}
byte aTxMeter[]  {TXMETERS, sPWR, sALC, sSWR, sMOD, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15};

//EEPROM Radio parameter arrays. Format: {Parameter value, EEPROM Address, Bit mask, Menu name sound, Indexed parameter value sounds}
//Front panel keys menu parameters
byte aKEYS[] =     {0x00, 0x76, 0x0F, sKEYS, sM1, sM2, sM3, sM4, sM5, sM6, sM7, sM8, sM9, sM10, sM11};
byte aVFO[] =      {0x00, 0x55, 0x01, sVFO, sA, sB};
byte aAGC[] =      {0x00, 0x57, 0x03, sAGC, sAUTO, sFAST, sSLOW, sOFF};
byte aPWR[] =      {0x00, 0x79, 0x03, sPWR, sHIGH, sMEDIUM, sLOW, sMIN};
byte aDW[] =       {0x00, 0x79, 0x08, sDW, sOFF, sON};
byte aPRIORITY[] = {0x00, 0x79, 0x10, sPRIORITY, sOFF, sON};
byte aSCAN[]  =    {0x00, 0x79, 0x60, sSCAN, sOFF, sUP, sDOWN};
byte aART[] =      {0x00, 0x79, 0x80, sART, sOFF, sON};
byte aPWRMETER[] = {0x00, 0x58, 0x03, sPWRMETER, sPWR, sALC, sSWR, sMOD};
byte aBREAKIN[] =  {0x00, 0x58, 0x20, sBREAKIN, sOFF, sON};
byte aKEYER[] =    {0x00, 0x58, 0x10, sKEYER, sOFF, sON};
byte aVOLTS[] =    {0x00, 0x58, 0x40, sVOLTS, sOFF, sON};
byte aDISPLAY[] =  {0x00, 0x57, 0x04, sDISPLAY, sLOW, sHIGH};
byte aCHARGE[] =   {0x00, 0x7B, 0x10, sCHARGE, sOFF, sON};
byte aVOX[] =      {0x00, 0x58, 0x80, sVOX, sOFF, sON};
byte aNB[] =       {0x00, 0x57, 0x20, sNB, sOFF, sON};
byte aVFOMODE[] =  {0x00, 0x55, 0x80, sVFOMODE, sMEM, sVFO};
byte aLOCK[] =     {0x00, 0x57, 0x40, sLOCK, sON, sOFF};
byte aSPLIT[] =    {0x00, 0x7A, 0x80, sSPLIT, sOFF, sON};
byte aIFSHIFT[] =  {0x00, 0x57, 0x10, sIFSHIFT, sOFF, sON};
byte aIPO[] =      {0x00, 0x02, 0x20, sIPO, VFOREF};
byte aATTEN[] =    {0x00, 0x02, 0x10, sATTEN, VFOREF};
byte aTUNSPD[] =   {0x00, 0x57, 0x80, sTUNSPD, sFAST, sSLOW};
byte aMENU[] =     {0x00, 0x75, 0x3F, sMENU,
                    s144ARS, s430ARS, s9K6MIC, sAMFM, sAMMIC, sAMSTEP, sANTENNA, sAPOTIME, sARTSBEEP, sBKLIGHT,
                    sBATCHG, sBEEPTONE, sBEEPVOL, sCATRATE, sCOLOR, sCONTRAST, sCWDELAY, sCWID, sCWPADDLE, sCWPITCH,
                    sCWSPEED, sCWWEIGHT, sDCSCODE, sDIGDISP, sDIGMIC, sDIGMODE, sDIGSHIFT, sEMERG, sFMMIC, sFMSTEP,
                    sID, sLCKMODE, sMAINSTEP, sMEMGRP, sMEMTAG, sMICKEY, sMICSCAN, sOPFILTER, sPKTMIC, sPKTRATE,
                    sSCANRESUME, sRPTSHIFT, sSCOPE, sSIDETONE, sSQLRFG, sSSBMIC, sSSBSTEP, sTONEFREQ, sTOTTIME, sVOXDELAY,
                    sVOXGAIN, sEXTEND, sDCSINV, sRLSBCAR, sRUSBCAR, sTLSBCAR, sTUSBCAR
                   };
/*MENU*/
/*01*/byte a144ARS[] =     {0x00, 0x5F, 0x40, s144ARS, sOFF, sON};
/*02*/byte a430ARS[] =     {0x00, 0x5F, 0x20, s430ARS, sOFF, sON};
/*03*/byte a9K6MIC[] =     {0x00, 0x6C, 0x7F, s9K6MIC, NUMBER};
/*04*/byte aAMFM[] =       {0x00, 0x63, 0x80, sAMFM, sON, sOFF};
/*05*/byte aAMMIC[] =      {0x00, 0x68, 0x7F, sAMMIC, NUMBER};
/*06*/byte aAMSTEP[] =     {0x00, 0x03, 0x38, sAMSTEP, VFOREF};
/*07*/byte aANTHF[] =      {0x00, 0x7A, 0x01, sANTHF, sFRONT, sREAR};
/*07*/byte aANT6M[] =      {0x00, 0x7A, 0x02, sANT6M, sFRONT, sREAR};
/*07*/byte aANTFM[] =      {0x00, 0x7A, 0x04, sANTFM, sFRONT, sREAR};
/*07*/byte aANTAIR[] =     {0x00, 0x7A, 0x08, sANTAIR, sFRONT, sREAR};
/*07*/byte aANT2M[] =      {0x00, 0x7A, 0x10, sANT2M, sFRONT, sREAR};
/*07*/byte aANTUHF[] =     {0x00, 0x7A, 0x20, sANTUHF, sFRONT, sREAR};
/*08*/byte aAPOTIME[] =    {0x00, 0x65, 0x07, sAPOTIME, NUMBER};
/*09*/byte aARTSBEEP[] =   {0x00, 0x5D, 0xC0, sARTSBEEP, sOFF, sRANGE, sALL};
/*10*/byte aBKLIGHT[] =    {0x00, 0x5B, 0xC0, sBKLIGHT, sOFF, sON, sAUTO};
/*11*/byte aBATCHG[] =     {0x00, 0x62, 0xC0, sBATCHG, s6, s8, s10};
/*12*/byte aBEEPTONE[] =   {0x00, 0x5C, 0x80, sBEEPTONE, sLOW, sHIGH};
/*13*/byte aBEEPVOL[] =    {0x00, 0x5C, 0x7F, sBEEPVOL, NUMBER};
/*14*/byte aCATRATE[] =    {0x00, 0x64, 0xC0, sCATRATE, s4K8, s9K6, s38K4};//CANT TEST
/*15*/byte aCOLOR[] =      {0x00, 0x5B, 0x30, sCOLOR, NUMBER};
/*16*/byte aCONTRAST[] =   {0x00, 0x5B, 0x0F, sCONTRAST, NUMBER};
/*17*/byte aCWDELAY[] =    {0x00, 0x60, 0xFF, sCWDELAY, NUMBER};
/*18*/byte aCWID[] =       {0x00, 0x5D, 0x10, sCWID, sOFF, sON};
/*19*/byte aCWPADDLE[] =   {0x00, 0x58, 0x04, sCWPADDLE, sNORM, sREV};
/*20*/byte aCWPITCH[] =    {0x00, 0x5E, 0x0F, sCWPITCH, NUMBER};
/*21*/byte aCWSPEED[] =    {0x00, 0x62, 0x3F, sCWSPEED, NUMBER};//0-56 = 4-60wpm. Number=WPM+4.
/*22*/byte aCWWEIGHT[] =   {0x00, 0x5F, 0x1F, sCWWEIGHT, NUMBER};
/*23*/byte aDCSCODE[] =    {0x00, 0x07, 0x3F, sDCSCODE, VFOREF};
/*24*/byte aDIGDISP[] =    {0x00, 0x6F, 0xFF, sDIGDISP, DOUBLE, 0x00};
/*25*/byte aDIGMIC[] =     {0x00, 0x6A, 0x7F, sDIGMIC, NUMBER};
/*26*/byte aDIGMODE[] =    {0x00, 0x65, 0xE0, sDIGMODE, sRTTY, sPSKLSB, sPSKUSB, sUSERLSB, sUSERUSB};
/*27*/byte aDIGSHIFT[] =   {0x00, 0x6D, 0xFF, sDIGSHIFT, DOUBLE, 0x00};
/*28*/byte aEMERG[] =      {0x00, 0x64, 0x20, sEMERG, sOFF, sON};
/*29*/byte aFMMIC[] =      {0x00, 0x69, 0x7F, sFMMIC, NUMBER};
/*30*/byte aFMSTEP[] =     {0x00, 0x03, 0x07, sFMSTEP, VFOREF};
/*31*/byte aID[] =         {0x00, 0x03, 0x07, sID, NUMBER};        //Not in EEPROM?
/*32*/byte aLCKMODE[] =    {0x00, 0x5E, 0x30, sLCKMODE, sDIAL, sFREQ, sPANEL};
/*33*/byte aMAINSTEP[] =   {0x00, 0x5D, 0x20, sMAINSTEP, sFINE, sCOURSE};
/*34*/byte aMEMGRP[] =     {0x00, 0x65, 0x10, sMEMGRP, sOFF, sON};
/*35*/byte aMEMTAG[] =     {0x00, 0x03, 0x07, sMEMTAG, NUMBER};//N/A
/*36*/byte aMICKEY[] =     {0x00, 0x68, 0x80, sMICKEY, sOFF, sON};
/*37*/byte aMICSCAN[] =    {0x00, 0x67, 0x80, sMICSCAN, sOFF, sON};
/*38*/byte aOPFILTER[] =   {0x00, 0x5E, 0xC0, sOPFILTER, sOFF, sSSB, sCW};
/*39*/byte aPKTMIC[] =     {0x00, 0x6B, 0x7F, sPKTMIC, NUMBER};
/*40*/byte aPKTRATE[] =    {0x00, 0x5D, 0x04, sPKTRATE, s1K2, s9K6};
/*41*/byte aSCANRESUME[] = {0x00, 0x5D, 0x03, sSCANRESUME, sOFF, s3, s5, s10};
/*42*/byte aRPTSHIFT[] =   {0x00, 0x03, 0x07, sRPTSHIFT, VFOREF}; //Requires a TRIPLE
/*43*/byte aSCOPE[] =      {0x00, 0x5D, 0x08, sSCOPE, sCONT, sCHK};
/*44*/byte aSIDETONE[] =   {0x00, 0x61, 0x7F, sSIDETONE, NUMBER};
/*45*/byte aSQLRFG[] =     {0x00, 0x5F, 0x80, sSQLRFG, sRFGAIN, sSQUELCH};
/*46*/byte aSSBMIC[] =     {0x00, 0x67, 0x7F, sSSBMIC, NUMBER};
/*47*/byte aSSBSTEP[] =    {0x00, 0x03, 0xC0, sSSBSTEP, VFOREF};
/*48*/byte aTONEFREQ[] =   {0x00, 0x06, 0x1F, sTONEFREQ, VFOREF};//0 = 67.0 Hz
/*49*/byte aTOTTIME[] =    {0x00, 0x66, 0x1F, sTOTTIME, NUMBER};//OK TOT OFF = 0
/*50*/byte aVOXDELAY[] =   {0x00, 0x64, 0x1F, sVOXDELAY, NUMBER};//Multiply number by 100 for milliseconds
/*51*/byte aVOXGAIN[] =    {0x00, 0x63, 0x7F, sVOXGAIN, NUMBER};
/*52*/byte aEXTEND[] =     {0x00, 0x6B, 0x80, sEXTEND, sOFF, sON};
/*53*/byte aDCSINV[] =     {0x00, 0x66, 0xC0, sDCSINV, sTNRN, sTNRI, sTIRN, sTIRI};
/*54*/byte aRLSBCAR[] =    {0x00, 0x71, 0xFF, sRLSBCAR, NEGNUM};
/*55*/byte aRUSBCAR[] =    {0x00, 0x72, 0xFF, sRUSBCAR, NEGNUM};
/*56*/byte aTLSBCAR[] =    {0x00, 0x73, 0xFF, sTLSBCAR, NEGNUM};
/*57*/byte aTUSBCAR[] =    {0x00, 0x74, 0xFF, sTUSBCAR, NEGNUM};

//Index of all radio parameter arrays
byte* radioParams[] = {a144ARS, a430ARS, a9K6MIC, aAMFM, aAMMIC, aAMSTEP, aANTHF, aANT6M, aANTFM,
             aANTAIR, aANT2M, aANTUHF, aAPOTIME, aARTSBEEP,
             aBKLIGHT, aBATCHG, aBEEPTONE, aBEEPVOL, aCATRATE, aCOLOR, aCONTRAST, aCWDELAY, aCWID, aCWPADDLE,
             aCWPITCH, aCWSPEED, aCWWEIGHT, aDCSCODE, aDIGDISP, aDIGMIC, aDIGMODE, aDIGSHIFT, aEMERG, aFMMIC,
             aFMSTEP, /*aID*/ aLCKMODE, aMAINSTEP, aMEMGRP, /*aMEMTAG*/ aMICKEY, aMICSCAN, aOPFILTER, aPKTMIC,
             aPKTRATE, aSCANRESUME, /*aRPTSHIFT*/ aSCOPE, aSIDETONE, aSQLRFG, aSSBMIC, aSSBSTEP, aTONEFREQ, aTOTTIME,
             aVOXDELAY, aVOXGAIN, aEXTEND, aDCSINV, aRLSBCAR, aRUSBCAR, aTLSBCAR, aTUSBCAR,
             aVFO, aNB,  aAGC, aPWR, aDW, aPRIORITY, aSCAN, aART, aPWRMETER,
             aBREAKIN, aKEYER, aVOLTS, aDISPLAY, aCHARGE, aVOX, aVFOMODE, aLOCK, aSPLIT, aIFSHIFT,
             aIPO, aATTEN, aTUNSPD, aKEYS, aMENU
            };

Bounce meterSelect = Bounce(METERSPIN, 15); // debounce the meter speak pin
Bounce configSelect = Bounce(CONFIGPIN, 15); // debounce the config speak pin

//------------------------------------------------------------------
void playWav(byte index) {

  char filename[20];
  sprintf(filename,"%s.wav",soundFiles[index]);
  lowerRadio();
  speakSD(filename);
  unMuteRadio();
}


//------------------------------------------------------------------
void playNumber(int value) {
  //Convert a value 0-999 to BCD
  byte hundreds = value / 100;
  value -= hundreds * 100;
  byte tens = value / 10;
  value -= tens * 10;
  byte ones = value;
  //Play each digit suppressing leading zeros
  if (hundreds > 0) playWav(hundreds);
  if ((hundreds > 0) || (tens > 0)) playWav(tens);
  playWav(ones);
}

//------------------------------------------------------------------
byte compress(byte value, byte mask) {
  //Right compress a value based on a mask
  //Mask the value
  value &= mask;
  //Shift the value and mask as far right as it will go
  while ((mask & 0x01) != 1) {
    value >>= 1;
    mask >>= 1;
  }
  return value;
}

//------------------------------------------------------------------
bool compare(int item1, int item2, bool checkit) {
  bool changed = true;
  if (checkit) {
    if (item1 == item2) changed = false;
  }
  return changed;
}

//------------------------------------------------------------------
void readRig(byte cmd, int address, byte* reply, byte count) {
  //Send a command to the rig to get count bytes at the address and return them to a buffer
  //Reapeat sending the command until count bytes are received
  do {
    //Flush the receive buffer
    while (Serial2.available() > 0) Serial2.read();
    //Send a 5-byte command to the rig
    Serial2.write(address >> 8);
    Serial2.write(address & 0xFF);
    Serial2.write(0);
    Serial2.write(0);
    Serial2.write(cmd);
    //Wait for a reply of the specified number of bytes or timeout
  } while (Serial2.readBytes(reply, count) == 0);
}

//------------------------------------------------------------------
byte readRigMask(int address, byte mask) {
  //Send the Read EEPROM command to the rig. Return the first byte of reply commpressed right by the mask.
  byte reply[2];
  readRig(ADDRDATA, address, reply, 2);
  return compress(reply[0], mask);
}

//------------------------------------------------------------------
byte getBandRegisterValue(byte byteOffset, byte mask) {
  const int vfoA [] = {0x07D, 0x097, 0x0B1, 0x0CB, 0x0E5, 0x0FF, 0x119, 0x133, 0x14D, 0x167, 0x181, 0x19B, 0x1B5, 0x1CF, 0x1E9};
  const int vfoB [] = {0x203, 0x21D, 0x237, 0x251, 0x26B, 0x285, 0x29F, 0x2B9, 0x2D3, 0x2ED, 0x307, 0x321, 0x33B, 0x355, 0x36F};
  byte vfo, band;
  int reg;
  vfo = readRigMask(0x55, 0x01);      //Read VFO A/B from 0x55, 0x01
  if (vfo == 0) {
    band = readRigMask(0x59, 0x0F);   //Read VFO A Band from 0x59,0x0F
    reg = vfoA[band];             //Use the VFO A Band as an index to get the VFO A base address
  }
  else {
    band = readRigMask(0x59, 0xF0);   //Read VFO B Band from 0x59,0xF0
    reg = vfoB[band];             //Use the VFO B Band as an index to get the VFO A base address
  }
  return readRigMask(reg + byteOffset, mask);
}

//------------------------------------------------------------------
void getSettings(bool unmuted, bool checkit) {
  byte reply[2];
  byte address, mask, base, title, minus;
  int value, last;
  bool changed;
  byte count = sizeof(radioParams) / sizeof(byte*);
  for (int i = 0; i < count; i++) {
    if (cancel) {
      cancel = false;
      break;
    }
    address = radioParams[i][1];
    base = radioParams[i][4];
    mask = radioParams[i][2];
    changed = false;
    minus = false;
    //For each different parameter type: Get the value, check if it has changed and if it is negative
    switch (base) {
      case NUMBER:
        last = radioParams[i][0];
        value = readRigMask(address, mask);
        changed = compare(value, last, checkit);
        if (changed) radioParams[i][0] = value;
        break;
      case NEGNUM:
        last = radioParams[i][0];
        value = readRigMask(address, mask);
        changed = compare(value, last, checkit);
        if (changed) {
          if (value < 0) minus = true;
          radioParams[i][0] = value;
        }
        break;
      case DOUBLE:
        last = (radioParams[i][0] << 8) + radioParams[i][5];
        readRig(0xBB, address, reply, 2);
        value = (reply[0] << 8) + reply[1];
        changed = compare(value, last, checkit);
        if (changed) {
          if (value < 0) minus = true;
          radioParams[i][0] = reply[0];
          radioParams[i][5] = reply[1];
        }
        break;
      case VFOREF:
        last = radioParams[i][0];
        //The address in this case is the band register
        value = getBandRegisterValue(address, mask);
        changed = compare(value, last, checkit);
        if (changed) radioParams[i][0] = value;
        break;
      default:
        last = radioParams[i][0];
        value = readRigMask(address, mask);
        changed = compare(value, last, checkit);
        if (changed) {
          radioParams[i][0] = value;
          value = radioParams[i][4 + value];
        }
        break;
    }
    if (unmuted && changed) {
      title = radioParams[i][3];
      playWav(title);
      if (minus) {
        value = -value;
        playWav(sMINUS);
      }
      if (base < VFOREF)playWav(value);
      else playNumber(value);
    }
    if (!checkit) delay(500);
  }
}

//------------------------------------------------------------------
void getConfig() {
  if (configSwitch) {
    getSettings(UNMUTE, UNCHECK);
    configSwitch = false;
  }
}

//------------------------------------------------------------------
void getMeters() {
  if (metersSwitch) {
    byte reply[2];
    readRig(TXSTATUS, 0, reply, 1);
    if ((reply[0] & 0x80) == 0) {
      //Transmit mode
      readRig(aTxMeter[0], 0, reply, 2);
      playWav(aTxMeter[1]);
      playWav(aTxMeter[5 + compress(reply[0], 0xF0)]);
      playWav(aTxMeter[2]);
      playWav(aTxMeter[5 + compress(reply[0], 0x0F)]);
      playWav(aTxMeter[3]);
      playWav(aTxMeter[5 + compress(reply[1], 0xF0)]);
      playWav(aTxMeter[4]);
      playWav(aTxMeter[5 + compress(reply[1], 0x0F)]);
    }
    else {
      //Receive mode
      readRig(aRxMeter[0], 0, reply, 1);
      playWav(aRxMeter[1]);
      playWav(aRxMeter[2 + compress(reply[0], 0x0F)]);
    }
    metersSwitch = false;
  }
}

//------------------------------------------------------------------
void getFreqMode(bool unmuted, bool checkit) {
  //This function reports the frequency or mode if either one changes
  //It reports frequent changes at regular intervals and once again after the changes stop
  byte reply[5];
  byte value;
  byte last;
  bool changed = false;
  //Get the radio frequency and mode reply using the special command
  readRig(aFreqMode[5], 0, reply, 5);
  //Check if the frequency has changed
  for (byte i = 0; i < 4; i++) {
    value = reply[i];
    last = aFreqMode[i];
    changed = compare(value, last, checkit);
    if (changed) break;
  }
  if (changed) {
    unsigned long thisTime = millis();
    unsigned long elapsed = thisTime - lastTime;
    if (elapsed > 2000UL) {
      for (byte i = 0; i < 4; i++) aFreqMode[i] = reply[i];
      if (unmuted) {
        byte f[9];
        f[0] = aFreqMode[0] >> 4;
        f[1] = aFreqMode[0] & 0x0F;
        f[2] = aFreqMode[1] >> 4;
        f[3] = sPOINT;
        f[4] = aFreqMode[1] & 0x0F;
        f[5] = aFreqMode[2] >> 4;
        f[6] = aFreqMode[2] & 0x0F;
        f[7] = aFreqMode[3] >> 4;
        f[8] = aFreqMode[3] & 0x0F;
        //Supress leading and trailing zeros when reading out frequencies
        int firstDigit = 0;
        for (int i = 0; i < 2; i++) {
          if (f[i] == 0) firstDigit = i + 1;
          else break;
        }
        int lastDigit = 8;
        for (int i = 8; i > 4; i--) {
          if (f[i] == 0) lastDigit = i - 1;
          else break;
        }
        playWav(sFREQ);
        //Now play only the relevent digits inclusive
        for (int i = firstDigit; i <= lastDigit; i++) playWav(f[i]);
        playWav(sMHZ);
        //Reset the timer
        lastTime = millis();
      }
    }
  }
  value = reply[4];
  last = aFreqMode[4];
  changed = compare(value, last, checkit);
  if (changed) {
    aFreqMode[4] = reply[4];
    if (unmuted) playWav(MODE[aFreqMode[4] & 0x0F]);
  }
}



//------------------------------------------------------------------
void meterSwitchPressed() {
  meterSelect.update();
  if (meterSelect.fallingEdge()) {
    if (configSwitch)cancel = true;
    else metersSwitch = true;
  }

}

//------------------------------------------------------------------
void configSwitchPressed() {
  configSelect.update();
  if (configSelect.fallingEdge()) {
    configSwitch = true;
  }
}


//------------------------------------------------------------------
void initConfigSpeak() {
#ifdef SPEECHCONFIG
  //Configure digital input pins for two switches: One for reading Tx and RX meters and one for reading total radio configuration
  pinMode(METERSPIN, INPUT_PULLUP);
  pinMode(CONFIGPIN, INPUT_PULLUP);

  Serial2.begin(CAT_BAUD, SERIAL_8N2);
  
  lastTime = millis();
  metersSwitch = false;
  configSwitch = false;

  //Get the initial settings from the rig while the speech is muted.
  getSettings(MUTE, CHECK);
  getFreqMode(MUTE, CHECK);
#endif
}

//------------------------------------------------------------------
void serviceConfigSpeak() {
#ifdef SPEECHCONFIG
  //Service each of the routines which report changes
  configSwitchPressed();
  meterSwitchPressed();
  getMeters();                //Report TX or RX meters if the meters switch has been pressed
  getConfig();                //Report total radio configuration is the config switch has been pressed
  getFreqMode(UNMUTE, CHECK); //Report any changes to rig frequency and mode
  getSettings(UNMUTE, CHECK); //Report any changes to rig settings
#endif
}
