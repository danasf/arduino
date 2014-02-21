/*
  Xively sensor client with Strings
 
 This sketch connects an analog sensor to Xively (http://www.xively.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 This example has been updated to use version 2.0 of the xively.com API. 
 To make it work, create a feed with two datastreams, and give them the IDs
 sensor1 and sensor2. Or change the code below to match your feed.
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 DHT Sensor Library
 https://github.com/adafruit/DHT-sensor-library
 
 Circuit:
 * Analog sensor attached to analog in 0
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 modified 9 Apr 2012
 by Tom Igoe with input from Usman Haque and Joe Saavedra
 modified 8 September 2012
 by Scott Fitzgerald
 modified 2 Jan 2014 
 by Dana Sniezko
 
 http://arduino.cc/en/Tutorial/XivelyClientString
 This code is in the public domain.
 
 */
 
 nt measurePin = 0;
int ledPower = 9;
 
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
 
float voMeasured = 0;
float calcVoltage = 0;
 char test[20];

#include "DHT.h"

#include <SPI.h>
#include <Ethernet.h>
#include<stdlib.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define APIKEY         "v3ZXDz6udkXXIrvtz8UQS3YYe9ERto2OESFDESijTgL5X9hn" // replace your Xively api key here
#define FEEDID         1410530669 // replace your feed ID
#define USERAGENT      "Environmental Sensor 001" // user agent is the project name

DHT dht(DHTPIN, DHTTYPE);

// assign a MAC address for the ethernet controller.
// fill in your address here:
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(10,0,1,111);

// initialize the library instance:
EthernetClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
char server[] = "api.xively.com";   // name address for xively API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to xively.com

void setup() {
    pinMode(ledPower,OUTPUT);

 // Open serial communications and wait for port to open:
  //Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip);
  }
}

void loop() {
   String dataString = "";

   // read floats from your analog sensors:
   float hum = dht.readHumidity();
   float t = dht.readTemperature();
   float dust = measureDust();

  // convert the data to a String to send it:
  dataString +="humidity,";
  dataString += dtostrf(hum,3,2,test);

  // you can append multiple readings to this String if your
  // xively feed is set up to handle multiple values:
  // temperature
  dataString += "\r\ntemp,";
  dataString += dtostrf(t,3,2,test);
  // temperature in F
  dataString += "\r\ntempF,";
  dataString += dtostrf((t*1.8)+32,3,2,test);
  // particle count
  dataString += "\r\nparticle,";
  dataString += dtostrf(dust,3,2,test);

  //Serial.println(dataString);

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    //Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
   // Serial.println();
    //Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data: 
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    sendData(dataString);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData(String thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
   // Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.println(thisData);
  } 
  else {
    // if you couldn't make a connection:
    //Serial.println("connection failed");
    //Serial.println();
    //Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}

float measureDust () {
  float dustDensity = 0;
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
 
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024);
 
  // Airquality linear eqaution from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;
  return dustDensity;
}
