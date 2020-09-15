#include <EasyTransfer.h>

#define AXES 6
#define BUTTONS 8

const long speedLimit = 10;

unsigned long lastSend = 0;

int buttonPins[BUTTONS] = { 2, 3, 4, 5, 6, 7, 8, 9 };
int analogPins[AXES] = { 16, 17, 18, 19, 20, 21 };

int axisMinima[AXES];
int axisMaxima[AXES];

EasyTransfer ET; 

struct SEND_DATA_STRUCTURE{
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t joyx;
  int16_t joyy;
  int8_t joyz;
  int16_t joyxr;
  int16_t joyyr;
  int8_t joyzr;
  uint32_t buttonState;
};

SEND_DATA_STRUCTURE controllerState;

void setup() {
  Serial.begin(2000000);
  ET.begin(details(controllerState), &Serial);
  for (int i = 0; i < BUTTONS; i++) { pinMode(buttonPins[i], INPUT_PULLUP); }
  for (int i = 0; i < AXES; i++) {
    pinMode(analogPins[i], INPUT);
    axisMinima[i] = 512;
    axisMaxima[i] = 768;
  }
}

uint32_t readButtons(){
  uint32_t buttonStateNow = 0;
  for (int i=0; i<BUTTONS; i++) { bitWrite(buttonStateNow, i, !digitalRead(buttonPins[i])); }
  return buttonStateNow;
}

void loop(){
  int analogValue[AXES];
  for (int i = 0; i < AXES; i++){
    analogValue[i] = analogRead(analogPins[i]);
    if (analogValue[i] > axisMaxima[i]) { axisMaxima[i] = analogValue[i]; }
    else if (analogValue[i] < axisMinima[i]) { axisMinima[i] = analogValue[i]; }

    if (i < 4) { analogValue[i] = map(analogValue[i], axisMinima[i], axisMaxima[i], -32768, 32767); }
    else { analogValue[i] = map(analogValue[i], axisMinima[i], axisMaxima[i], -128, 127); }
  }
  controllerState.joyx  = analogValue[0];
  controllerState.joyxr = analogValue[1];
  controllerState.joyy  = analogValue[2];
  controllerState.joyyr = analogValue[3];
  controllerState.joyz  = analogValue[4];
  controllerState.joyzr = analogValue[5];
  controllerState.buttonState = readButtons();
  if ((millis() - lastSend) > speedLimit ) {
    ET.sendData();
    lastSend = millis();
  }
}
