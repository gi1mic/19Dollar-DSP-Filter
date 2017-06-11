@echo off
:: Create voice prompts for GI1MIC open source DSP project
:: Manually replace spaces in the created file names with underscores
::
:: See https://www.elifulkerson.com/projects/commandline-text-to-speech.php for voice.exe
:: See http://sr-convert.sourceforge.net/ for sr-convert
setlocal enabledelayedexpansion
DEL *.WAV
set LIST={ ALPHA BRAVO CHARLIE DELTA ECHO FOXTROT GOLF HOTEL INDIA JULIETT KILO LIMA MIKE NOVEMBER OSCAR PAPA QUEBEC ROMEO SIERRA TANGO UNIFORM VICTOR WHISKEY XRAY YANKIEE ZULU MORSE "MORSE DECODE" "G I 1 M I C" DUAL "LOW PASS" "HI PASS" "PASS THROUGH" "NOTCH" A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 10DB 20DB 30DB 40DB 50DB 60DB SIGNAL POWER SWR ALC MOD LSB USB CW CWR AM "FM WIDE" FM DIGITAL PACKET POINT MHZ A B OFF ON NORMAL REVERSE AUTO FAST SLOW MEMORY VFO FRONT REAR HIGH MEDIUM LOW MIN UP DOWN AGC "NOISE BLANKER" LOCK SPEED "CW PADDLE" M1 M2 M3 M4 M5 M6 M7 M8 M9 M10 M11 M12 "OP FILTER" SQLRFG "RF GAIN" SQUELCH VOX "VOX GAIN" "VOX DELAY" "LOCK MODE" DIAL PANEL "DIGITAL MODE" RTTY "PSK USB" "PSK LSB" "USER USB" "USER LSB" KEYER BREAKIN "BEEP VOLUME" "BEEP TONE" "SSB MIC" "AM MIC" "FM MIC" "DIGITAL MIC" "PKT MIC" "BATTERY CHARGE" "VFO MODE" CONTRAST COLOR "BACK LIGHT" "SCAN RESUME" "PACKET RATE" SCOPE CONT CHK "CW ID" "CW PITCH" "CW SPEED" "MAIN STEP" COURSE FINE "ARTS BEEP" RANGE ALL FREQ "430A R S" "144A R S" "CW DELAY" "AM FM" EMERGENCY "CAT RATE" 4K8 9K6 38K4 "APO TIME" "MEMORY GROUP" "TOT TIME" "DCS INV" "MIC KEY" "MIC SCAN" 9K6MIC "DIGITAL SHIFT" "AM STEP" ANTENNA ID RLSBCAR RUSBCAR TLSBCAR TUSBCAR "CW WEIGHT" "DCS CODE" "DIGITAL DISPLAY" "FM STEP" "MEM TAG" "REPEATER SHIFT" SIDETONE "SSB STEP" "TONE FREQUENCY" EXTEND MENU KEYS 1K2 DW PRIORITY SCAN ART "POWER METER" VOLTS DISPLAY SSB CHARGE MINUS SPLIT "IF SHIFT" IPO ATTENUATION "ANTENNA HF" "ANTENNA 6M" "ANTENNA FM" "ANTENNA AIR" "ANTENNA 2M" "ANTENNA UHF" TNRN TNRI TIRN TIRI TUNSPD }


:: deletes the parentheses from LIST
set _list=%LIST:~1,-1%
for  %%G in (%_list%) do call :processParam  %%G
goto:End

:processParam 
@echo off
set Z=%1
set F=%z: =_%
echo %Z%
echo %F%

	voice  --mono --8bit --khz 48 -r 3 -f -o %F%.tmp %Z%
	sr-convert %F%.tmp 48000 %F%.wav 44100
	
goto:eof

:End

del *.tmp

rem Fixup long filenames - filenames must match the array in the code!
MOVE ANTENNA_HF.WAV  	ANT_HF.wav
move ANTENNA_6M.wav  	ANT_6M.wav 
move ANTENNA_FM.wav  	ANT_FM.wav
move ANTENNA_AIR.wav 	ANT_AIR.wav
move ANTENNA_2M.wav  	ANT_2M.wav
move ANTENNA_UHF.wav 	ANT_UHF.wav
move ARTS_BEEP.wav   	ARTS.wav
move BACK_LIGHT.wav  	BACKLGHT.wav
move BATTERY_CHARGE.wav BATTERY.wav
move BEEP_TONE.wav   	BEEPTONE.wav
move BEEP_VOLUME.wav 	BEEPVOL.wav
move DIGITAL_DISPLAY.wav DIGDSP.wav
move DIGITAL_MIC.wav 	DIGMIC.wav
move EMERGENCY.wav   	EMERGEN.wav
move LOCK_MODE.wav   	LOCKMODE.wav
move MEMORY_GROUP.wav 	MEMGRP.wav
move PACKET_RATE.wav 	PCKTRATE.wav
move SCAN_RESUME.wav 	SCANRES.wav
move TONE_FREQUENCY.wav TONEFREQ.wav
move NOISE_BLANKER.wav 	NOISEBLK.wav
move POWER_METER.wav 	PWRMETER.wav
move ATTENUATION.wav	ATTEN.wav
move OP_FILTER.wav      OPFLTER.wav
move CW_PADDLE.wav      CWPADD.wav
move CW_WEIGHT.wav      CWWGHT.wav
move DIGITAL_MODE.wav   DIGMODE.wav
move MAIN_STEP.wav      MNSTEP.wav
move VOX_DELAY.wav      VOXDELAY.wav
move DIGITAL_SHIFT.wav  DIGSHIFT.wav
move REPEATER_SHIFT.wav REPSHIFT.wav



