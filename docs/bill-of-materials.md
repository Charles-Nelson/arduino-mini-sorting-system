# Bill of Materials

This document lists the major components used in the Arduino Mini Sorting System.

---

## Core Control Hardware

| Item | Purpose | Notes |
|---|---|---|
| Arduino board | Main controller | Reads sensor input, controls servo, updates LCD |
| USB cable | Programming and power during development | Used to upload code from computer to Arduino |
| Breadboard / jumper wires | Prototyping connections | Temporary wiring during development |

---

## Sensors

| Item | Purpose | Notes |
|---|---|---|
| Reflectivity sensor / photoresistor sensor | Detects part condition/color | Used to distinguish empty state, black part, and white part |
| Sensor tunnel or enclosure | Controls lighting around sensor | Helps reduce ambient light variation and improve repeatability |

---

## Actuation

| Item | Purpose | Notes |
|---|---|---|
| Servo motor | Moves diverter mechanism | Routes parts based on sensor result |
| Servo horn / linkage | Connects servo to diverter | Exact geometry depends on final mechanical design |
| Diverter gate | Physically redirects parts | Can be 3D printed or fabricated |

---

## Display / Feedback

| Item | Purpose | Notes |
|---|---|---|
| I2C LCD display | Provides system feedback | Displays status, counts, or detected part type |
| I2C backpack/module | LCD communication interface | Reduces wiring by using SDA/SCL communication |

---

## Power

| Item | Purpose | Notes |
|---|---|---|
| External 5V power supply | Powers servo motor | Helps avoid Arduino brownout/reset issues |
| Common ground connection | Shared reference between Arduino and servo supply | Required for reliable servo signal behavior |
| Arduino 5V / USB power | Powers controller and low-current logic | Do not rely on Arduino 5V pin for high servo current if unstable |

---

## Mechanical / 3D Printed Parts

| Item | Purpose | Notes |
|---|---|---|
| Part ramp / chute | Guides parts through the sensor area | Should keep parts aligned and repeatable |
| Sensor mount | Holds sensor in fixed position | Important for stable calibration |
| Servo mount | Holds servo securely | Prevents movement during actuation |
| Diverter mechanism | Redirects sorted parts | Controlled by servo |
| Collection bins / output paths | Separates sorted parts | One path for each part category |
| Base plate / frame | Supports the full assembly | Can be 3D printed, fabricated, or mounted to a board |

---

## Documentation / Project Assets

| Item | Purpose | Notes |
|---|---|---|
| Wiring diagram | Documents electrical connections | To be added to `diagrams/` |
| State machine diagram | Documents control logic | To be added to `diagrams/` |
| Project photos | Shows build quality and layout | To be added to `images/` |
| Demo video | Shows system operation | To be added to `videos/` |

---

## Items to Confirm

The following details should be verified and updated as the project develops:

| Item | Detail Needed |
|---|---|
| Arduino model | Uno, Nano, Mega, etc. |
| Servo model | Exact model and torque rating |
| Sensor model | Exact reflectivity sensor/photoresistor module used |
| LCD size | 16x2, 20x4, or other |
| LCD I2C address | Common addresses include `0x27` or `0x3F` |
| External power supply rating | Voltage and current capacity |
| Part material/color | Black/white test pieces or actual sorted objects |
| 3D printed material | PLA, PETG, etc. |
| Fasteners | Screws, nuts, standoffs, mounting hardware |