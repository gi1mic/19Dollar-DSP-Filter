
Introduction
============

This is a low cost open source audio DSP filter for Radio Hams. It is
released as a building block for anyone who wants to design or customise
their own audio filters.

It is different from commercial offerings because it is cheap, can
contain multiple filter profiles and uses voice messages to tell
describe which filter has been selected.

A video of the board in action fitted using an FT-817 will be available
soon.

While I am using the board with a Yaesu FT-817 it should be easily
adaptable to any other rig or fitted inside an amplified speaker if you
don’t want to modify your radio.

The design is based around a \$19 Teensy 3.2 board along with a handful
of other components which most radio hams probably have in their junk
box. The board is programmed using the Arduino IDE and takes audio in
via an ADC, filters it using a software defined FIR filter and outputs
it via a DAC.

Before anyone asks - no you cannot use an Arduino to do this.

This is only a starting point and not meant to be a detailed
introduction to Teensy or DSP filters. There are many other cool things
that could be done with this board and library.

Setting up a development environment
====================================

This is well documented on the [Teensy Web
Site](https://www.pjrc.com/teensy/td_download.html) in principle you
need to:

1.  Download the latest
    [Teensyduino](https://www.pjrc.com/teensy/td_download.html) and
    check which versions of the Arduino IDE are supported (At the time
    of writing IDE 1.82)

2.  Download and install the relevant [Arduino
    IDE](https://www.arduino.cc/en/Main/Software)

3.  Install [Teensyduino](https://www.pjrc.com/teensy/td_download.html)
    on top of the Arduino IDE

4.  Load up one of the Teensy examples included with the IDE and plug in
    a Teensy 3.2

5.  On the Tools menu option set

    -   Teensy: 3.1/3.2

    -   USB Type: serial

    -   CPU Speed: 96Mhz

    -   Optimize: Faster

    -   Com Port: Set as required to select the Teensy

6.  Select “Sketch/Upload” to flash the Teensy and run the code.

Once you are happy with the basics you can try out the DSP Sketch with
its sample filters and then move on to creating your own filters and
voice prompts!

Fitting the Teensy into a FT-817 
=================================

**WARNING: **

**Make sure you cut the PCB track on the Teensy which connects the
external RAW and USB power together. Failure to do this may cause damage
to your PC USB port. Cutting this track means the Teensy is no longer
powered via USB.**

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_12.gif](media/image1.gif){width="2.094463035870516in"
height="0.8155555555555556in"}

Fitting the Teensy DSP filter to an FT-817 requires the following:

-   removal of the main PCB to gain access to the underside.

-   Tapping a power feed

-   Breaking into the audio chain so we can insert the DSP in line

-   Tapping of a digital trigger signal that we can use to configure
    different modes

-   Insert a CW filter bypass if a CW filter is not already fitted

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0009.jpg](media/image2.jpeg){width="1.980456036745407in"
height="2.970685695538058in"}

Planned fitting location for the Teensy

Removing the main PCB
---------------------

![https://github.com/gi1mic/19Dollar-DSP-Filter/media/Yaesu%2BFT-817%2BService%2BManual.jpg](media/image3.jpeg){width="3.4383562992125984in"
height="2.1059930008748906in"}

Remove the top cover from the FT-817

  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  ![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0009a.jpg](media/image4.jpeg){width="2.6071423884514435in" height="3.9107141294838144in"}   ![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0009c.jpg](media/image5.jpeg){width="2.611111111111111in" height="3.9166666666666665in"}
                                                                                                                                                                                 
  Remove the five screws holding the main PCB                                                                                                                                    Unclip the cables
  ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Getting a power feed
--------------------

PWR is taken straight directly of the FT817 PCB. On my rig this provides
9.6V when the FT-817 is used with a 13.8V external power source. The
voltage regulators fitted to the Teensy 3.2 boards are a LP38691 which
is rated at 10V max (Teensy documentation states 6V max). I added an
inline power diode to drop the input another 0.6V to create some
headroom. While close to the limit the regulator does not get
excessively warm.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0018.jpg](media/image6.jpeg){width="5.315475721784777in"
height="4.441744313210848in"}

Breaking into the audio chain
-----------------------------

Remove C1338 and attach two shielded audio cables as below. You can
easily determine which is the the audio amplifier by simply tapping the
bare end and listening for mains hum.

![](media/image7.jpeg)

Audio input to the Teensy is accomplished using one of the ADC
convertors on pin PA2. This is decoupled from the FT817 via a 10nf
ceramic cap. I used a 6.8K resistor between PA2 and GND to limit the
input as it was saturating the ADC. You can verify if you need to adjust
this value later using a terminal emulator connected to the Teensy once
the code is loaded.

Audio output from the Teensy comes from the internal 12bit DAC. This has
a much higher quality output than PWM outputs used by most single chip
systems. Again, the DAC is decoupled from the FT817 via a 10nf ceramic
cap.

Use the built-in debugging info to ensure the audio level remains mostly
bellow 1.0. If it constantly exceeds this level, you will need to lower
it by changing the input resistor or change it to a proper voltage
divider.

CW filter bypass
----------------

You can skip this if you have a physical CW filter fitted. Otherwise add
two 10nf ceramic caps as shown below. These act as an audio pass-through
for the missing filter when it is selected.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0012.jpg](media/image8.jpeg){width="6.255952537182852in" height="6.22206583552056in"}
------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Trigger
-------

