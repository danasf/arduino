// Dependencies:
// https://github.com/adafruit/Adafruit_NeoPixel
// pitches.h from Arduino Examples->Digital Melody sketch
// Arduino Ethernet and SPI library
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>
#include "pitches.h"

// ==== Edit these vars ====

int ledPin=7; // pin for LEDs
int doorPin = 5; // pin for inbound door signal
int tonePin = 8; // pin for doorbell tone

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192,168,1,20);

// tell the server somebody rang the bell. we need to setup the web part
char serverName[] = "minotaur.noisebridge.net";  
char getStr[] = "GET /action/ringbell HTTP/1.0";

unsigned long buzzThreshold = 22000; // time before buzzing again, 15 seconds int
unsigned long ledThreshold = 2000; // start led flash interval

// doorbell chime
int melody[] = { NOTE_E5, NOTE_C5};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = { 8,8 };

// number of notes
int numNotes = 2;
// end user vars
// start init

unsigned long currentTime = 0;
unsigned long buzzTime = 0;
unsigned long ledTime = 0;
int ledState = LOW; // ledState used to set the LED
int buzzCount = 1; // number if times buzzed (per threshold)
int doorState = 1;
boolean timerState = false;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, ledPin, NEO_GRB +
NEO_KHZ800);
// initialize the library instance:
EthernetClient client;


void setup()
{
    Serial.begin(9600); // for debugging
    pinMode(doorPin, INPUT);
    digitalWrite(doorPin, HIGH);
    //makeSound();
    Serial.println("Attempting to get an IP address using DHCP:");
    if (!Ethernet.begin(mac)) {
      // if DHCP fails, start with a hard-coded address:
      Serial.println("failed to get an IP address using DHCP, trying manually");
      Ethernet.begin(mac, ip);
    }
    Serial.print("My address:");
    Serial.println(Ethernet.localIP());
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
}

void loop()
{

    doorState = digitalRead(doorPin);
    // show lights

    ledToggle();

    // if buzzer is buzzed
    if (doorState == LOW) {
        if(buzzCount == 1) { makeSound(); }
        // if time is over threshold
        if (checkThres(buzzTime) == true) {

            // set new time mark
            buzzTime = currentTime;
            buzzCount = 1;
            Serial.println("buzz threshold!");
            // tell the server someone buzzed
            connectToServer();

        } else {
            // count # of presses up to 10, modulate led flash speed
            if(buzzCount < 10) {
                buzzCount++;
            }
            Serial.print(".");

        }
    }
    // if door is not being buzzed do something else
    else {
          digitalWrite(doorPin, HIGH);
    }
}


// check the time threshold given a mark
boolean checkThres(unsigned long mark)
{
currentTime=millis();
    if (currentTime - mark > buzzThreshold) {
        return true;
    } else {
        return false;
    }
}

// toggle led given interval
void ledToggle()
{
// if not buzzed and over threshold, make rainbows
    if (doorState == HIGH && checkThres(buzzTime) == true) {
        rainbowCycle(15);

    }
// if buzzed
    else {
        // flip between these two colors
        uint32_t c1 = strip.Color(0, 0, 255);
        uint32_t c2 = strip.Color(0, 255, 0);
        // make more urgent if more buzzes
        currentTime=millis();
        if (currentTime - ledTime > (ledThreshold / buzzCount)) {
            // set ledtime to current time
            ledTime = currentTime;
            // change state
            if (ledState == LOW) {
                ledState = HIGH;
                colorSet(c1, c2, 0); // Flip

            } else {
                ledState = LOW;
                colorSet(c2, c1, 0); // Flop
            }
        }
    }

}

// play a tone
void makeSound()
{
for (int thisNote = 0; thisNote < numNotes; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(tonePin, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(tonePin);
  }
}

//
// code for lights

// flip lights
void colorSet(uint32_t c, uint32_t c2, uint8_t wait)
{
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        if (i > strip.numPixels() / 2) {
            strip.setPixelColor(i, c);
        } else {
            strip.setPixelColor(i, c2);
        }
    }
    strip.show();
    delay(wait);
}

// color wipe
void colorWipe(uint32_t c, uint8_t wait)
{
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        doorState = digitalRead(doorPin);
        if (doorState == LOW) {
            break;
        }

        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

// cycle rainbow colors
void rainbowCycle(uint8_t wait)
{
    uint16_t i, j;

    for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
        doorState = digitalRead(doorPin);
        // break if door is triggered
        if (doorState == LOW) {
            break;
        }

        for (i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) +
j) &
255));
        }
        strip.show();
        delay(wait);
    }
}

uint32_t Wheel(byte WheelPos)
{
    if (WheelPos < 85) {
        return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

// ethernet connect
void connectToServer() {
  // attempt to connect, and wait a millisecond:
  Serial.println("connecting to server...");
  if (client.connect(serverName, 80)) {
    Serial.println("making HTTP request...");
    // make HTTP GET request to server:
    client.println(getStr);
    client.println();
  }
}   