

#include "MIDIUSB.h"


const int  buttonPin[] = {6, 7, 8, 9, 10, 14, 15, 16}; // the pin that the pushbutton is attached to
bool buttonState[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; // current state of the button
bool lastButtonState [] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}; // previous state of the button


//Global Variables
int sensorPin[] = {0, 1, 2, 3};     //pin number to use the ADC
int analogValue[] = {0, 0, 0, 0};   //initialization of sensor variable, equivalent to EMA Y
int lastAnalogValue[] = {128, 128, 128, 128};




void setup() {
  for (int button = 0; button < 8; button++) {
    pinMode(buttonPin[button], INPUT_PULLUP);
  }
  pinMode(3, OUTPUT);
}


void sendControlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void loop() {

  midiEventPacket_t rx;

  rx = MidiUSB.read();
  if (rx.header != 0) {
    if (rx.byte3 == 127) {
      digitalWrite(rx.byte2, HIGH);
    }
    if (rx.byte3 == 0) {
      digitalWrite(rx.byte2, LOW);
    }
  }

  for (int button = 0; button < 8; button++) {

    buttonState[button] = digitalRead(buttonPin[button]);

    // compare the buttonState to its previous state
    if (buttonState[button] != lastButtonState[button]) {
      // if the state has changed, increment the counter
      if (buttonState[button] == HIGH) {

        sendControlChange(0, button + 16, 127);
        //MidiUSB.flush();
      }
      else {
        // if the current state is LOW then the button
        // wend from on to off:
        sendControlChange(0, button + 16, 0);
        //MidiUSB.flush();
      }
    }
    // save the current state as the last state,
    //for next time through the loop
    lastButtonState[button] = buttonState[button];
  }

  for (int slider = 0; slider < 4; slider++) {
    analogValue[slider] = analogRead(sensorPin[slider]) / 8;  //read the sensor value using ADC


    if ((analogValue[slider] - lastAnalogValue[slider]) > 1 || (analogValue[slider] - lastAnalogValue[slider]) < -1)

    {
      // value changed?
      if (analogValue[slider] != lastAnalogValue[slider]) {
        sendControlChange(0, slider + 12, analogValue[slider]);
        lastAnalogValue[slider] = analogValue[slider];
      }
    }
  }

  MidiUSB.flush();

  delay(5);                              //20ms delay
}
