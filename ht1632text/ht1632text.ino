/** Interface HT1632C LED matrix with Arduino
* Requires HT1632 Library and font
* https://github.com/flavio-fernandes/HT1632-for-Arduino
* Takes serial input on \n sends to sign
**/
#include <FONT_7X5.h>
#include <HT1632.h>
#include <images.h>
int pinCS1 = 10;
int pinCS2 = 11;
int pinCS3 = 12;
int pinWR = 6;
int pinDATA = 7;
int i = 0;
int wd;
char buffer[500] = "hello, world! This sign finally works.";
int index=0;
int done=0;
boolean stringComplete = false;
void setup() {
  Serial.begin(38400);
  /* 3 segments */
  HT1632.begin(pinCS1,pinCS2,pinCS3,pinWR,pinDATA);
}

void loop () {
  if(stringComplete) {
  Serial.print("Buffer:");
  Serial.println(buffer);
  index=0; i=0;
  stringComplete = false;
  }

  printSomething(buffer);
  i = (i+1)%(wd + OUT_SIZE * 3);
  Serial.println(i);
  delay(50); 
}

void serialEvent() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    // if start of string clean out buffer
    if(index == 0) { memset(buffer, 0, sizeof(buffer)); }
    buffer[index] = c;
    index++;
    if (c == '\n') {
      stringComplete = true;
      index=0;
    }  
  } 
 }
 
 void printSomething(const char* input) {
  wd = HT1632.getTextWidth(input, FONT_7X5_WIDTH, FONT_7X5_HEIGHT);
  
  HT1632.drawTarget(BUFFER_BOARD(1));
  HT1632.clear();
  HT1632.drawText(input, 3*OUT_SIZE - i, 1, FONT_7X5, FONT_7X5_WIDTH, FONT_7X5_HEIGHT, FONT_7X5_STEP_GLYPH);
  HT1632.render();
  
  HT1632.drawTarget(BUFFER_BOARD(2));
  HT1632.clear();
  HT1632.drawText(input, 2*OUT_SIZE - i, 1, FONT_7X5, FONT_7X5_WIDTH, FONT_7X5_HEIGHT, FONT_7X5_STEP_GLYPH);
  HT1632.render();
  
  HT1632.drawTarget(BUFFER_BOARD(3));
  HT1632.clear();
  HT1632.drawText(input, OUT_SIZE - i, 1, FONT_7X5, FONT_7X5_WIDTH, FONT_7X5_HEIGHT, FONT_7X5_STEP_GLYPH);
  HT1632.render();
}