For the moment add a small signal wire (green wire) as shown below. This
is used to switch filter modes.

Connect the wire to PIN0 on the Teensy which is configured as an active
low input toggle. It is used to select between the different audio
filters. Since the signal I am using from the FT817 has a level higher
than 3.2V it is feed via a simple voltage divider consisting of two
resistors. The Teensy has 5V tolerant inputs.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0019.jpg](media/image9.jpeg){width="5.819489282589676in"
height="3.595238407699038in"}

In the future, I aim to take mode signal(s) from switching IC Q1049
allowing automatic selection of filters based on the modulation mode of
the radio.

With the cables fitted it’s time to resemble the radio ensuring no wires
are trapped or nipped.

Teensy
------

### Schematic

As you can see there are only six components external to the Teensy.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_schem.jpg](media/image10.jpeg){width="3.2142858705161856in"
height="3.016163604549431in"}

![](media/image11.png){width="2.9088418635170603in"
height="1.83250656167979in"}

### Debugging

Once programmed the device will appear as a standard serial port. Use
the built in Arduino IDE Serial Monitor or a terminal program of your
choice to connect to the board at 9600 Baud, 8,N,1.

Debug output format

-   Start-up message

-   Peak audio input level

-   CPU utilisation

-   Mode changes

### Prototype 

This was my initial hacked together audio used in testing before I
discovered the level was too high.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0037.jpg](media/image12.jpeg){width="4.560235126859142in"
height="3.2976192038495187in"}

The following images shows my test board and power connection and the
shrink wrap ready to go. Initially I was using PIN1 as the mode switch
as show in the photo. I have since switched to PIN0 as reflected in the
sample code to simplify the hardware.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\_0045.jpg](media/image13.jpeg){width="4.643081802274716in"
height="3.10119094488189in"}

Generating new DSP filters
--------------------------

### Free Online Tool

Go to <http://t-filter.engineerjs.com/> and use the Predefined drop down
to select one of the predefined filter types such as Bandpass.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media](media/image14.png){width="6.254853455818023in"
height="3.937007874015748in"}

Change the frequency to 44117 HZ.

Now modify the filter to obtain the desired result.

Use the “Filter Design and Analysis” button to recalculate the design as
required. Note it is important to set the maximum number of taps or
check the actual number of taps has an even number and does not exceed
the Teensy maximum of 200. If this is a real problem, modify the code to
daisy chain multiple filters together. You should be able to chain three
or four filters before maxing out the processor (check debugging for CPU
utilisation info).

When you are happy you can export the coefficients as a 16bit int "C/C++
array" option. You will need to modify the "int" to "short" in the
generated code for it to work with the Teensy.

Now you can update filters.h to add your newly designed code and then
add the new filters into the “fir\_filter fir\_list” array in the main
Sketch using the existing code as an example.

### Using MATLAB

Fire up MATLAB and go to the APPS menu. Then select the “Filter Builder”
app under “Signal Processing and Communications”.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media\matlab.jpg](media/image15.jpeg){width="6.25625in"
height="5.095138888888889in"}

