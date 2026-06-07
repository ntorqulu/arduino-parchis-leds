# How it works

## State machine

The project runs a 3-state machine. The button controls all transitions.

```
IDLE ──(hold 2s)──► READY ──(short press)──► RUNNING
 ▲                    │                         │
 └────(hold 2s)───────┘                         │
 └────(hold 2s / timer expires)─────────────────┘
```

### IDLE

Everything is off. The Arduino is waiting for the first interaction. Nothing happens until the button is held.

### READY

Triggered by holding the button for 2 seconds and releasing. The matrix displays a ring pattern as a visual signal that the system is armed and waiting. Both LEDs remain off.

### RUNNING

Triggered by a short press from READY. Three things happen simultaneously:

- The matrix displays the number **2**
- The **green LED** turns on solid
- The **red LED** flickers on and off every 200ms

The system stays in RUNNING for 10 seconds, then resets automatically to IDLE. It can also be reset at any time by holding the button for 2 seconds.

---

## Button logic

The button uses the Arduino internal pull-up resistor (`INPUT_PULLUP`). This means:

- Unpressed → pin reads **HIGH**
- Pressed → pin reads **LOW**

Two types of press are detected:

| Press type | Duration | Action |
|---|---|---|
| Short press | less than 2 seconds | READY → RUNNING |
| Long press | 2 seconds or more | reset to IDLE |

The detection happens on the **rising edge** (when the button is released), so the duration is measured from press to release.

---

## LED behaviour

| LED | Pin | IDLE | READY | RUNNING |
|---|---|---|---|---|
| Red | D6 | Off | Off | Flickers every 200ms |
| Green | D7 | Off | Off | Solid on |

The red LED flicker is handled without `delay()` — a timer checks every loop iteration whether 200ms have passed, then toggles the LED. This keeps the loop responsive to button input at all times.

---

## Matrix glyphs

Two glyphs are stored as byte arrays, one row per byte:

**Ring (READY state)**
```
  00111100
  01000010
  10000001
  10000001
  10000001
  10000001
  01000010
  00111100
```

**Number 2 (RUNNING state)**
```
  00111100
  01000010
  00000010
  00001100
  00110000
  01000000
  01111110
  00000000
```

---

## Timing constants

All timing values are defined at the top of the sketch and can be adjusted:

| Constant | Default | Description |
|---|---|---|
| `HOLD_THRESHOLD` | 2000 ms | Minimum hold time for a long press |
| `RUN_DURATION` | 10000 ms | How long RUNNING lasts before auto-reset |
| `FLICKER_PERIOD` | 200 ms | Red LED toggle interval |