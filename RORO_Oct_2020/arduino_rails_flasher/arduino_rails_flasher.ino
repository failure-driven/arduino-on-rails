#include "U8glib.h"
#include "logos.h"

U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9); // SCK = 13, MOSI = 11, CS = 10, A0 = 9

const int pageCount = 4;
int page;

void pageRuby() {
  u8g.drawBitmapP( 23, 0, 11, 64, ruby_au_bitmap); // 81x64 so 23 X offset and 11 bytes wide
}
void pageRails() {
  u8g.drawBitmapP( 0, 8, 16, 48, rails_bitmap); // 128x48 so 8 Y offset full width of 16 bytes wide
}
void pageRoro() {
  u8g.drawBitmapP( 23, 0, 11, 64, roro_bitmap); // 82x64 so 23 X offset and 11 bytes wide
}
void pageArduino () {
  u8g.drawBitmapP( 17, 0, 12, 64, arduino_bitmap); // 94x64 so 17 X offset and 12 bytes wide
}

void (*pages[pageCount])() = {
  pageRuby,
  pageRails,
  pageRoro,
  pageArduino
};

void setup() {
  // flip screen, if required
  u8g.setRot180();
  pinMode(8, OUTPUT);
}

void loop() {
  // picture loop
  u8g.firstPage();
  do {
    (*pages[page])();
  } while (u8g.nextPage());

  page++;
  if (page == pageCount) {
    page = 0;
  }
  // rebuild the picture after some delay
  delay(400);
  digitalWrite(8, HIGH);
  delay(400);
  digitalWrite(8, LOW);
}
