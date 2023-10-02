# Node Red Serial

Send command by arduino serial, process, and output.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Arduino Features

Features:

  1. Able to turn the LED on, off, or on and off by specified frequency and duty cycle.
  2. Able to set the colors of the LED.
  3. Able to produce PWM signal based on duty cycle.
  4. Able to recognize single click, double click, hold, and no click events.
  5. Able to output all the actions from the feature.

Commands:

  1. `ON` (Turn the LED on by changing analog values to 255, 255, 255 due to pullup LED)
  2. `OFF` (Turn the LED off by changing analog values to 0, 0, 0 due to pullup LED)
  3. `BLINK` (Blink the LED by turning on and off specified by duty cycle and frequency. Default to 50% duty cycle and 2 Hz frequency)
  4. `SET_COLOR ${RED} ${GREEN} ${BLUE}` (Set the colors of the LED when it is on. Range from 0-255 Integer only)
  5. `RESET_COLOR` (Reset the colors of the LED when it is on to 255, 255, 255)
  6. `FREQ ${FREQUENCY}` (Set the frequency of the LED. Range in non-negative integer)
  7. `DUTY_CYCLE ${DUTY_CYCLE%}` (Set the duty cycle of the LED and set PWM signal duty cycle. Range in 0-100 Integer only)

Arduino Output:

  - `IORAW $(EVENT_TYPE) $(VALUE1) $(VALUE2) $(VALUE3) ...`

  1. `IORAW BUTTON $(STATE)` (Output pullup button states. 0 for press, 1 for not pressed)
  2. `IORAW ANALOG $(VALUE)` (Output analog value from analog input when two analog input difference is greater than 10 and output every 100 ms. Range from 0-1023)

  - `$(PREFIX) $(EVENT_DIALOGUE)`

  3. `IOPRO Single Click` (Output single click event)
  4. `IOPRO Double Click` (Output double click event)
  5. `IOPRO Long Press` (Output long press event)
  6. `IOPRO No Click` (Output no click event)

  - `DEBUG $(EVENT_TYPE) $(DIALOGUE)`

  7. `DEBUG EVENT_DOUBLE_CLICK Double Click Timeout` (Output when click time difference is exceed double click interval)
  8. `DEBUG EVENT_LONG_PRESS Long Press Timeout` (Output when unclicked and still waiting for long press event)

  - `STATUS $(EVENT_TYPE) $(VALUE1) $(VALUE2) $(VALUE3) ...`

  9. `STATUS LED ON` (Output when the LED is on)
  10. `STATUS LED OFF` (Output when the LED is off)
  11. `STATUS FREQ $(FREQUENCY)` (Output the frequncy when the frequency command is called)
  12. `STATUS COLORS $(RED) $(GREEN) $(BLUE)` (Output the colors RGB values when the colors-associated command is called)
  13. `STATUS HIGH_DUTY_CYCLE $(HIGH_DUTY_CYCLE)` (Output the high signal duty cycle when duty cycle command is called)
  14. `STATUS LOW_DUTY_CYCLE $(LOW_DUTY_CYCLE)` (Output the low signal duty cycle when duty cycle command is called)

Things you need to do:

  1. Set all input pins to your situation

## ESP32 Features

Features:

  1. Able to receive analog input
  2. Able to receive digital input
  3. Able to recongise clicks event
  4. Able to output events through MQTT protocol

Analog Event:

  1. `IORAW ANALOG $(VALUE)` (Output analog value from analog input when two analog input difference is greater than 10 and output every 100 ms. Range from 0-1023)

Digital Event:

  1. `IORAW BUTTON $(STATE)` (Output pullup button states. 0 for press, 1 for not pressed)
  2. `IOPRO Single Click` (Output single click event)
  3. `IOPRO Double Click` (Output double click event)
  4. `IOPRO Long Press` (Output long press event)
  5. `IOPRO No Click` (Output no click event)

Things you need to do:

  1. You need to setup your WiFi
  ```cpp
  #define WIFI_STA_NAME "Your WiFi's Name"
  #define WIFI_STA_PASS "Your WiFi's Password"
  ```
  2. You need to setup MQTT to connect (This version have no username and password)
  ```cpp
  #define MQTT_SERVER "MQTT Broker's Address"
  #define MQTT_PORT "MQTT Broker's Port" // integer
  #define client_name "esp32"
  #define LEDSIGNALS "Topic"
  ```