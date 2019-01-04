# Master Clock

## Features
This module features tap tempo, 1x, 2x, 3x, 4x, 1/2x, 1/3x, and 1/4x clock multiplication/division, and the ability to dial in the clock rate using a potentiometer. A second mode disables tap tempo and allows rate to be adjusted directly using the potentiometer or allows the module to be used as a standard clock mutiplier/divider..

## Files
### Master_Clock_xx.ino
This is the Arduino code. An ISP header on the board allows the microcontroller to be programmed directly.

### masterClock_xx.brd and .sch
These are the Eagle files for the PCB. To order PCBs, gerbers need to be generated using a CAM job in Eagle. A free version of Eagle is available.

### masterClockParts_xx.md
This is the parts list. A Mouser BOM is available at in this file as well.

### masterClock.ai
This is the panel in Adobe Illustrator format. This can be used to manufacture a panel.

## Updates

### 01.1 03JAN19
The original code had major issues with timing. That code has been replaced with a test version of the code that keeps time correctly but only allows the tempo to be set using the potentiometer. All pin deffinitions were left in place to facilitate adding additonal code to meet the desired funsitonality. In additon, I have discovered the anode on LD1 (the bicolor LED) is not connected to VCC. This can be fixed with a jumper wire and will be corrected in the next board revition. 

### 01 24JUN18
First push to GitHub. Currently this design is untested.
