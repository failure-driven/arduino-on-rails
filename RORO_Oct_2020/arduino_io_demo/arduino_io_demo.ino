#include <Arduino.h>    // standard Arduino libraty

/*
   Demo of a number of Arduino I/O (input and output) features.

   A button connected to interrupt pin 2 switches through a number of demos:
     1) flashing red LED
     2) flashing green LED
     3) flashing blue LED
     4) blink external LED
     5) pulsing green LED
     6) pulsing red/green/blue LED
     7) music
     8) temperature reading using LM335
     9) servo sweep demo
     10) ultrasonic distance and output on LED display
*/
enum {
  OFF,
  RED_BLINK,
  GREEN_BLINK,
  BLUE_BLINK,
  BRIGHT_BLINK,
  GREEN_PULSE,
  ALL_PULSE,
  MUSIC,
  TEMP,
  SWEEP,
  DIST_DISPLAY
} mode;

/*
   switching between modes
*/
const int switchPin = 2;    // interrupt pin
const int debounceDelay = 100;  // DEBOUNCE delay

long lastDebounceTime = 0;
int lastSwitchState = HIGH;

void changeState();

// as triggered by interrupt on pin 2
void change() {
  int switchState = digitalRead(switchPin);
  if (switchState != lastSwitchState) {
    lastDebounceTime = millis();
  }
  if ((millis - lastDebounceTime) > debounceDelay) {
    if (switchState != lastSwitchState) {
      lastSwitchState = switchState;
      if (switchState == LOW) {
        changeState();
      }
    }
  }
}

/*
   blinking LED's
*/
const int ledPinRed = 11;   // RED_BLINK
const int ledPinBlue = 3;   // BLUE_BLINK
const int ledPinGreen = 5;  // GREEN_BLINK
const int ledPinBright = 8; // BRIGHT_BLINK
const int period = 200;     // BLINK period

int ledState = LOW;
long lastChangeTime = 0;

void handleBlink(int ledPin) {
  long now = millis();
  if (now > lastChangeTime + period) {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    lastChangeTime = now;
  }
}

void pulse(int ledPin, int delay) {
  long now = millis();
  int fadeModulus = ((now / delay) % 512);
  Serial.print(fadeModulus);

  int fadeOffset = fadeModulus - 255;
  Serial.print(", ");
  Serial.print(fadeOffset);

  int fadeAmount = abs(fadeOffset);
  Serial.print(", ");
  Serial.println(fadeAmount);

  analogWrite(ledPin, fadeAmount);
}

void pulseAll(int ledPin1, int ledPin2, int ledPin3) {
  pulse(ledPin1, 5);
  pulse(ledPin2, 50);
  pulse(ledPin3, 25);
}

/*
   music
*/
#include "pitches.h"        // pitches
#include "tunes.h"          // tunes

const int speakerPin = 6;   // MUSIC

const int songspeed = 1.5;
void playMelody() {
//  int melodyLength = sizeof(durationsPirates) / sizeof(int);
//  for (int i = 0; i < melodyLength && mode == MUSIC; i++) {
//    int wait = durationsPirates[i] * songspeed;
//    tone(speakerPin, notesPirates[i], wait); // tone(pin,frequency,duration)
//    delay(wait);
//  }
  int melodyLength = sizeof(durationsInspectorGadget) / sizeof(int);
  for (int i = 0; i < melodyLength && mode == MUSIC; i++) {
    // e.g. quarter note = 1000 / 4, eighth note = 1000 / 8, etc.
    int wait = 1000 / durationsInspectorGadget[i] * songspeed;
    tone(speakerPin, notesInspectorGadget[i], wait); // tone(pin,frequency,duration)
    delay(wait);
  }
}

/*
   servo sweep
*/
#include <Servo.h>          // external library to control a servo
#define SWEEP_PERIOD 1000   // 1 second sweep period for servo demo

const int servoPin = 4;     // SWEEP servo

Servo myservo;
//int servoSweepCount = 0;

void sweepServo() {
  int millisPosition = millis() % SWEEP_PERIOD;
  double floatPosition = TWO_PI * (((float) millisPosition ) / SWEEP_PERIOD);
  int servoPosition = (70 * sin(floatPosition)) + 90;
  Serial.println(servoPosition);
  myservo.write(servoPosition);
}

/*
   LM335 temperature
*/
const int LM335_pin = A0;   // LM335 TEMP

