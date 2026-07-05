#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// --------------------------------------------------
// MINI AUTOMATED SORTING SYSTEM
// State machine 
// --------------------------------------------------

// ---------------- PIN ASSIGNMENTS ----------------
const int RUN_BUTTON_PIN = 2;       // Run / Start button
const int STOP_BUTTON_PIN = 3;      // Stop / Idle button
const int SENSOR_PIN = A0;          // LDR analog output
const int SERVO_PIN = 9;            // Servo control signal
const int LIGHT_LED_PIN = 7;        // White illumination LED

const int STATUS_RUNNING_PIN = 13;
const int STATUS_IDLE_PIN = 11;

// ---------------- LCD SETUP ----------------

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ---------------- SERVO SETUP ----------------
Servo diverterServo;

// Servo positions - adjust after physical testing
const int SERVO_HOME_ANGLE   = 90;    // Default path to Bin 1 (accepted)
const int SERVO_REJECT_ANGLE = 180;   // Divert white chip to Bin 2 (rejected)

// ---------------- SENSOR THRESHOLDS ----------------
// reading < EMPTY_THRESHOLD  -> no chip / empty tunnel
// reading >= EMPTY_THRESHOLD and reading < WHITE_THRESHOLD -> black chip
// reading >= WHITE_THRESHOLD -> white chip

const int BLACK_MAX = 895;   // adjust from calibration
const int WHITE_MIN = 908;   // adjust from calibration


// ---------------- TIMING VALUES ----------------
// These will need tuning during testing.
const unsigned long SERVO_REJECT_HOLD_MS = 700;   // How long servo stays in reject position
const unsigned long SENSOR_SETTLE_MS     = 10;    // Small delay after chip enters sensing zone
const unsigned long LCD_UPDATE_MS        = 200;   // display refresh

// ---------------- COUNTERS ----------------
unsigned long acceptedCount = 0;
unsigned long rejectedCount = 0;

// ---------------- SENSOR VARIABLES ----------------
int sensorValue = 0;
String lastPart = "NONE";

// ---------------- STATE MACHINE ----------------
enum SystemState
{
  IDLE,
  WAIT_FOR_PART,
  READ_PART,
  DIVERT_REJECT,
  COUNT_ACCEPT,
  COUNT_REJECT,
  RETURN_HOME,
  WAIT_PART_CLEAR
};

SystemState currentState = IDLE;

// Used for non-blocking timing
unsigned long stateStartTime = 0;

// ---------------- FUNCTION PROTOTYPES ----------------
void updateLCD();
void goToState(SystemState newState);
bool runButtonPressed();
bool stopButtonPressed();
bool partPresent(int reading);
bool isWhitePart(int reading);
bool tunnelClear(int reading);
void printDebugInfo();

// --------------------------------------------------
// SETUP
// --------------------------------------------------
void setup()
{
  Serial.begin(9600);

  // Configure button pins using internal pull-ups
  // Pressed = LOW, released = HIGH
  pinMode(RUN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);

  // Illumination LED
  pinMode(LIGHT_LED_PIN, OUTPUT);
  digitalWrite(LIGHT_LED_PIN, LOW);

  // Attach servo and move to home position
  diverterServo.attach(SERVO_PIN);
  diverterServo.write(SERVO_HOME_ANGLE);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Startup state
  currentState = IDLE;
  updateLCD();

  Serial.println("System initialized.");
  Serial.println("State: IDLE");
}

