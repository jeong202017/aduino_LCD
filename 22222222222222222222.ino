#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// LCD 설정: 주소 0x27 또는 0x3F 중 하나
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 핀 번호 설정
const int touchLeftPin = 3;
const int touchRightPin = 12;
const int buzzerPin = 6;
const int resetButtonPin = 9;
const int ledPin = 10; // 선택 사항

// 혈당값 및 상태 변수
int glucose = 210;
bool measuring = false;

void setup() {
  pinMode(touchLeftPin, INPUT);
  pinMode(touchRightPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP); // 풀업 사용
  pinMode(ledPin, OUTPUT); // LED (선택 사항)

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("START!");

  // 시드값으로 랜덤성 강화
  randomSeed(analogRead(0));
}

void loop() {
  // 초기화 버튼 확인
  if (digitalRead(resetButtonPin) == LOW) {
    glucose = 210;
    measuring = false;

    // LED 깜빡이기 (선택 사항)
    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH);
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
    }

    // LCD 초기화 메시지
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET!");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MEASUREMENT START!");
    delay(500);
    return;
  }

  int leftTouch = digitalRead(touchLeftPin);
  int rightTouch = digitalRead(touchRightPin);

  // 측정 시작 조건
  if (leftTouch == HIGH && rightTouch == HIGH && !measuring) {
    tone(buzzerPin, 1000);    // 부저 울림
    delay(1000);
    noTone(buzzerPin);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Measuring....");
    delay(5000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Blood Sugar Now ");
    lcd.print(glucose);
    lcd.print(" mg/dL");

    measuring = true;
    delay(500);
  }

  // 측정 중: 터치가 감지되면 혈당값 감소
  if (measuring && (leftTouch == HIGH || rightTouch == HIGH)) {
    int decrease = random(8, 16);  // 8~15 사이
    glucose -= decrease;
    if (glucose < 100) glucose = 100;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Blood Sugar Now ");
    lcd.print(glucose);
    lcd.print(" mg/dL");

    delay(1000); // 연속 입력 방지
  }
}
