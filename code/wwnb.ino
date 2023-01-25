#include <math.h>
#include <AccelStepper.h>

#define HALFSTEP 8

unsigned long stepperPreviousMillis = 0; // initialization of t0 for stepper
int stepperSteps = 0; // initialization of stepper count
unsigned long mainRotationPreviousMillis = 0; // initialization of t0 for main rotation cycle
const unsigned long mainRotationDuration = 360000; // duration of main rotation: 6 minutes in milliseconds (changed to speed up, originally was 10 minutes)
unsigned long subPausePreviousMillis = 0;
const unsigned long subPauseDuration = 2309999; //1 hours 54 seconds in milliseconds (changed as main rotation duration decreased)
const unsigned long fastStepperDuration = 1000; //1 second in milliseconds
const int stepsFastRotation = 1019; // steps equivalent to 0.25 turns in half mode
int mainRotationState = 0; // initialize main rotation state
int subPauseState = 0; // initialize sub pasue state
int pauseCycleCounter = 0; // initialize counter of pause cycle
int stepsMainRotation = 224180; // initialize main rotation steps
int directionSense = 1; // initialize direction
int ledState = 0; // initalize led state
unsigned long ledPreviousMillis = 0; // initialize of t0 for led
const unsigned long ledOnTime = 250; // in milliseconds
const unsigned long ledOffTime = 250; // in milliseconds
volatile bool resetLoop = false; // variable to check if an interrupt is going to reset the loop
int* userInput; // variable to get inputs from user
const int stepsPerRevolution = 2038*2; // for 28BYJ-48 stepper in half step mode

const int switch1Pin1 = 7; // pin for switch 1 position 1
const int switch1Pin2 = 8; // pin for switch 1 position 2
const int switch2Pin1 = 9; // pin for switch 2 position 1
const int switch2Pin2 = 10; // pin for switch 2 position 2

const int ledPin = 2; // pin for LED

AccelStepper stepper(HALFSTEP, 3, 5, 4, 6);

void setup() {
  pinMode(switch1Pin1, INPUT_PULLUP);
  pinMode(switch1Pin2, INPUT_PULLUP);
  pinMode(switch2Pin1, INPUT_PULLUP);
  pinMode(switch2Pin2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(switch1Pin1), handleInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(switch1Pin2), handleInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(switch2Pin1), handleInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(switch2Pin2), handleInterrupt, CHANGE);
}

void loop() {

  if (resetLoop == true) {
    resetLoop == false;
    return;
  }
  // Get user input
  handleUserInput();

  // Start Main Rotation
  handleMainRotation();

  // Start Pause cycle
  handlePauseCycle();
}

void handleInterrupt() {
    mainRotationState = 0;
    subPauseState = 0;
    pauseCycleCounter = 0;
    resetLoop = true;
}

void handleUserInput() {
  stepsMainRotation = userInput[0];
  directionSense = userInput[1];
}

void handleMainRotation() {
  if (mainRotationState == 0) {
    runStepper(stepsMainRotation, mainRotationDuration, directionSense);
    controlLED(mainRotationState);
    mainRotationState = 1;
  } else if (mainRotationState == 1) {
    unsigned long currentMillis = millis();
    if (currentMillis - mainRotationPreviousMillis >= mainRotationDuration) {
      mainRotationState = 2;
      mainRotationPreviousMillis = currentMillis;
    }
  }
}

void handlePauseCycle() {
  if (mainRotationState == 2) {
    // Repeat sub-pause pattern 4 times
    while (pauseCycleCounter < 4) {
      // Start sub-pause (resting)
      if (subPauseState == 0) {
        controlLED(subPauseState);
        subPauseState = 1;
        subPausePreviousMillis = millis();
      }
      // Check if sub-pause (resting) is ended
      if (subPauseState == 1) {
        unsigned long currentMillis = millis();
        if (currentMillis - subPausePreviousMillis >= subPauseDuration) {
          subPauseState = 2;
          subPausePreviousMillis = currentMillis;
          }
        }
  // Start fast stepper action
    if (subPauseState == 2) {
      runStepper(stepsMainRotation, fastStepperDuration,1);
      controlLED(1);
      subPauseState = 3;
      subPausePreviousMillis = millis();
      }
    // Check if fast stepper action is ended
    if (subPauseState == 3) {
    unsigned long currentMillis = millis();
      if (currentMillis - subPausePreviousMillis >= fastStepperDuration) {
        subPauseState = 0;
        pauseCycleCounter++;
        }
      }
    }
  // End of pause cycle
  mainRotationState = 0;
  pauseCycleCounter = 0;
  }
}


int* checkUserInput() {
  // Read switch inputs and update user selections
  int switch1Pin1 = digitalRead(7);
  int switch1Pin2 = digitalRead(8);
  int switch2Pin1 = digitalRead(9);
  int switch2Pin2 = digitalRead(10);
  int* values;
  // Switch1:
  if (switch1Pin1 == LOW) {
    values[0] = ceil(650/12.0)*stepsPerRevolution; //650 turns per day, divided in 12 interval and trasformed in steps per interval
  } else if (switch1Pin2 == LOW) {
    values[0] = ceil(950/12)*stepsPerRevolution; //950 turns per day, divided in 12 interval and trasformed in steps per interval
  } else {
    values[0] = ceil(800/12)*stepsPerRevolution; //800 turns per day, divided in 12 interval and trasformed in steps per interval
  }

  // Switch2:
  if (switch2Pin1 == LOW) {
    values[1] = 1; //clockwise
  } else if (switch2Pin2 == LOW) {
    values[1] = 0; //both
  } else {
    values[1] = -1; //counterclockwise
  }

  return values;
}

void runStepper(int steps, unsigned long millisToRotate, int directionSense) {
  // Get current time
  unsigned long currentMillis = millis();
  long int stepperDirection;
  unsigned long stepperSpeed = millisToRotate/steps;

  if (directionSense == 0) {
    if (stepperSteps < steps / 2) {
      stepperDirection = 1;
    } else {
      stepperDirection = -1;
    }
    } else {
    stepperDirection = directionSense;
  }
  // Check if it's time to take a step

  if (currentMillis - stepperPreviousMillis >= stepperSpeed) {
        stepper.move(stepperDirection);
        stepper.runToPosition();
    }
    stepperSteps++;
    stepperPreviousMillis = currentMillis;


  // Check if we've reached the desired number of steps
  if (stepperSteps >= steps) {
    stepperSteps = 0;
    stepperPreviousMillis = 0;
  }
}

void controlLED(int statePar) {
  // Get current time
  unsigned long currentMillis = millis();

  if (statePar == 1) {
    // Fixed on LED when stepper is running
    digitalWrite(ledPin, HIGH);
  }
  else if (statePar == 0) {
    // Breathing blinking LED during sub-pause
    if (ledState == 0) {
      digitalWrite(ledPin, HIGH);
      ledState = 1;
    } else {
      if (currentMillis - ledPreviousMillis >= ledOnTime) {
        digitalWrite(ledPin, LOW);
        ledState = 2;
        ledPreviousMillis = currentMillis;
      }
    }

    if (ledState == 2) {
      if (currentMillis - ledPreviousMillis >= ledOffTime) {
        digitalWrite(ledPin, HIGH);
        ledState = 1;
        ledPreviousMillis = currentMillis;
      }
    }
  }
  else {
    digitalWrite(ledPin, LOW);
  }
}
