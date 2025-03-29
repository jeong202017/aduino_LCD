#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 핀 번호 설정
const int touchLeftPin = 3;
const int touchRightPin = 12;
const int buzzerPin = 6;
const int resetButtonPin = 9;
const int ledPin = 10;

int glucose = 210;
bool measuring = false;
unsigned long lastMeasureTime = 0;
const unsigned long cooldownDuration = 3000; // 3초 쿨다운

// 물방울 모양 사용자 정의 문자 (5x8 픽셀)
byte droplet[8] = {
  0b00100,
  0b00100,
  0b01010,
  0b01010,
  0b10001,
  0b10001,
  0b10001,
  0b01110
};

void setup() {
  pinMode(touchLeftPin, INPUT);
  pinMode(touchRightPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  // 물방울 문자 등록
  lcd.createChar(0, droplet);

  lcd.setCursor(0, 0);
  lcd.print("START!");
  lcd.setCursor(15, 0);
  lcd.write(byte(0)); // 물방울 아이콘

  randomSeed(analogRead(0));
}

void loop() {
  if (digitalRead(resetButtonPin) == LOW) {
    glucose = 210;
    measuring = false;

    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET!");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MEASUREMENT START!");
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    delay(500);
    return;
  }

  int leftTouch = digitalRead(touchLeftPin);
  int rightTouch = digitalRead(touchRightPin);

  if (leftTouch == HIGH && rightTouch == HIGH && !measuring) {
    tone(buzzerPin, 1000);
    delay(2000);
    noTone(buzzerPin);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Measuring...");
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    delay(5000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Glucose Now:");
    lcd.setCursor(0, 1);
    lcd.print(glucose);
    lcd.print(" mg/dL ");
    lcd.write(byte(0));

    measuring = true;
    lastMeasureTime = millis();
    delay(500);
  }

  if (measuring && (leftTouch == HIGH || rightTouch == HIGH)) {
    if (millis() - lastMeasureTime > cooldownDuration) {
      int decrease = random(8, 16);
      glucose -= decrease;
      if (glucose < 100) glucose = 100;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Glucose Now:");
      lcd.setCursor(0, 1);
      lcd.print(glucose);
      lcd.print(" mg/dL ");
      lcd.write(byte(0));

      lastMeasureTime = millis();
      delay(1000);
    }
  }
}
