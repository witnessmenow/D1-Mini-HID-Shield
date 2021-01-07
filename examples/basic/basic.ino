#include "CH_HID_Arduino.h"

void setup() {

  pinMode(D0, OUTPUT);
  digitalWrite(D0, LOW);
  ch_hid.begin();

  delay(100);
  ch_hid.send_sign();
  delay(10);
  Keyboard.print("Hallo");
  
  //Mouse.click(MOUSE_LEFT);  
  //Mouse.move(0, -40);
  
  //ch_hid.reset();
  //ch_hid.bootloader();
  
}


void loop() {
	//nothing to do here.
}