// --------------------------------------------------
// LOOP
// --------------------------------------------------
void loop()
{
  // Stop button
  if (stopButtonPressed())
  {
    // Return system to IDLE
    digitalWrite(LIGHT_LED_PIN, LOW);
    digitalWrite(STATUS_RUNNING_PIN, LOW);
    diverterServo.write(SERVO_HOME_ANGLE);
    goToState(IDLE);
  }

  // Read sensor continuously for debugging / state logic
  sensorValue = analogRead(SENSOR_PIN);

  // Uncomment for live debugging during calibration
  printDebugInfo();

  switch (currentState)
  {
    case IDLE:
      // -----------------------------------------
      // STEP 1: System powers up -> LCD displays IDLE
      // STEP 2: Wait for operator to press RUN
      // -----------------------------------------
      digitalWrite(LIGHT_LED_PIN, LOW);
      diverterServo.write(SERVO_HOME_ANGLE);
      acceptedCount = -1;
      rejectedCount = 0;
      digitalWrite(STATUS_IDLE_PIN, HIGH);

      if (runButtonPressed())
      {
        digitalWrite(LIGHT_LED_PIN, HIGH);   // Turn on tunnel illumination
        digitalWrite(STATUS_RUNNING_PIN, HIGH);
        digitalWrite(STATUS_IDLE_PIN, LOW);
        goToState(WAIT_FOR_PART);
      }
      break;

    case WAIT_FOR_PART:
      // -----------------------------------------
      // STEP 3: Servo is home/default
      // STEP 4: Illumination LED is ON
      // STEP 5: Wait for chip to enter sensing tunnel
      // -----------------------------------------
      if (partPresent(sensorValue))
      {
        goToState(READ_PART);
      }
      break;

    case READ_PART:
      // -----------------------------------------
      // STEP 6: Chip enters tunnel
      // STEP 7: Arduino reads reflectivity
      // STEP 8/9: Classify part as WHITE or BLACK
      // -----------------------------------------
      if (millis() - stateStartTime >= SENSOR_SETTLE_MS)
      {
        sensorValue = analogRead(SENSOR_PIN);
          if (isWhitePart(sensorValue))
          {
            lastPart = "WHITE";
            goToState(DIVERT_REJECT);
          }
          else if (isBlackPart(sensorValue))
          {
            lastPart = "BLACK";
            goToState(COUNT_ACCEPT);
          }
          else
          {
          // EMPTY — ignore and wait
          // optionally stay in READ_PART or go back to WAIT_FOR_PART
          }
       
      }
      break;

    case DIVERT_REJECT:
      // -----------------------------------------
      // STEP 9: White chip detected
      // STEP 10: Move servo to reject position
      // -----------------------------------------
      diverterServo.write(SERVO_REJECT_ANGLE);
      goToState(COUNT_REJECT);
      break;

    case COUNT_ACCEPT:
      // -----------------------------------------
      // STEP 10: Black chip goes to Bin 1
      // Increment accepted count
      // -----------------------------------------
      acceptedCount++;
      updateLCD();
      goToState(WAIT_PART_CLEAR);
      break;

    case COUNT_REJECT:
      // -----------------------------------------
      // STEP 11: White chip routed to Bin 2
      // Increment rejected count
      // -----------------------------------------
      rejectedCount++;
      updateLCD();
      goToState(RETURN_HOME);
      break;

    case RETURN_HOME:
      // -----------------------------------------
      // STEP 12: Hold reject position briefly
      // STEP 13: Return servo to home
      // -----------------------------------------
      if (millis() - stateStartTime >= SERVO_REJECT_HOLD_MS)
      {
        diverterServo.write(SERVO_HOME_ANGLE);
        goToState(WAIT_PART_CLEAR);
      }
      break;

    case WAIT_PART_CLEAR:
      // -----------------------------------------
      // STEP 14: Wait for chip to leave tunnel
      // Prevent one chip from being counted multiple times
      // STEP 15: Return to waiting for next chip
      // -----------------------------------------
      if (tunnelClear(sensorValue))
      {
        updateLCD();
        goToState(WAIT_FOR_PART);
      }
      break;
  }
}

