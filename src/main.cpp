#include <Arduino.h>

String stringCommand = "";
String prevStringCommand = "";
String cmd = "";
String var = "";
bool isOn = true;
int ledPins[] = {11, 9, 10};
unsigned int currentColors[] = {255, 255, 255};
unsigned int onColors[] = {0, 0, 0};
unsigned int offColors[] = {255, 255, 255};

unsigned int buttonPin = 4;
int stateButton = LOW;
int prevStateButton = LOW;
unsigned long buttonMillis = 0;
unsigned long lastClickTime = 0;
const int debounceDelay = 50;
const int longPressDuration = 500;
const int doubleClickInterval = 300;
bool longPress = false;
bool noClick = false;
bool isWaitingForDoubleClick = false;
bool isWaitingForLongPress = false;
bool isDoubleClickLastEvent = false;

int analogInp = 0;
int prevAnalogInp = 0;
unsigned long analogMillis = 0;

int dlay = 250;
float dutyCycle = 0.5;
unsigned long blinkMillis = 0;
bool blinkDelay = false;
unsigned long onDelay = (int)(((float)dlay)*dutyCycle);
unsigned long offDelay = (int)(((float)dlay)*(1-dutyCycle));

void setup() {
  Serial.begin(115200); // Set BAUD_RATE

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT); // Assign OUTPUT PINS for colors
  }

  pinMode(12, OUTPUT); // Digital OUTPUT for VCC 
  digitalWrite(12, HIGH); // OUTPUT VCC as HIGH
  pinMode(A0, INPUT); // Analog INPUT for Potentiometer
  pinMode(buttonPin, INPUT_PULLUP); // Digital Input of Pullup Switch
  pinMode(5, OUTPUT); // Analog OUTPUT for PWM

  blinkMillis = millis();
  buttonMillis = millis();
  analogMillis = millis();
}

String command(String cm) {
  int spacePos = cm.indexOf(' ');
  if (spacePos != -1) {
    return cm.substring(0, spacePos);
  }
  return cm;
}

String variable(String cm) {
  int spacePos = cm.indexOf(' ');
  if (spacePos != -1) {
    return cm.substring(spacePos + 1);
  }
  return "";
}

