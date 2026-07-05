# Wiring Notes

This document tracks wiring decisions, connection requirements, and electrical lessons learned for the Arduino Mini Sorting System.

---

## System Wiring Overview

The mini sorting system uses an Arduino to read sensor input, control a servo diverter, and provide system feedback through an LCD display.

Primary electrical components:

- Arduino controller
- Reflectivity sensor / photoresistor sensor
- Servo motor for part diversion
- I2C LCD display
- External 5V power supply for servo
- Shared ground between Arduino and external power supply

---

## Ground Reference

All control signals need a common electrical reference.

The Arduino ground and the external servo power supply ground should be connected together.

Without a shared ground, the servo signal may behave unpredictably because the Arduino signal does not have the same reference as the servo power circuit.

### Note

The servo can be powered from a separate supply, but the grounds must still be tied together.

---

## Servo Power

The servo should not be powered directly from the Arduino 5V pin if it draws more current than the board can reliably supply.

Servo motors can cause voltage drops, resets, or unstable behavior when powered from the Arduino.

### Recommended setup

- Servo VCC → external 5V supply positive
- Servo GND → external 5V supply ground
- Servo signal → Arduino PWM/control pin
- Arduino GND → external 5V supply ground

### Reason

This separates motor power from logic power while keeping a common signal reference.

---

## Reflectivity Sensor Wiring

The reflectivity sensor provides an analog input to the Arduino.

A previous issue caused the analog reading to stay near `1023`, which suggested the analog input was stuck at full scale.

### Wiring checks

If sensor readings are stuck high or do not change:

- Check sensor power
- Check sensor ground
- Verify the analog signal wire is connected to the correct Arduino analog input
- Confirm the sensor has a proper reference path
- Check for loose jumper wires or breadboard connection issues

---

## Sensor Calibration Wiring Considerations

Sensor readings may change based on:

- Sensor distance from the part
- Ambient light
- Surface color and reflectivity
- Tunnel/enclosure design
- Loose wiring or unstable power

The sensor should be mounted securely before final calibration. If the sensor moves, threshold values may need to be recalibrated.

---

## LCD Wiring

The LCD uses an I2C connection.

Typical I2C wiring:

- VCC → 5V
- GND → GND
- SDA → Arduino SDA
- SCL → Arduino SCL

The exact SDA/SCL pins depend on the Arduino board being used.

---

## Wiring Best Practices

- Keep power and ground connections secure.
- Avoid loose jumper wires where possible.
- Separate servo power from Arduino logic power when needed.
- Use a common ground between all signal-connected devices.
- Label wires during later cleanup.
- Confirm wiring before changing code when hardware behavior is inconsistent.

---

## Current Wiring Items to Verify

- Confirm Arduino board model and exact pin assignments.
- Record the analog input pin used by the reflectivity sensor.
- Record the servo control pin.
- Record LCD I2C address.
- Confirm whether the servo uses a separate 5V supply.
- Confirm all grounds are tied together.
- Add a wiring diagram to the `diagrams/` folder.