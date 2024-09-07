/*
   Description: This program is for the Robot Remote Control. It takes input from joysticks, buttons, and potentiometers and transmits that data to a receiver using an NRF24l01 radio transceiver. 

        Wiring: The required components are an Arduino Nano, NRF24L01 module, 2x joysticks, 2x momentary switches, 2x potentiometers, a toggle switch, and a 4xAAA battery pack
        NRF24L01
            GND -> GND
            VCC -> 3V3
            CSN -> D10
            MOSI -> D11
            MISO -> D12
            SCK -> D13
            CE -> D9
        RIGHT JOYSTICK
            GND -> GND
            +5V -> 5V
            VRX -> A2
            VRY -> A3
            SW -> A1
        LEFT JOYSTICK
            GND -> GND
            +5V -> 5V
            VRX -> A5
            VRY -> A6
            SW -> A4
        RIGHT BUMPER
            C -> GND
            NO -> D2
        LEFT BUMPER
            C -> GND
            NO -> D3
        RIGHT POTENTIOMETER
            LEFT -> GND
            MIDDLE -> A0
            RIGHT -> 5V
        LEFT POTENTIOMETER
            LEFT -> GND
            MIDDLE -> A7
            RIGHT -> 5V
*/

#include "SPI.h"                           
#include "RF24.h"                           //Include libraries for NRF24L01 Radio Transciever
RF24 radio(9, 10);                          //CE, CSN
const byte chan[6] = "00007";               //Byte array representing the address. This is the address where we will send the data. This should be same on the receiving side.

int l_xJoy = A5;                            //Define joystick, button, and potentiometer connections
int l_yJoy = A6;
int l_joy = A4;
int r_xJoy = A2;
int r_yJoy = A3;
int r_joy = A1;
int l_bumper = 3;
int r_bumper = 2;
int l_pot = A7;
int r_pot = A0;

int l_xJoy_zero;                            //Create variable for joystick zero values
int l_yJoy_zero;
int r_xJoy_zero;
int r_yJoy_zero;

byte data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  Serial.println("Serial Communication Initialized");

  pinMode(l_xJoy, INPUT);                   //Set pin modes for joysticks, buttons, and potentiometers. Buttons defined as input pullups
  pinMode(l_yJoy, INPUT);
  pinMode(r_xJoy, INPUT);
  pinMode(r_yJoy, INPUT);
  pinMode(l_joy, INPUT_PULLUP);
  pinMode(r_joy, INPUT_PULLUP);
  pinMode(l_bumper, INPUT_PULLUP);
  pinMode(r_bumper, INPUT_PULLUP);
  pinMode(l_pot, INPUT);
  pinMode(r_pot, INPUT);

  radio.begin();                            //Starting the radio communication
  radio.openWritingPipe(chan);              //Setting the address to send data to
  radio.setPALevel(RF24_PA_MIN);  
  radio.stopListening();

  l_xJoy_zero = analogRead(l_xJoy);         //Initialize the joystick zero values (leave the joysticks in their neutral positions when turning on remote
  l_yJoy_zero = analogRead(l_yJoy);
  r_xJoy_zero = analogRead(r_xJoy);
  r_yJoy_zero = analogRead(r_yJoy);
}

void loop() {
  if (analogRead(l_xJoy) > (l_xJoy_zero - 10) && analogRead(l_xJoy) < (l_xJoy_zero + 10)) data[0] = 0;          //Setting joystick value to 0 if centered, otherwise mapping that value between 1 and 255
  else data[0] = map(analogRead(l_xJoy), 0, 1023, 1, 255);

  if (analogRead(l_yJoy) > (l_yJoy_zero - 10) && analogRead(l_yJoy) < (l_yJoy_zero + 10)) data[1] = 0;
  else data[1] = map(analogRead(l_yJoy), 0, 1023, 1, 255);

  data[2] = !digitalRead(l_joy);                                                                                //Setting button values to 1 if pressed
  data[3] = !digitalRead(l_bumper);

  if (analogRead(r_xJoy) > (r_xJoy_zero - 10) && analogRead(r_xJoy) < (r_xJoy_zero + 10)) data[4] = 0;
  else data[4] = map(analogRead(r_xJoy), 0, 1023, 1, 255);

  if (analogRead(r_yJoy) > (r_yJoy_zero - 10) && analogRead(r_yJoy) < (r_yJoy_zero + 10)) data[5] = 0;
  else data[5] = map(analogRead(r_yJoy), 0, 1023, 1, 255);

  data[6] = !digitalRead(r_joy);
  data[7] = !digitalRead(r_bumper);

  data[8] = map(analogRead(l_pot), 0, 1023, 0, 255);                                                            //Mapping potentiometer value between 0 and 255
  data[9] = map(analogRead(r_pot), 0, 1023, 0, 255);


  for (int i = 0; i < 10; i++) {                                                                                //Serial print data
    Serial.print(data[i]);
    Serial.print('\t');
  }
  Serial.println("");
  radio.write(&data, sizeof(data));                                                                             //Transmit data
  delay(10);
}
