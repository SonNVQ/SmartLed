#include <LiquidCrystal.h>

const int LIGHT = 11, BUTTON = 10, POTEN = A5, PHOTO = A2;
const long BUTTON_HOLD_TIME = 1000;

long buttonTimer = 0;
int photoValue = 0, outputPhotoValue = 0;
int potenValue = 0, outputPotenValue = 0;

boolean isManual = 1;
boolean isChangedMode = 1;
boolean status = 1;

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal LCD(rs, en, d4, d5, d6, d7);

void setup() {
  pinMode(LIGHT, OUTPUT);
  pinMode(BUTTON, INPUT);
  LCD.begin(16, 2);
  Serial.begin(9000);
}

void loop() {
  buttonProcess();
  if (status) {
    if (isChangedMode) {
      printMode();
      isChangedMode = 0;
      outputPhotoValue = -1;
      outputPotenValue = -1;
    }
    if (isManual) {
      potenProcess();
    } else {
      photoProcess();
    }
  }
}

void printMode() {
  LCD.setCursor(0, 0);
  if (isManual) {
    LCD.print("MODE: Manual");
    LCD.setCursor(0, 1);
    LCD.print("Brightness: ");
  } else
    LCD.print("MODE: Auto");
}

const float FIX_PHOTO_SENSITIVITY = 1.3f;
void photoProcess() {
  photoValue = analogRead(PHOTO);
  int mappedPhotoValue = map(photoValue, 0, 1023, 0, 255);
  int tempValue = 255 - mappedPhotoValue * FIX_PHOTO_SENSITIVITY;
  outputPhotoValue = tempValue <= 0 ? 0 : tempValue;
  analogWrite(LIGHT, outputPhotoValue);
}

void potenProcess() {
  int oldOutputPotenValue = outputPotenValue;  //get old value
  potenValue = analogRead(POTEN);
  outputPotenValue = map(potenValue, 0, 1023, 0, 255);
  if (outputPotenValue <= 3)
    outputPotenValue = 0;
  else if (outputPotenValue >= 253)
    outputPotenValue = 255;
  if (outputPotenValue != oldOutputPotenValue) {
    int displayPotenValue = map(outputPotenValue, 0, 255, 0, 10);
    Serial.println(outputPotenValue);
    analogWrite(LIGHT, outputPotenValue);
    LCD.setCursor(12, 1);
    LCD.print(displayPotenValue);
    LCD.print(" ");  //fix old value ghosting
  }
}

//=============================================BUTTON============================================
boolean buttonActive = false;
boolean longPressActive = false;

void buttonProcess() {
  if (digitalRead(BUTTON) == HIGH) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > BUTTON_HOLD_TIME) && (longPressActive == false)) {
      longPressActive = true;
      buttonHoldEvent();
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        buttonClickEvent();
      }
      buttonActive = false;
    }
  }
}

void buttonClickEvent() {
  Serial.println("click: ");
  isChangedMode = 1;
  int oldisManual = isManual;
  isManual = !isManual;
  if (oldisManual != isManual) {
    LCD.clear();
  }
  Serial.println(isManual);
}

void buttonHoldEvent() {
  Serial.print("hold: ");
  isChangedMode = 1;
  status = !status;
  Serial.println(status);
  if (!status) {
    digitalWrite(LIGHT, LOW);
    LCD.noDisplay();
  } else {
    LCD.display();
  }
}
//=============================================BUTTON============================================