// --------------------------------------------------
// STATE TRANSITION HELPER
// --------------------------------------------------
void goToState(SystemState newState)
{
  currentState = newState;
  stateStartTime = millis();

  // Debug print for transitions
  Serial.print("Transition to state: ");
  switch (currentState)
  {
    case IDLE:            Serial.println("IDLE"); break;
    case WAIT_FOR_PART:   Serial.println("WAIT_FOR_PART"); break;
    case READ_PART:       Serial.println("READ_PART"); break;
    case DIVERT_REJECT:   Serial.println("DIVERT_REJECT"); break;
    case COUNT_ACCEPT:    Serial.println("COUNT_ACCEPT"); break;
    case COUNT_REJECT:    Serial.println("COUNT_REJECT"); break;
    case RETURN_HOME:     Serial.println("RETURN_HOME"); break;
    case WAIT_PART_CLEAR: Serial.println("WAIT_PART_CLEAR"); break;
  }

  updateLCD();
}

// --------------------------------------------------
// BUTTON FUNCTIONS
// --------------------------------------------------
bool runButtonPressed()
{
  static bool lastState = HIGH;
  bool currentReading = digitalRead(RUN_BUTTON_PIN);

  // Detect falling edge: HIGH -> LOW
  if (lastState == HIGH && currentReading == LOW)
  {
    delay(20); // simple debounce
    lastState = currentReading;
    return true;
  }

  lastState = currentReading;
  return false;
}

bool stopButtonPressed()
{
  static bool lastState = HIGH;
  bool currentReading = digitalRead(STOP_BUTTON_PIN);

  // Detect falling edge: HIGH -> LOW
  if (lastState == HIGH && currentReading == LOW)
  {
    delay(20); // simple debounce
    lastState = currentReading;
    return true;
  }

  lastState = currentReading;
  return false;
}

// --------------------------------------------------
// SENSOR LOGIC FUNCTIONS
// --------------------------------------------------

// --------------------------------------------------
// SENSOR LOGIC FUNCTIONS (UPDATED FOR 3-ZONE MODEL)
// --------------------------------------------------

// Returns true when a BLACK chip is detected (low reflectivity)
bool isBlackPart(int reading)
{
  return reading <= BLACK_MAX;
}

// Returns true when a WHITE chip is detected (high reflectivity)
bool isWhitePart(int reading)
{
  return reading >= WHITE_MIN;
}

// Returns true when NO chip is present (middle range)
bool tunnelClear(int reading)
{
  return reading > BLACK_MAX && reading < WHITE_MIN;
}

// Returns true when ANY chip is present
bool partPresent(int reading)
{
  return isBlackPart(reading) || isWhitePart(reading);
}

// --------------------------------------------------
// LCD UPDATE
// --------------------------------------------------
void updateLCD()
{
  lcd.clear();

  // Line 1: System status
  lcd.setCursor(0, 0);
  lcd.print("System: ");
  if (currentState == IDLE)
  {
    lcd.print("IDLE");
  }
  else
  {
    lcd.print("RUNNING");
  }

  // Line 2: Last detected part
  lcd.setCursor(0, 1);
  lcd.print("Last: ");
  lcd.print(lastPart);

  // Line 3: Accepted count
  lcd.setCursor(0, 2);
  lcd.print("Accepted: ");
  lcd.print(acceptedCount);

  // Line 4: Rejected count
  lcd.setCursor(0, 3);
  lcd.print("Rejected: ");
  lcd.print(rejectedCount);
}

// --------------------------------------------------
// SERIAL DEBUG OUTPUT
// --------------------------------------------------
void printDebugInfo()
{
  static unsigned long lastPrintTime = 0;

  if (millis() - lastPrintTime >= 200)
  {
    lastPrintTime = millis();

    Serial.print("Sensor: ");
    Serial.print(sensorValue);
    Serial.print(" | State: ");
    Serial.print(currentState);
    Serial.print(" | Accepted: ");
    Serial.print(acceptedCount);
    Serial.print(" | Rejected: ");
    Serial.println(rejectedCount);
  }
}