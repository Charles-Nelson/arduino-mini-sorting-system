# Arduino Mini Sorting System

Arduino-based automated sorting system using reflectivity sensing, servo actuation, LCD feedback, and state-machine control.

This project demonstrates a small-scale industrial automation concept: detecting parts, classifying them based on sensor input, routing them with an actuator, and tracking system status through counters and display feedback.

---

## Project Overview

The Arduino Mini Sorting System is a prototype sorting machine designed to identify different part conditions using a reflectivity sensor and route parts using a servo-controlled diverter.

The system is built around practical automation concepts:

- Sensor-based part detection
- State-machine control logic
- Servo actuation
- Part counting
- LCD system feedback
- Troubleshooting and calibration documentation
- Git/GitHub version control workflow

The goal of this project is not just to make an Arduino circuit work, but to document the system like an engineering project.

---

## System Behavior

The sorter operates through a repeatable control sequence:

1. The system waits for a part to enter the sensor area.
2. The reflectivity sensor reads the part surface.
3. The Arduino classifies the reading based on calibrated threshold values.
4. The servo moves the diverter to the appropriate position.
5. The system updates the appropriate count.
6. The system waits for the part to clear the sensor area.
7. The system returns to idle and waits for the next part.

The `WAIT_CLEAR` behavior is important because the Arduino loop runs many times per second. Without state-machine control, one physical part could be counted multiple times.

---

## Hardware Used

| Component | Purpose |
|---|---|
| Arduino board | Main controller |
| Reflectivity sensor / photoresistor sensor | Detects part presence and surface reflectivity |
| Servo motor | Moves diverter mechanism |
| I2C LCD display | Displays system status and counts |
| External 5V power supply | Provides stable servo power |
| Shared ground connection | Provides common reference between Arduino and external servo supply |
| Jumper wires / breadboard | Prototype wiring |
| 3D printed or fabricated chute/ramp | Guides parts through the sensor area |
| 3D printed or fabricated diverter | Routes parts based on classification |

---

## Pin Assignment

> Pin assignments should be updated as the wiring is finalized.

| Device | Signal | Arduino Pin | Notes |
|---|---:|---:|---|
| Reflectivity sensor | Analog output | A0 | Reads part reflectivity |
| Servo motor | Control signal | D9 | Controls diverter position |
| LCD | SDA | SDA | I2C data |
| LCD | SCL | SCL | I2C clock |
| Run button | Digital input | TBD | Starts/enables system |
| Stop button | Digital input | TBD | Stops/disables system |
| Status LED | Digital output | TBD | Optional running/idle indication |
| Servo power | VCC | External 5V | Do not power servo from Arduino if unstable |
| Common ground | GND | Arduino GND | Must be tied to external supply ground |

---

## Control Logic

The sorter uses a state-machine approach instead of simple continuous sensor polling.

This improves reliability by separating the control process into defined operating states such as:

- Idle / waiting for part
- Sensor reading
- Part classification
- Servo diversion
- Count update
- Wait for part to clear
- Return to idle

More detail is available in:

```text
docs/state-machine.md