# Watch Winder Project

## Overview
The goal of the project is to create a watch winder using a 28byj-48 stepper with a ULN2003 driver running for a selectable number of turns every day (24 hours). The turns are distributed during the day and will happen in 12 periods of 10 minutes, separated by a long pause. For example: 650 turns per day, divided by 12 periods means that the stepper will complete 54.2 (rounded up to 55) turns in 10 minutes then rest for 1 hour and 50 minutes. During the rest time the stepper will run again 4 times, completing 1/4 of turn each time in 1 second, evenly separated, in order to improve the watch positional accuracy. This basically means that the pause period is actually divided in 4 subperiods ot time equal to 1/4 of the pause period minus 1 second, at the end of each subperiod the stepper run. 

## Features
- Selectable number of turns per day: 650, 800, and 950
- Selectable direction: clockwise, counterclockwise, and both
- One LED that stays on during stepper running period and blinks slowly during pause time

## Main Cycle
The exection will loop 12 times the "Main cicle" in the span of 24h. Each iteration of the "main cicle" is 2 hours long and it is divided in the following parts:
- Main rotation: 10 minutes long
- Pause cicle: 1 hour 50 minutes long

### Main Rotation

- Rotate ceil(TPD/12) turns in clockwise direction in 10 minutes OR
- Rotate ceil(TPD/12) turns in counterclockwise direction in 10 minutes OR
- Rotate (ceil(TPD/12))/2 turns in counterclockwise diretion in 5 minutes then Rotate (TPD/12)/2 turns in clockwise diretion in 5 minutes

### Pause Cycle
The "Pause cicle" is a cicle made like this:
Repeat 4 times:
- Pause for  1649 seconds
- Rotate stepper fo 1/4 of turn in 1 second.

## Hardware
This code shall be implemented on an Arduino Nano 3 Every and a 28byj-48 stepper with a ULN2003 driver. The stepper will operate in half step mode.
The ULN2003 is connected to pin 3,4,5,6 of the Arduino Board.
There are also 2 ON-OFF-ON switches that the user will use for selections.
First Switch (Switch1) has one terminal ON connected to pin 7 on board, one terminal ON connected to pin 8 and the OFF terminal connected to ground
Second Switch (Switch2) has one terminal ON connected to pin 9 on board, one terminal ON connected to pin 10 and the OFF terminal connected to ground
Pins for the switches will use input_pullup, so that the pin will read LOW when the belonging terminal is selected.
The LED is connected to pin 2 on the board.

## Code Structure
The loop() part must be very lean, the real action must happen in external functions called in the loop.
The code MUST be NON-BLOCKING in all parts in order to fast react to user changes. 
The exection is orchestrated using time intervals checking current millis(), and via a conditional cascade of IF statements.

### Libraries
The following libraries are used in the code:
- `math.h`
- `AccelStepper.h`

### Variables

- `stepperPreviousMillis`: variable to store the time when the stepper last moved
- `stepperSteps`: variable to store the number of steps the stepper has completed
- `mainRotationPreviousMillis`: variable to store the time when the main rotation last started
- `mainRotationSpeed`: variable to store the speed of the main rotation, in milliseconds. The default value is 10 minutes in milliseconds (600000)
- `subPausePreviousMillis`: variable to store the time when the pause cycle last started
- `subPauseDuration`: variable to store the duration of the pause cycle, in milliseconds. The default value is 1649 seconds in milliseconds (1649000)
- `fastStepperDuration`: variable to store the duration of the fast stepper action, in milliseconds. The default value is 1 second in milliseconds (1000)
- `stepsFastRotation`: variable to store the number of steps for the fast stepper action. The default value is 1019
- `mainRotationState`: variable to store the state of the main rotation (0 = not started, 1 = running, 2 = ended)
- `subPauseState`: variable to store the state of the pause cycle (0 = not started, 1 = resting, 2 = fast stepper action)
- `pauseCycleCounter`: variable to store the number of times the pause cycle has been completed
- `stepsMainRotation`: variable to store the number of steps for the main rotation. The default value is 224180
- `direction`: variable to store the direction of the main rotation (1 = clockwise, -1 = counterclockwise, 0 = both)
- `fastStepper`: variable to store the number of steps for the fast stepper action. The default value is 1/4 of the total steps
- `prevTurnsPerDay`: variable to store the number of turns per day the user previously selected
- `prevDirection`: variable to store the direction the user previously selected
- `ledState`: variable to store the state of the LED (0 = off, 1 = on)
- `ledPreviousMillis`: variable to store the time when the LED state last changed

### Pins
- ULN2003: connected to pin 3, 4, 5, 6
- Switch1: connected to pin 7 and 8 with input_pullup, OFF terminal connected to ground
- Switch2: connected to pin 9 and 10 with input_pullup, OFF terminal connected to ground
- LED: connected to pin 2

## Conclusion
This project aims to create a watch winder using a 28byj-48 stepper motor with a ULN2003 driver, the user can select the number of turns per day and the direction of the rotation. The code is designed to be non-blocking and the `millis()` function is used for creating non-blocking delays and loops. The code uses several libraries and variables to track the execution and the hardware connections are clearly defined.

**Note:** Both the description and the code have been generated with the help of ChatGPT.

