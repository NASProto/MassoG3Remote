#include <Arduino.h>
#include <Keyboard.h>

// ——— Pin Definitions ———
const uint8_t PIN_JOY_X       = A0;
const uint8_t PIN_JOY_Y       = A1;
const uint8_t PIN_BTN_FEED    = 2;   // “Space” (feed‐hold)
const uint8_t PIN_BTN_FO_PLUS = 3;   // Numpad +
const uint8_t PIN_BTN_FO_MINUS= 4;   // Numpad –

const float JOY_THRESHOLD = 0.3;     // 30% deadzone
const int   JOY_MID       = 512;     // 0..1023 midpoint

// Track current state so we only call press() once:
bool xPlusHeld   = false;
bool xMinusHeld  = false;
bool yPlusHeld   = false;
bool yMinusHeld  = false;
bool shiftHeld   = false;

void setup() {
  pinMode(PIN_JOY_X, INPUT);
  pinMode(PIN_JOY_Y, INPUT);
  pinMode(PIN_BTN_FEED, INPUT_PULLUP);
  pinMode(PIN_BTN_FO_PLUS, INPUT_PULLUP);
  pinMode(PIN_BTN_FO_MINUS, INPUT_PULLUP);

  Keyboard.begin();
}

void loop() {
  // — Read joystick, normalize to –1…+1
  int rawX = analogRead(PIN_JOY_X);
  int rawY = analogRead(PIN_JOY_Y);
  float normX = (rawX - JOY_MID) / float(JOY_MID);
  float normY = (rawY - JOY_MID) / float(JOY_MID);

  // Determine if we need “Shift” held (continuous mode) at all:
  bool wantAnyArrow = (fabs(normX) > JOY_THRESHOLD) || (fabs(normY) > JOY_THRESHOLD);

  // —– Handle Shift key for continuous mode –—
  if (wantAnyArrow) {
    if (!shiftHeld) {
      Keyboard.press(KEY_LEFT_SHIFT);
      shiftHeld = true;
    }
  } else {
    if (shiftHeld) {
      Keyboard.release(KEY_LEFT_SHIFT);
      shiftHeld = false;
    }
  }

  // —– X axis: Shift+Right for X+, Shift+Left for X− –—
  if (normX > JOY_THRESHOLD) {
    // X+: hold Right Arrow
    if (!xPlusHeld) {
      Keyboard.press(KEY_RIGHT_ARROW);
      xPlusHeld = true;
    }
    // release X− if it was held
    if (xMinusHeld) {
      Keyboard.release(KEY_LEFT_ARROW);
      xMinusHeld = false;
    }
  }
  else if (normX < -JOY_THRESHOLD) {
    // X−: hold Left Arrow
    if (!xMinusHeld) {
      Keyboard.press(KEY_LEFT_ARROW);
      xMinusHeld = true;
    }
    if (xPlusHeld) {
      Keyboard.release(KEY_RIGHT_ARROW);
      xPlusHeld = false;
    }
  }
  else {
    // deadzone X: release both
    if (xPlusHeld) {
      Keyboard.release(KEY_RIGHT_ARROW);
      xPlusHeld = false;
    }
    if (xMinusHeld) {
      Keyboard.release(KEY_LEFT_ARROW);
      xMinusHeld = false;
    }
  }

  // —– Y axis: Shift+Up for Y+, Shift+Down for Y− –—
  if (normY < -JOY_THRESHOLD) {
    // note: pulling stick up yields negative normY → Y+
    if (!yPlusHeld) {
      Keyboard.press(KEY_UP_ARROW);
      yPlusHeld = true;
    }
    if (yMinusHeld) {
      Keyboard.release(KEY_DOWN_ARROW);
      yMinusHeld = false;
    }
  }
  else if (normY > JOY_THRESHOLD) {
    // stick down → Y−
    if (!yMinusHeld) {
      Keyboard.press(KEY_DOWN_ARROW);
      yMinusHeld = true;
    }
    if (yPlusHeld) {
      Keyboard.release(KEY_UP_ARROW);
      yPlusHeld = false;
    }
  }
  else {
    // deadzone Y: release both
    if (yPlusHeld) {
      Keyboard.release(KEY_UP_ARROW);
      yPlusHeld = false;
    }
    if (yMinusHeld) {
      Keyboard.release(KEY_DOWN_ARROW);
      yMinusHeld = false;
    }
  }

  // —– Buttons: Feed Hold (Space), FO +/– –—
  // Space: ASCII ' ' 
  if (digitalRead(PIN_BTN_FEED) == LOW) {
    Keyboard.press(' ');
  } else {
    Keyboard.release(' ');
  }

  if (digitalRead(PIN_BTN_FO_PLUS) == LOW) {
    Keyboard.press(KEY_KP_PLUS);
  } else {
    Keyboard.release(KEY_KP_PLUS);
  }

  if (digitalRead(PIN_BTN_FO_MINUS) == LOW) {
    Keyboard.press(KEY_KP_MINUS);
  } else {
    Keyboard.release(KEY_KP_MINUS);
  }

  delay(10);
}