void loop() {
  if (Serial.available() > 0) {
    stringCommand = Serial.readStringUntil('\n');
  }

  if (prevStringCommand != stringCommand) {
    prevStringCommand = stringCommand;
    cmd = command(stringCommand);
    var = variable(stringCommand);
  }

  if (cmd == "ON") {
    for (int i = 0; i < 3; i++) {
      currentColors[i] = onColors[i];
      analogWrite(ledPins[i], currentColors[i]);
    }
    isOn = true;
  }
  else if (cmd == "OFF") {
    for (int i = 0; i < 3; i++) {
      currentColors[i] = offColors[i];
      analogWrite(ledPins[i], currentColors[i]);
    }
    isOn = false;
  }

  /*
  else if (cmd == "BLINK") {
    for (int i = 0; i < 3; i++) {
      if (isOn) {
        currentColors[i] = onColors[i];
      }
      else {
        currentColors[i] = offColors[i];
      }
      analogWrite(ledPins[i], currentColors[i]);
    }
    if (isOn) {
      delay((int)(((float)dlay)*dutyCycle));
    }
    else {
      delay((int)(((float)dlay)*(1-dutyCycle)));
    }
    isOn = !isOn;
  }
  */

  else if (cmd == "BLINK") {
    if ((!isOn) && (!blinkDelay)) {
      for (int i = 0; i < 3; i++) {
        currentColors[i] = onColors[i];
        analogWrite(ledPins[i], currentColors[i]);
      }
      isOn = !isOn;
      blinkDelay = !blinkDelay;
      blinkMillis = millis();
    }
    else if ((isOn) && (!blinkDelay)) {
      for (int i = 0; i < 3; i++) {
        currentColors[i] = offColors[i];
        analogWrite(ledPins[i], currentColors[i]);
      }
      isOn = !isOn;
      blinkDelay = !blinkDelay;
      blinkMillis = millis();
    }

    if (isOn && ((millis() - blinkMillis) >= onDelay)) {
      blinkDelay = !blinkDelay;
    }
    else if (!isOn && ((millis() - blinkMillis) >= offDelay)) {
      blinkDelay = !blinkDelay;
    }
  }

  else if (cmd == "FREQ") {
    int spacePos = var.indexOf(' ');
    float freq;
    if (spacePos != 1) {
      freq = var.substring(0, spacePos).toFloat();
    }
    else {
      freq = var.toFloat();
    }
    if (freq == 0) {
      stringCommand = "OFF";
    }
    else {
      freq = 1/freq*1000;
      dlay = (int)freq;
      onDelay = (int)(((float)dlay)*dutyCycle);
      offDelay = (int)(((float)dlay)*(1-dutyCycle));
      stringCommand = "BLINK";
    }
  }

  else if (cmd == "SET_COLOR") {
    int spacePos;
    for (int i = 0; i < 3; i++) {
      spacePos = var.indexOf(' ');
      if (spacePos != 1) {
        onColors[i] = 255 - var.substring(0, spacePos).toInt();
        var = var.substring(spacePos + 1);
      }
      else {
        onColors[i] = 255 - var.toInt();
      }
    }
    stringCommand = "ON";
  }

  else if (cmd == "RESET_COLOR") {
    for (int i = 0; i < 3; i++) {
      onColors[i] = 0;
    }
    stringCommand = "ON";
  }

  else if (cmd == "DUTY_CYCLE") {
    int spacePos = var.indexOf(' ');
    int dCyc;
    if (spacePos != -1) {
      dCyc = var.substring(0, spacePos).toInt();
    }
    else {
      dCyc = var.toInt();
    }
    if (dCyc == 0) {
      stringCommand = "OFF";
    }
    else {
      dutyCycle = ((float)dCyc)/100;
      onDelay = (int)(((float)dlay)*dutyCycle);
      offDelay = (int)(((float)dlay)*(1-dutyCycle));
      stringCommand = "BLINK";
    }
    analogWrite(5, (int)(((float)dCyc)/100*255));
  }

  analogInp = analogRead(A0);
  if ((abs(analogInp - prevAnalogInp) > 10) && ((millis() - analogMillis) > 1)) {
    prevAnalogInp = analogInp;
    Serial.println("IORAW ANALOG " + (String)prevAnalogInp);
    analogMillis = millis();
  }

  /*
  stateButton = digitalRead(buttonPin);
  if ((stateButton != prevStateButton) && ((millis() - buttonMillis) > 1)) {
    prevStateButton = stateButton;
    Serial.println("IORAW BUTTON " + (String)prevStateButton);
    buttonMillis = millis();
  }
  */

  stateButton = digitalRead(4);
  if (stateButton != prevStateButton) {
    if (millis() - buttonMillis > debounceDelay) {
      longPress = false;
      noClick = false;
      prevStateButton = stateButton;
      Serial.println("IORAW BUTTON " + (String)prevStateButton);
      buttonMillis = millis();
      if (stateButton == LOW) {
        if (!isWaitingForLongPress) {
          isWaitingForLongPress = true;
        } if (!isWaitingForDoubleClick) {
          isWaitingForDoubleClick = true;
        } if (millis() - lastClickTime < doubleClickInterval && isWaitingForDoubleClick) {
          Serial.println("IOPRO BUTTON \"Double Click\"");
          isWaitingForDoubleClick = false;
          isDoubleClickLastEvent = true;
        }
        lastClickTime = millis();
      }
      else {
        if ((isWaitingForLongPress || (isWaitingForDoubleClick && !isDoubleClickLastEvent))) {
          Serial.println("IOPRO BUTTON \"Single Click\"");
          isWaitingForDoubleClick = false;
          isWaitingForLongPress = false;
          isDoubleClickLastEvent = false;
        }
      }
    }
  }
  if (stateButton == LOW && millis() - buttonMillis > longPressDuration && !longPress) {
    Serial.println("IOPRO BUTTON \"Long Press\"");
    longPress = true;
    isWaitingForLongPress = false;
    isWaitingForDoubleClick = false;
    isDoubleClickLastEvent = false;
  }
  if (stateButton == HIGH && millis() - buttonMillis > 500 && !noClick) {
    Serial.println("IOPRO BUTTON \"No Click\"");
    noClick = true;
    isWaitingForDoubleClick = false;
    isWaitingForLongPress = false;
    isDoubleClickLastEvent = false;
  }
}