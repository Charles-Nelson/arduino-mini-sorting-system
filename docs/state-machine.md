# State Machine

This document describes the control logic for the Arduino Mini Sorting System.

The sorter uses a state-machine approach so that each physical part is handled once per cycle. This helps prevent false counts, double detection, and actuator timing problems.

---

## State Machine Diagram

```mermaid
stateDiagram-v2
    [*] --> INIT

    INIT --> IDLE : System ready

    IDLE --> READ_SENSOR : Part detected

    READ_SENSOR --> CLASSIFY_PART : Sensor value captured

    CLASSIFY_PART --> SORT_BLACK : Value in black threshold range
    CLASSIFY_PART --> SORT_WHITE : Value in white threshold range
    CLASSIFY_PART --> NO_PART : Value in empty/no-part range
    CLASSIFY_PART --> ERROR : Value outside expected ranges

    SORT_BLACK --> MOVE_DIVERTER_BLACK : Command servo to black position
    SORT_WHITE --> MOVE_DIVERTER_WHITE : Command servo to white position

    MOVE_DIVERTER_BLACK --> COUNT_BLACK : Part routed
    MOVE_DIVERTER_WHITE --> COUNT_WHITE : Part routed

    COUNT_BLACK --> WAIT_CLEAR : Black count updated
    COUNT_WHITE --> WAIT_CLEAR : White count updated
    NO_PART --> IDLE : No valid part detected

    WAIT_CLEAR --> IDLE : Sensor returns to empty state

    ERROR --> IDLE : Error logged / ignored
```

---

## State Descriptions

| State | Purpose |
|---|---|
| `INIT` | Initializes LCD, servo position, counters, and sensor settings. |
| `IDLE` | Waits for a part to enter the sensor area. |
| `READ_SENSOR` | Reads the reflectivity sensor value. |
| `CLASSIFY_PART` | Compares sensor value against threshold ranges. |
| `SORT_BLACK` | Begins logic for routing a black part. |
| `SORT_WHITE` | Begins logic for routing a white part. |
| `MOVE_DIVERTER_BLACK` | Moves the servo/diverter to the black-part path. |
| `MOVE_DIVERTER_WHITE` | Moves the servo/diverter to the white-part path. |
| `COUNT_BLACK` | Increments the black-part counter. |
| `COUNT_WHITE` | Increments the white-part counter. |
| `WAIT_CLEAR` | Waits until the sensor area is clear before accepting another part. |
| `NO_PART` | Handles readings that indicate no part is present. |
| `ERROR` | Handles readings outside expected threshold ranges. |

---

## Why This State Machine Matters

The sorter should not simply check the sensor value repeatedly inside the Arduino loop and count every matching reading.

Because the Arduino loop runs many times per second, one physical part could be detected multiple times.

The `WAIT_CLEAR` state solves this by requiring the system to see an empty sensor condition before returning to `IDLE`.

---

## Basic Logic Flow

1. System initializes.
2. System waits for a part.
3. Sensor reading is captured.
4. Reading is classified as black, white, empty, or error.
5. Servo moves diverter to the correct position.
6. Count is updated.
7. System waits for the part to clear the sensor area.
8. System returns to idle.

---

## Threshold Logic

Exact threshold values still need to be measured and confirmed during calibration.

Planned categories:

| Condition | Sensor Reading Range | Action |
|---|---:|---|
| Empty / no part | TBD | Return to idle |
| Black part | TBD | Route to black bin |
| White part | TBD | Route to white bin |
| Unknown / unstable | TBD | Ignore or flag error |

---

## Future Improvements

- Add debounce/filtering for noisy sensor readings.
- Record actual measured sensor values.
- Add timeout behavior if the sensor never clears.
- Add LCD messages for each state.
- Add serial monitor debugging output.
- Add error handling for unexpected readings.