/*
*Author: Luis Luna
*Description: This program helps echo dot recognize the mcu8266 different "WeMo Devices".
*
*There are two sets of LED lights. The outer LED lights and inner LED lights.
*The outter LED lights have the flicker function that if enabled the lights will flicker.
*The inned LEDs only have the turn on/off function
*
*
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <Wire.h>

// prototypes
boolean connectWifi();

//on/off callbacks
void lightOneOn();
void lightOneOff();

void lightTwoOn();
void lightTwoOff();

void flickerModeOn();
void flickerModeOff();


// Change this before you flash
const char* ssid = "FTS Headquarters";
const char* password = "ac8182706890";

boolean wifiConnected = false;
boolean lightOne_state = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *lightOne = NULL;
Switch *lightTwo = NULL;
Switch *flicker = NULL;

// constants won't change :
const long interval = 100;               //interval at which to blink (milliseconds)
unsigned long previousMillis = 0;        //will store last time LED was updated

// Set Relay Pins
int relayOne =  14;
int relayOneOpp = 15;
int relayTwo = 16;
int ledState = HIGH;
int ledStateOpp = HIGH;


void setup()
{
  Serial.begin(115200);
   pinMode(LED_BUILTIN, OUTPUT);

  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected) {

    upnpBroadcastResponder.beginUdpMulticast();

    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    lightOne = new Switch("Light One", 80, lightOneOn, lightOneOff);
    lightTwo = new Switch("Light Two", 81, lightTwoOn, lightTwoOff);
    flicker = new Switch("Flicker", 82, flickerOn, flickerOff);


    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*lightOne);
    upnpBroadcastResponder.addDevice(*lightTwo);
    upnpBroadcastResponder.addDevice(*flicker);

    //Set relay pins to outputs
    pinMode(14, OUTPUT); //used to turn outter lights on
    pinMode(15, OUTPUT); //used to turn flicker mode on
    pinMode(16, OUTPUT); //used to turn inner lights on

    delay(2000);

    //Set each relay pin to HIGH this display status messagefor each relay
    digitalWrite(relayOne, HIGH);   // sets relayOne on
    Serial.println("Relay 1 ON");
    delay(500);

    digitalWrite(relayTwo, HIGH);   // sets relayOne on
    Serial.println("Relay 2 ON");
    delay(500);


    //Create system initialized message
    delay(1000);
    Serial.println(" System Initialized ");
    delay(1000);

    Serial.println(" Ready For Commands ");
    delay(2000);

    //Set up device status message
    Serial.println("Light  One: Off     ");
    delay(500);
    Serial.println("Light  Two: Off     ");

  }
}

void loop()
{
  unsigned long currentMillis = millis();
  if (wifiConnected) {
    upnpBroadcastResponder.serverLoop();
    lightOne->serverLoop();
    lightTwo->serverLoop();
    flicker->serverLoop();

     if(lightOne_state){
      if (ledState == HIGH){
           ledState = LOW;
           digitalWrite(relayOne,ledState);

          digitalWrite(relayOneOpp,LOW);
          for(int i=0;i<8;i++){
            delay(100);
            digitalWrite(relayOneOpp,HIGH);
            delay(1500);
            digitalWrite(relayOneOpp,LOW);
          }
        }
     }else{
        ledState = HIGH;
        digitalWrite(relayOne,ledState);
        digitalWrite(relayOneOpp,HIGH);
        delay(1000);
        digitalWrite(relayOneOpp,LOW);
     }
  }
}//end loop

/******************************************/

void lightOneOn() {
  lightOne_state = true;

}

void lightOneOff() {
  lightOne_state = false;
}

/******************************************/
void lightTwoOn() {
  // Serial.print("Switch 2 turn on ...");
  digitalWrite(relayTwo, LOW);   // sets relayTwo on
}

void lightTwoOff() {
  // Serial.print("Switch 2 turn off ...");
  digitalWrite(relayTwo, HIGH);   // sets relayTwo Off
}
/******************************************/
void flickerOn(){
  for(int i=0;i<2;i++){
    delay(100);
    digitalWrite(relayOneOpp,HIGH);
    delay(1500);
    digitalWrite(relayOneOpp,LOW);
  }
}

void flickerOff(){
    digitalWrite(relayOneOpp, HIGH);
    delay(1000);
    digitalWrite(relayOneOpp, LOW);
  }

/******************************************/

// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  // Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println(WiFi.localIP());
  }
  else {
     Serial.println("");
     Serial.println("Connection failed.");
     digitalWrite(LED_BUILTIN, LOW);
     delay(500);
     digitalWrite(LED_BUILTIN, HIGH);
     delay(500);
     digitalWrite(LED_BUILTIN, LOW);
     delay(500);
     digitalWrite(LED_BUILTIN, HIGH);
  }

  return state;
}
