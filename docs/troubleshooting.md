# Troubleshooting Notes

This document tracks problems encountered during development of the Arduino Mini Sorting System, along with suspected causes, fixes, and lessons learned.

---

## Sensor Reading Saturated at 1023

### Symptom
The reflectivity sensor reading stayed at or near `1023`, regardless of the part being detected.

### Likely Cause
The sensor circuit did not have a proper reference path, causing the Arduino analog input to read maximum value.

### Fix
Restored the proper ground/reference connection and verified the sensor wiring.

### Result
Analog readings began changing in response to different surfaces and part colors.

### Lesson Learned
Analog sensors require a stable reference path. If readings are stuck at full scale, check wiring, ground, and voltage reference before changing code.

---

## Difficulty Distinguishing Black, White, and Empty Conditions

### Symptom
The system needed to reliably identify different conditions:
- No part present
- Black part
- White part

### Likely Cause
The raw sensor values were close enough that the thresholds needed tuning.

### Fix
Observed sensor readings under each condition and planned threshold ranges for:
- Empty sensor state
- Black part detected
- White part detected

### Result
The system can be improved by documenting actual measured values and using those values to set detection thresholds.

### Lesson Learned
Sensor thresholds should be based on measured data, not guesses. Calibration should be documented so the system can be adjusted later.

---

## False Counts or Double Detection

### Symptom
A single part could potentially be counted more than once as it passed through the sensor area.

### Likely Cause
The sensor may continue detecting the same object across multiple Arduino loop cycles.

### Fix
Use state-machine logic so the system only counts a part once per detection cycle. The system should wait until the sensor returns to an empty state before accepting another part.

### Result
This prevents repeated counts from one physical part.

### Lesson Learned
Real-world automation systems need edge detection or state-based logic. It is usually not enough to check whether a sensor is currently active.

---

## Servo Timing Issues

### Symptom
The diverter servo may not reach its intended position before the part arrives or before the system accepts the next part.

### Likely Cause
Mechanical motion takes time, and the code must allow enough time for the servo to move fully.

### Fix
Add or tune timing delays around servo movement. Confirm that the diverter reaches position before allowing the next sort action.

### Result
More reliable part routing.

### Lesson Learned
Mechanical systems do not respond instantly. Control logic must account for actuation time.

---

## Servo Power Instability / Brownout Risk

### Symptom
The Arduino may reset, behave unpredictably, or produce inconsistent servo movement when the servo is powered directly from the Arduino.

### Likely Cause
Servo motors can draw more current than the Arduino 5V pin can reliably provide.

### Fix
Power the servo from a separate 5V supply while keeping a shared ground between the Arduino and the servo supply.

### Result
More stable servo operation and reduced risk of Arduino resets.

### Lesson Learned
Separate motor/servo power from logic power when needed, but always maintain a common ground for signal reference.

---

## Repository / Git Setup Issue: `.gitignore` Created as Folder

### Symptom
The `.gitignore` item could not be edited like a normal file in VS Code.

### Cause
`.gitignore` was accidentally created as a folder instead of a text file.

### Fix
Deleted the `.gitignore` folder and recreated `.gitignore` as a file. Added:

```text
.vscode/