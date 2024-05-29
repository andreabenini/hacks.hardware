#include "main.h"

// I/O
#define PIN_RELAY       0                       // Relay [ON/OFF]               (output)
#define PIN_BUTTON      1                       // Push button                  (input )
#define PIN_DETECTION   2                       // External detection pin       (input )
#define PIN_SHUTDOWN    4                       // Output signal for shutdown   (output)
//
#define DELAY_BOUNCE    100                     // button bounce delay [ms]
#define RELAY_OFF       HIGH
#define RELAY_ON        LOW
#define STATE_OFF       0
#define STATE_ON        1
#define STATE_SHUTDOWN  2
#define TIME_STARTUP    40                      // Appliance startup time                               [seconds]
#define TIME_SHUTDOWN   40                      // Appliance max time for safe shutdown                 [seconds]
#define TIME_DETECTION  10                      // If not detected for 5secs it can be shutted off      [seconds]

byte
    mode,                                       // [State Finite] mode
    applianceState,                             // Remote appliance state (current and previous state)
    applianceDetected,                          // Appliance detected at least one time. Without detection do not turn it off, never.
    buttonState,    buttonStatePrevious;        // Button state (current and previous state)
unsigned long
    modeStart;                                  // Time spent after last switching/change [mode] [with millis()]

// Program setup
void setup() {
    pinMode(PIN_BUTTON,     INPUT);
    pinMode(PIN_DETECTION,  INPUT);
    pinMode(PIN_RELAY,      OUTPUT);
    pinMode(PIN_SHUTDOWN,   OUTPUT);
    modeOff();
} /**/

// Main loop
void loop() {
    buttonRead();
    switch (mode) {
    case STATE_SHUTDOWN:
        turnOffIfNeeded();
        applianceDetectShutdown();
        break;
    case STATE_ON:
        applianceRead();
        break;
    }
} /**/

// Read push button
void buttonRead() {
    buttonState = digitalRead(PIN_BUTTON);
    if (buttonState != buttonStatePrevious) {
        if (buttonState == HIGH) {                              // Button pressed
            switch (mode) {                                     // Detect status
            case STATE_OFF:                                     // Turn ON the appliance
                modeOn();
                break;
            case STATE_ON:                                      // Turn it OFF, where possible
                // Sending shutdown signal to appliance (only if appliance startup time has elapsed)
                if (millis()-modeStart > (unsigned long) TIME_STARTUP*1000) {
                    // Shutdown mode setup
                    digitalWrite(PIN_SHUTDOWN, HIGH);
                    mode = STATE_SHUTDOWN;
                    modeStart = millis();
                }
                break;
            }
        }
        buttonStatePrevious = buttonState;
        delay(DELAY_BOUNCE);
    }
} /**/

// Turn appliance on
void modeOn() {
    mode = STATE_ON;
    modeStart = millis();
    applianceDetected = 0;
    digitalWrite(PIN_RELAY, RELAY_ON);
} /**/
// Turn appliance off
void modeOff() {
    mode = STATE_OFF;
    modeStart = 0;
    digitalWrite(PIN_SHUTDOWN, LOW);
    digitalWrite(PIN_RELAY,    RELAY_OFF);
    buttonState = buttonStatePrevious = LOW;
} /**/
// Turn off the appliance after a while, if needed
void turnOffIfNeeded() {
    // Remote not responding on time, brutal shutdown
    if (millis()-modeStart > (unsigned long) TIME_SHUTDOWN*1000) {
        modeOff();
    }
} /**/

// Detect if appliance has been powered off or if it's still ON
void applianceDetectShutdown() {
    applianceState = digitalRead(PIN_DETECTION);
    if (applianceState == LOW) {                                // appliance state changed (no input voltage)
        // Boot time expired, it was previously detected but now it's off
        if (applianceDetected > 0) {
            if (applianceDetected <= TIME_DETECTION) {          // wait for a while...
                delay(1000);
                applianceDetected++;
            } else {                                            // ...and turn it off after it
                modeOff();
            }
        }
    } else {
        applianceDetected = 1;                                  // Detected at least one time !
    }
} /**/

// Read Appliance State when relay is ON, if it's not detected for more than TIME_DETECTION times relay will be turned off
void applianceRead() {
    applianceState = digitalRead(PIN_DETECTION);
    if (applianceState == LOW) {                                // appliance state changed (no input voltage from device)
        // Boot time expired, it was previously detected but now it's off
        if (millis()-modeStart > (unsigned long)TIME_STARTUP*1000  &&  applianceDetected > 0) {
            if (applianceDetected <= TIME_DETECTION) {          // wait for a while...
                delay(1000);
                applianceDetected++;
            } else {                                            // ...and turn it off if it's not detected for a while
                modeOff();
            }
        }
    } else {
        applianceDetected = 1;                                  // Detected at least one time !
    }
} /**/
