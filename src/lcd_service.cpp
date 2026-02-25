#include "lcd_service.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

void initLCD() {
    lcd.begin();
    lcd.backlight();
}

void updateLCDPage(uint8_t page,
                   int temperature, int pressure,
                   int heartRate, int spo2,
                   int16_t ax, int16_t ay, int16_t az,
                   int16_t gx, int16_t gy, int16_t gz) {

    lcd.clear();

    if (page == 0) {
        lcd.setCursor(0,0);
        lcd.print("T:"); lcd.print(temperature);
        lcd.print(" P:"); lcd.print(pressure);

        lcd.setCursor(0,1);
        lcd.print("HR:"); lcd.print(heartRate);
        lcd.print(" S:"); lcd.print(spo2);
    }
    else if (page == 1) {
        lcd.setCursor(0,0);
        lcd.print("AX:"); lcd.print(ax);
        lcd.setCursor(0,1);
        lcd.print("AY:"); lcd.print(ay);
    }
    else if (page == 2) {
        lcd.setCursor(0,0);
        lcd.print("GX:"); lcd.print(gx);
        lcd.setCursor(0,1);
        lcd.print("GY:"); lcd.print(gy);
    }
}