void handleTemp() {
  // 0.489 = 500/1023
  //   10mV for the range 0 - 5V
  //   via 10bit ADC 2^10 1,024 0 - 1,023
  float temp_val = analogRead(LM335_pin) * 0.489;

  // Convert Kelvin to degree Celsius
  float temp_val_celsius = temp_val - 273.16;
  Serial.print(temp_val_celsius);
  Serial.println(" Degree Celsius");
  delay(1000);
}

/*
   Ultrasonic distance finder
*/
const int pingPin = 9;      // DIST ping
const int echoPin = 10;     // DIST echo

long measureDistance() {
  delay(100);
  long duration, inches, cm;
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  return cm;
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

/*
   8x8 LED matrix display
*/
#include <LedControl.h> // external library to control LED display
#include "digits.h"     // library of 0-9 digits on 8x8 display
const int dataPin = 12;
const int clkPin = 13;
const int csPin = 7;

LedControl ledControl = LedControl(dataPin, clkPin, csPin, 1);

void writeValues(byte* values) {
  for (int row = 0; row < 8; row++) {
    ledControl.setRow(0, row, values[row]);
  }
}

/*
   measure distance, display it on LED matrix and via servo
*/
void handleDistDisplay() {
  long distance = measureDistance();
  if (distance >= 0 && distance <= 20) {
    myservo.write((int)(distance / 20.0 * 180));
  }
  if (distance >= 0 && distance <= 9) {
    writeValues(matrixNumeral[distance]);
  }
}

int oneShot = true;         // only play through tune once
// Reset all the things between states
void reset() {
  oneShot = true;
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinGreen, HIGH);
  digitalWrite(ledPinBlue, HIGH);
  digitalWrite(ledPinBright, LOW);
  digitalWrite(speakerPin, LOW);
  myservo.write(0);
}

void changeState() {
  if (mode == DIST_DISPLAY) {
    reset();
    Serial.println("Temp display to off");
    mode = OFF;
  } else if (mode == SWEEP) {
    reset();
    Serial.println("Sweep to dist display");
    mode = DIST_DISPLAY;
  } else if (mode == TEMP) {
    reset();
    Serial.println("Temp to sweep");
    mode = SWEEP;
  } else if (mode == MUSIC) {
    reset();
    Serial.println("Music to temp");
    mode = TEMP;
  } else if (mode == ALL_PULSE) {
    reset();
    Serial.println("pulse all to Music");
    mode = MUSIC;
  } else if (mode == GREEN_PULSE) {
    reset();
    Serial.println("pulse green to pulse all");
    mode = ALL_PULSE;
  } else if (mode == BRIGHT_BLINK) {
    reset();
    Serial.println("bright to pulse green");
    mode = GREEN_PULSE;
  } else if (mode == BLUE_BLINK) {
    reset();
    Serial.println("blue to bright");
    mode = BRIGHT_BLINK;
  } else if (mode == GREEN_BLINK) {
    reset();
    Serial.println("green to blue");
    mode = BLUE_BLINK;
  } else if (mode == RED_BLINK) {
    Serial.println("red to green");
    reset();
    mode = GREEN_BLINK;
  } else if (mode == OFF) {
    Serial.println("off to red");
    reset();
    mode = RED_BLINK;
  }
}

/*
   default setup method
*/
void setup() {
  myservo.attach(servoPin);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledPinBright, OUTPUT);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(switchPin), change, CHANGE);
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  ledControl.shutdown(0, false);
  /* Set the brightness to a medium values */
  ledControl.setIntensity(0, 8);
  /* and clear the display */
  ledControl.clearDisplay(0);
  reset();
}

/*
   default loop that jumps into the appropriate mode with a delay of 20ms per loop
*/
void loop() {
  if (mode == OFF) {
    reset();
  } else if (mode == RED_BLINK) {
    handleBlink(ledPinRed);
  } else if (mode == BLUE_BLINK) {
    handleBlink(ledPinBlue);
  } else if (mode == GREEN_BLINK) {
    handleBlink(ledPinGreen);
  } else if (mode == BRIGHT_BLINK) {
    handleBlink(ledPinBright);
  } else if (mode == GREEN_PULSE) {
    pulse(ledPinBlue, 5);
  } else if (mode == ALL_PULSE) {
    pulseAll(ledPinRed, ledPinGreen, ledPinBlue);
  } else if (mode == MUSIC) {
    if (oneShot) {
      playMelody();
      oneShot = false;
    }
  } else if (mode == SWEEP) {
    sweepServo();
  } else if (mode == TEMP) {
    handleTemp();
  } else if (mode == DIST_DISPLAY) {
    handleDistDisplay();
  }
  delay(20);
}
