/*******************************************************************
    A webpage that listens for button presses and
    passes them onto the HID Shield

    Parts:
    ESP8266 D1 Mini Dev board -
    D1 Mini HID Shield -

 *  * = Affilate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/


    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <WiFiClientSecure.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <CH_HID_Arduino.h>
// Library for connecing to the HID
// Shield

// Install from Github
// https://github.com/atc1441/CH_HID_Arduino

//------- Replace the following! ------

char ssid[] = "SSID";         // your network SSID (name)
char password[] = "PASS"; // your network password

#define MDNS_NAME "fire"

//------- ---------------------- ------

ESP8266WebServer server(80);
WiFiClientSecure client;

const char *webpage =
#include "page.h"
  ;

void handleRoot() {

  server.send(200, "text/html", webpage);
}

void handleCommand() {
  String command = "";
  int commandNum = 0;
  uint8_t key = 0;
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "press") {
      command = server.arg(i);
      commandNum = command.toInt();
      switch (commandNum) {
        case 1:
          key = KEY_UP_ARROW;
          break;
        case 2:
          key = KEY_LEFT_ARROW;
          break;
        case 3:
          key = KEY_RIGHT_ARROW;
          break;
        case 4:
          key = KEY_DOWN_ARROW;
          break;
        case 5:
          key = KEY_ESC;
          break;
        case 6:
          key = KEY_RETURN;
          break;
        case 7:
          key = KEY_F12;
          break;
      }
    }
  }
  ch_hid.send_sign();
  if (key > 0) {
    Keyboard.press(key);
    delay(50);
    Keyboard.release(key);
    delay(50);
    server.send(200, "text/plain", "got it");
  } else {
    server.send(404, "text/plain", "not a real key");
  }
}

void handleText() {
  String text = "";
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "text") {
      text = server.arg(i);
    }

  }
  if(text != ""){
    ch_hid.send_sign();
    Keyboard.print(text);
  }

}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  //Serial.print(message);
  server.send(404, "text/plain", message);
}

void setup() {

  pinMode(D4, OUTPUT); // Built-in LED
  digitalWrite(D4, LOW);

  // NOTE: Need to set D0 Low for HID Shield!
  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);

  ch_hid.begin();
  delay(100);

  digitalWrite(D4, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(ssid, password);

  bool ledActive = HIGH;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ledActive = !ledActive;
    digitalWrite(D4, ledActive);
  }

  if (MDNS.begin(MDNS_NAME)) {
    //Serial.println("MDNS Responder Started");
  }

  server.on("/", handleRoot);
  server.on("/command", handleCommand);
  server.on("/text", handleText);
  server.onNotFound(handleNotFound);

  server.begin();
  digitalWrite(D4, LOW);
}

void loop() {
  server.handleClient();
  MDNS.update();
}
