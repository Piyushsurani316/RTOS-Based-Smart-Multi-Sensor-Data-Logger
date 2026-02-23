#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 not found!");
    while (1);
  }
}

void loop() {
  Serial.println(bmp.readPressure());
  delay(1000);
}