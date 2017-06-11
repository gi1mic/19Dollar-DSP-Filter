@echo off
:: Create embedded voice prompts for the GI1MIC open source DSP project


sr-convert.exe	"..\SD Files\DUAL.wav"				-	wavDual.wav			11025 
sr-convert.exe	"..\SD Files\HI_PASS.wav"			-	wavHipass.wav		11025 
sr-convert.exe	"..\SD Files\LOW_PASS.wav"			-	wavLowpass.wav		11025 
sr-convert.exe	"..\SD Files\MORSE.wav"				-	wavMorse.wav 		11025 
sr-convert.exe	"..\SD Files\MORSE_DECODE.wav"		-	wavMorsedecode.wav	11025 
sr-convert.exe	"..\SD Files\PASS_THROUGH.wav"		-	wavPassthru.wav		11025 
sr-convert.exe	"..\SD Files\SSB.wav"				-	wavSSB.wav			11025 

wav2sketch.exe
del *.wav

exit


move AudioSampleWavdual.cpp			wavdual.cpp
move AudioSampleWavdual.h			wavdual.h
move AudioSampleWavHipass.cpp		wavHipass.cpp
move AudioSampleWavHipass.h			wavHipass.h
move AudioSampleWavLowpass.cpp		wavLowpass.cpp
move AudioSampleWavLowpass.h		wavLowpass.h
move AudioSampleWavMorse.cpp		wavMorse.cpp
move AudioSampleWavMorse.h			wavMorse.h
move AudioSampleWavMorsedecode.cpp	wavMorsedecode.cpp
move AudioSampleWavMorsedecode.h	wavMorsedecode.h
move AudioSampleWavPassthru.cpp		wavPassthru.cpp
move AudioSampleWavPassthru.h		wavPassthru.h
move AudioSampleWavSSB.cpp			wavSSB.cpp
move AudioSampleWavSSB.h			wavSSB.h




