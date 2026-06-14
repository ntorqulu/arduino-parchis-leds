#include <LedControl.h>

#define BTN_PIN   2
#define RED_PIN   6
#define GRN_PIN   7

LedControl lc = LedControl(11, 13, 10, 1);

const unsigned long HOLD_THRESHOLD = 2000;
const unsigned long RUN_DURATION = 10000;
const unsigned long FLICKER_PERIOD = 200;

enum State { IDLE, READY, RUNNING };
State state = IDLE;

bool lastBtnRaw  = HIGH;
bool btnPressed  = false;
bool btnHeld     = false;
unsigned long btnDownAt = 0;

unsigned long runStartAt  = 0;
unsigned long flickerAt   = 0;
bool flickerState = false;

const byte GLYPH_START[8] = {
  0b00111100,
  0b01000010,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
  0b01000010,
  0b00111100
};

const byte GLYPH_2[8] = {
  0b01100110,  // row 0 — top-left + top-right dots
  0b01100110,  // row 1
  0b00000000,  // row 2
  0b00000000,  // row 3
  0b00000000,  // row 4
  0b00000000,  // row 5
  0b01100110,  // row 6 — bottom-left + bottom-right dots
  0b01100110   // row 7
};

void showGlyph(const byte* g) {
  for (int r = 0; r < 8; r++)
    lc.setRow(0, r, g[r]);
}

void enterIdle() {
  state = IDLE;
  lc.clearDisplay(0);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);
}

void enterReady() {
  state = READY;
  showGlyph(GLYPH_START);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GRN_PIN, LOW);
}

void enterRunning() {
  state = RUNNING;
  runStartAt = millis();
  flickerAt = millis();
  flickerState = false;
  showGlyph(GLYPH_2);
  digitalWrite(GRN_PIN, HIGH);
}

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  enterIdle();
}

void loop() {
  unsigned long now = millis();

  bool raw = digitalRead(BTN_PIN);
  btnPressed = false;
  btnHeld = false;

  if (lastBtnRaw == HIGH && raw == LOW) {
    btnDownAt = now;
  }

  if (lastBtnRaw == LOW && raw == HIGH) {
    unsigned long held = now - btnDownAt;
    if (held >= HOLD_THRESHOLD) btnHeld = true;
    else                         btnPressed = true;
  }

  lastBtnRaw = raw;

  switch (state) {

    case IDLE:
      if (btnHeld) enterReady();
      break;

    case READY:
      if (btnPressed) enterRunning();
      if (btnHeld) enterIdle();
      break;

    case RUNNING:
      if (btnHeld) { enterIdle(); break; }
      if (now - flickerAt >= FLICKER_PERIOD) {
        flickerState = !flickerState;
        digitalWrite(RED_PIN, flickerState ? HIGH : LOW);
        flickerAt = now;
      }
      if (now - runStartAt >= RUN_DURATION) enterIdle();
      break;
  }
}
