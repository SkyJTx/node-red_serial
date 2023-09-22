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
const int doubleClickInterval = 250;
bool noClick = false;
bool isWaitingForDoubleClick = false;
bool isWaitingForLongPress = false;
bool isSingleClickLastEvent = false;
bool isDoubleClickLastEvent = false;
bool isLongPressLastEvent = false;

int analogPin = A0;
int analogInp = 0;
int prevAnalogInp = 0;

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
  pinMode(analogPin, INPUT); // Analog INPUT for Potentiometer
  pinMode(buttonPin, INPUT_PULLUP); // Digital Input of Pullup Switch
  pinMode(5, OUTPUT); // Analog OUTPUT for PWM

  blinkMillis = millis();
  buttonMillis = millis();
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
    Serial.println("STATUS LED ON");
    stringCommand = "";
  }
  else if (cmd == "OFF") {
    for (int i = 0; i < 3; i++) {
      currentColors[i] = offColors[i];
      analogWrite(ledPins[i], currentColors[i]);
    }
    isOn = false;
    Serial.println("STATUS LED OFF");
    stringCommand = "";
  }

  else if (cmd == "BLINK") {
    if ((!isOn) && (!blinkDelay)) {
      Serial.println("STATUS LED ON");
      for (int i = 0; i < 3; i++) {
        currentColors[i] = onColors[i];
        analogWrite(ledPins[i], currentColors[i]);
      }
      isOn = !isOn;
      blinkDelay = !blinkDelay;
      blinkMillis = millis();
    }
    else if ((isOn) && (!blinkDelay)) {
      Serial.println("STATUS LED OFF");
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
      Serial.println("STATUS FREQ " + (String)freq);
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
      Serial.println("STATUS HIGH_DUTY_CYCLE " + (String)dutyCycle);
      Serial.println("STATUS LOW_DUTY_CYCLE " + (String)(1-dutyCycle));
      onDelay = (int)(((float)dlay)*dutyCycle);
      offDelay = (int)(((float)dlay)*(1-dutyCycle));
      stringCommand = "BLINK";
    }
    analogWrite(5, (int)(((float)dCyc)/100*255));
  }

  analogInp = analogRead(analogPin);
  if ((abs(analogInp - prevAnalogInp) > 10)) {
    prevAnalogInp = analogInp;
    Serial.println("IORAW ANALOG " + (String)prevAnalogInp);
  }

  stateButton = digitalRead(4);
  if (stateButton != prevStateButton) {
    if (millis() - buttonMillis > debounceDelay) {
      noClick = false;
      prevStateButton = stateButton;
      Serial.println("IORAW BUTTON " + (String)prevStateButton);
      buttonMillis = millis();
      if (stateButton == LOW) {
        if (!isWaitingForLongPress) {
          isWaitingForLongPress = true;
        }
        if (!isWaitingForDoubleClick) {
          isWaitingForDoubleClick = true;
        }
        if (millis() - lastClickTime < doubleClickInterval && isWaitingForDoubleClick) {
          Serial.println("IOPRO BUTTON \"Double Click\"");
          isWaitingForDoubleClick = false;
          isSingleClickLastEvent = false;
          isDoubleClickLastEvent = true;
          isLongPressLastEvent = false;
          // Code for Double Click
        }
        lastClickTime = millis();
      }
    }
  }
  if (stateButton == LOW && millis() - buttonMillis > longPressDuration && !isLongPressLastEvent) {
    Serial.println("IOPRO BUTTON \"Long Press\"");
    isLongPressLastEvent = true;
    isWaitingForLongPress = false;
    isSingleClickLastEvent = false;
    isDoubleClickLastEvent = false;
    // Code for Long Press
  }
  if (stateButton == HIGH && !isDoubleClickLastEvent && !isWaitingForDoubleClick && !isWaitingForLongPress && !noClick && !isSingleClickLastEvent) {
    Serial.println("IOPRO BUTTON \"Single Click\"");
    isSingleClickLastEvent = true;
    isDoubleClickLastEvent = false;
    isLongPressLastEvent = false;
    // Code for Single Click
  }
  if (stateButton == HIGH && millis() - buttonMillis > 1000 && !noClick) {
    Serial.println("IOPRO BUTTON \"No Click\"");
    noClick = true;
    isWaitingForDoubleClick = false;
    isWaitingForLongPress = false;
    isSingleClickLastEvent = false;
    isDoubleClickLastEvent = false;
    isLongPressLastEvent = false;
    // Code for No Click
  }
  if (stateButton == HIGH && millis() - buttonMillis > doubleClickInterval && isWaitingForDoubleClick) {
    isWaitingForDoubleClick = false;
  }
  if (stateButton == HIGH && isWaitingForLongPress) {
    isWaitingForLongPress = false;
  }
}