1.  Set the Frequency Specification to 44117 HZ.

2.  Select a response type such as Bandpass

3.  Select one of the FIR filter types

4.  Specify a maximum order of 200 - If this is a real problem, modify
    the code to daisy chain multiple filters together. You should be
    able to chain three or four filters before maxing out the processor
    (check debugging for CPU utilisation info).

5.  There is no five I’m just checking to see if you are still awake

6.  Specify the parameters for your specific filter type

7.  Click the Design Filter button to model the filter and update the
    graph

8.  When happy select the “Targets Menu/Generate C Header” option

The Numerator and Numerator length are simply the names used in the
generated file for the coefficients. I would recommend using something
line filterXXXXX and filterLenXXXXX.

Export the files as Unsigned 16bit integers for compatibility with the
Teensy development environment. The files should be saved into your
development environment.

You can then update filters.h to add your newly designed code and then
add the filters into the “fir\_filter fir\_list” array in the main
Sketch using the existing code as an example.

![https://github.com/gi1mic/19Dollar-DSP-Filter/media](media/image16.png){width="4.291666666666667in"
height="3.1086767279090113in"}

Generating Audio Prompts
------------------------

Audio prompts are used to identify filters as they are selected.
Creating a new audio prompt requires creating a suitable WAV file and
then converting that into a data (C Array) for use inside the
application.

### Creating a WAV file

An easy way to create the WAV file is to use the online system
<https://online-voice-recorder.com/> this allows you to record a message
and trim it to suit. When you are happy you can save the final message
to a directory on your computer.

### WAV file conversion

WAV file conversion requires a windows command line application called
[wav2sketch](https://github.com/PaulStoffregen/Audio/tree/master/extras/wav2sketch).
Simply put it in a folder along with the WAV file(s) you generated
earlier and run it. Detailed instructions can be found
[here](https://forum.pjrc.com/threads/42401-Instructions-or-tutorials-for-using-wav2sketch?p=135069&viewfull=1#post135069).

Once the conversion has finished it will have created a CPP and H file
for each input WAV file. All you need to do is add these to you project
using the existing sample files as examples.

Background
==========

A while ago I spotted some third-party add-on DSP filters for the
FT-817. A quick visual inspection of the product photographs showed they
were probably based on the PIC32MX range of microprocessor and like any
good hacker I ordered up a couple of low cost dev boards.

While waiting for the boards I started researching DSP filter
implementations including reading the DSP chapter in the ARRL handbook.
This was all well and good but I did not turn up any great examples of
working code.

Anyway, by the time the boards arrived I had become started some other
projects so the boards were consigned to my growing stash of low cost
processor boards with the aim of looking at them at some point in the
future.

Jump forward a month or so and I happen to be reading an article on
[Hackaday](http://hackaday.com/2017/04/29/teensy-and-3d-printer-make-beautiful-music-together/#more-254819)
covering the use of a [Teensy](https://www.pjrc.com/teensy/) board as an
audio synthesiser. After watching the linked
[video](https://www.youtube.com/watch?v=KbcNqarBTsI) of the device in
action I became intrigued as to how such a low-cost device could
generate incredible audio.

At this point I had never looked at the
[Teensy](https://www.pjrc.com/teensy/) in detail. I knew it used the
Arduino IDE and that many of the Arduino libraries would work on it but
I thought it was just a faster Arduino clone. What a mistake that was.

Turns out the Teensy is based on a Freescale ARM Cortex M4 processor
which includes a wide range peripherals including two analogue to
digital convertors, a 12bit digital to analogue convertor, 64K RAM and
256K Flash. It also supports a range of DSP instructions.

What made it even more interesting was the PJRC the developers of the
Teensy had released an [audio
library](https://www.pjrc.com/teensy/td_libs_Audio.html) for the board
making digital audio very straight forward.

As luck, would have it I happened to have a few [Teensy
3.2](https://www.pjrc.com/store/teensy32.html) boards on hand (impulse
purchases from using [OSH Park PCB services](http://oshpark.com/)). So,
a few minutes later I had a development system configured and had run
through a few of the Teensy audio examples. Within a couple of hours, I
had my first working DSP filter – very cool!
