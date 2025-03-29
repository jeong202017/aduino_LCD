#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD 설정 (주소 0x27 또는 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 핀 설정
const int touchLeftPin = 3;
const int touchRightPin = 12;
const int buzzerPin = 6;
const int resetButtonPin = 9;
const int ledPin = 10;

int glucose = 210;
bool measuring = false;
unsigned long lastMeasureTime = 0;
const unsigned long cooldownDuration = 3000; // 3초 쿨다운

// 물방울 아이콘 정의 (5x8 픽셀)
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
  lcd.createChar(0, droplet);  // 물방울 문자 등록

  lcd.setCursor(0, 0);
  lcd.print("START");
  lcd.setCursor(15, 0);
  lcd.write(byte(0));  // 물방울 아이콘

  randomSeed(analogRead(0));
}

void loop() {
  // 초기화 버튼 눌림
  if (digitalRead(resetButtonPin) == LOW) {
    glucose = 210;
    measuring = false;

    // LED 깜빡임
    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET DONE");
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("START");
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    delay(500);
    return;
  }

  int leftTouch = digitalRead(touchLeftPin);
  int rightTouch = digitalRead(touchRightPin);

  // 측정 시작 조건
  if (leftTouch == HIGH && rightTouch == HIGH && !measuring) {
    tone(buzzerPin, 1000);
    delay(2000);
    noTone(buzzerPin);

    // 측정 시작 메시지 (물방울 출력 없음)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MEASURE START");
    delay(5000);

    // 초기 혈당 표시 + 물방울 아이콘
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Glucose:");
    lcd.setCursor(0, 1);
    lcd.print(glucose);
    lcd.print(" mg/dL");
    lcd.setCursor(15, 1);
    lcd.write(byte(0));

    measuring = true;
    lastMeasureTime = millis();
    delay(500);
  }

  // 측정 중일 때 터치하면 수치 감소
  if (measuring && (leftTouch == HIGH || rightTouch == HIGH)) {
    if (millis() - lastMeasureTime > cooldownDuration) {
      int decrease = random(8, 16);
      glucose -= decrease;
      if (glucose < 100) glucose = 100;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Glucose:");
      lcd.setCursor(0, 1);
      lcd.print(glucose);
      lcd.print(" mg/dL");
      lcd.setCursor(15, 1);
      lcd.write(byte(0));

      lastMeasureTime = millis();
      delay(1000);
    }
  }
}
