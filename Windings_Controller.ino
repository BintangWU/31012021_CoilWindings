/*
   @Windings Controller
   @Version 1.0
   @Author Bintang W. U.
   @contanct bintang.wahyu.channel@gmail.com

   @Description
   > Mengatur kecepatan putaran motor dan counter jumlah lilitan pada jangkar/kern
   > Singel MOSFET Driver IRF540
   > Keypad 4x3 Keypad.h
   > LCD Alphanumeric 16x2 (non-I2c) LiquidCrystal.h
   #
*/

#include <LiquidCrystal.h>
#include <Keypad.h>

#define ROWS       4
#define COLS       3
uint8_t rowPins[ROWS] = {12, 11, A0, A1};
uint8_t colPins[COLS] = {A2, A3, A4};

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

LiquidCrystal       lcd(9, 8, 7, 6, 5, 4);
Keypad              keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

#define potensio    A5
#define sensor      2
#define pwm         3

uint8_t cnt, menu = 0, speed;
uint16_t nLilit, pulse;
bool run = false, status = false, pause = false;
char key;
String temp = "";

float adcFilter (uint16_t adcIn, uint8_t n);
void testRoll(bool run = false);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);

  keypad.addEventListener(keypadEvent);
  attachInterrupt(digitalPinToInterrupt(sensor), count, FALLING);

  pinMode(pwm, OUTPUT);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Windings Control");
  lcd.setCursor(0, 1);
  lcd.print("Version 1.0     ");
  delay(1000); lcd.clear();

}

void loop() {
  key = keypad.getKey();
  speed = map((int)adcFilter(potensio, 20), 70, 1022, 0, 100);
  if (speed <= 10)
    speed = 0;

  if (digitalRead(13) == HIGH)
    testRoll(true);
  else
    testRoll(false);

  if (run) {
    attachInterrupt(digitalPinToInterrupt(sensor), count, FALLING);
    analogWrite(pwm, map(speed, 0, 100, 0, 255));

    lcd.setCursor(0, 0);
    lcd.print("nLilit : " + (String)pulse + " ");
    lcd.setCursor(0, 1);
    lcd.print("Speed  : " + (String)speed + "% ");

    if (pulse >= nLilit) {
      lcd.clear();
      run = false;
    }
  }
  else {
    detachInterrupt(digitalPinToInterrupt(sensor));

    if (key >= '0' and key <= '9') {
      temp += key;
      nLilit = temp.toInt();
    }
    lcd.setCursor(0, 0);
    lcd.print("nLilit : " + temp + " ");
    //analogWrite(pwm, map(speed, 0, 100, 0, 255));
    lcd.setCursor(0, 1);
    lcd.print("Speed  : " + (String)speed + "% ");

    testRoll(status);
  }


}

void count() {
  pulse++;
}

float adcFilter (uint16_t adcIn, uint8_t n) {
  float adcValue, adcBuff[n];
  int adcCnt;

  for (adcCnt = 0; adcCnt <= n; adcCnt++)
    adcBuff[adcCnt] = analogRead(adcIn);

  adcValue = 0;
  for (adcCnt = 0; adcCnt < 20; adcCnt++)
    adcValue += adcBuff[adcCnt];

  adcValue = (adcValue / n);
  return adcValue;
}

void testRoll(bool run) {
  if (run)
    analogWrite(pwm, map(speed, 0, 100, 0, 255));
  else
    analogWrite(pwm, 0);
}

void playRoll() {

}

void clear() {
  temp = "";
  nLilit = 0;
  lcd.clear();
}

void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED  :
      if (key == '*') {
        clear();
      }
      else if (key == '#') {
        pulse = 0;
        run = true;
        lcd.clear();
      }
      break;

    case HOLD     :
      if (key == '#')
        status = true;
      break;

    case RELEASED :
      if (key == '#')
        status = false;
      break;
  }
}





