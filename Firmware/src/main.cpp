#include <Arduino.h>

// Define PWM pins for motors and lights
#define EXTRUDER_1M_PIN PC6
#define EXTRUDER_2M_PIN PB8
#define FEED_1M_PIN PB15
#define FEED_2M_PIN PB9
#define L3_PIN PA0
#define L4_PIN PB0
#define L1_PIN PB10
#define L2_PIN PB11
#define HEATER_PIN PA15

// Define GPIO pins for enable and direction
#define EXTRUDER_1_EN_PIN PB5
#define EXTRUDER_1_DIR_PIN PA7 // idk why this isnt working
#define EXTRUDER_2_EN_PIN PD14
#define EXTRUDER_2_DIR_PIN PD15 // idk why this isnt working
#define FEED_1_EN_PIN PF12 // idk why this isnt working
#define FEED_1_DIR_PIN PF13
#define FEED_2_EN_PIN PE9
#define FEED_2_DIR_PIN PE11

// Function declarations
void setupPins();
void processSerialInput();
void updatePWMValue(int pin, int pwmValue);
void setGPIOState(int pin, bool state);

void setup() {
    Serial.begin(9600);
    setupPins();

    // Print the clock frequency
    Serial.print("CPU Clock Frequency: ");
    Serial.print(F_CPU);
    Serial.println(" Hz");
}

void loop() {
    processSerialInput();
}

void setupPins() {
    // Set PWM pins as output
    pinMode(EXTRUDER_1M_PIN, OUTPUT);
    pinMode(EXTRUDER_2M_PIN, OUTPUT);
    pinMode(FEED_1M_PIN, OUTPUT);
    pinMode(FEED_2M_PIN, OUTPUT);
    pinMode(L3_PIN, OUTPUT);
    pinMode(L4_PIN, OUTPUT);
    pinMode(L1_PIN, OUTPUT);
    pinMode(L2_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);

    // Set GPIO pins as output and pull EN pins high by default
    pinMode(EXTRUDER_1_EN_PIN, OUTPUT);
    digitalWrite(EXTRUDER_1_EN_PIN, HIGH);

    // Clear previous configuration and set PA7 as GPIO output
    pinMode(EXTRUDER_1_DIR_PIN, OUTPUT);
    digitalWrite(EXTRUDER_1_DIR_PIN, LOW); // Ensure it's set to a known state

    pinMode(EXTRUDER_2_EN_PIN, OUTPUT);
    digitalWrite(EXTRUDER_2_EN_PIN, HIGH);

    // Clear previous configuration and set PD15 as GPIO output
    pinMode(EXTRUDER_2_DIR_PIN, OUTPUT);
    digitalWrite(EXTRUDER_2_DIR_PIN, LOW); // Ensure it's set to a known state

    // Clear previous configuration and set PF12 as GPIO output
    pinMode(FEED_1_EN_PIN, OUTPUT);
    digitalWrite(FEED_1_EN_PIN, HIGH);
    digitalWrite(FEED_1_EN_PIN, LOW); // Ensure it's set to a known state

    pinMode(FEED_1_DIR_PIN, OUTPUT);

    pinMode(FEED_2_EN_PIN, OUTPUT);
    digitalWrite(FEED_2_EN_PIN, HIGH);

    pinMode(FEED_2_DIR_PIN, OUTPUT);
}

void processSerialInput() {
    if (Serial.available() > 0) {
        // Read the incoming line
        String input = Serial.readStringUntil('\n');
        
        // Parse the command and value
        int separatorIndex = input.indexOf('_');
        if (separatorIndex != -1) {
            String command = input.substring(0, separatorIndex);
            int value = input.substring(separatorIndex + 1).toInt();

            if (command.startsWith("PWM")) {
                int pinIndex = command.substring(3).toInt();
                updatePWMValue(pinIndex, constrain(value, 0, 255));
            } else if (command.startsWith("GPIO")) {
                int pinIndex = command.substring(4).toInt();
                setGPIOState(pinIndex, value != 0);
            } else {
                Serial.println("Unknown command");
            }
        }
    }
}

void updatePWMValue(int pinIndex, int pwmValue) {
    switch (pinIndex) {
        case 1: analogWrite(EXTRUDER_1M_PIN, pwmValue); break;
        case 2: analogWrite(EXTRUDER_2M_PIN, pwmValue); break;
        case 3: analogWrite(FEED_1M_PIN, pwmValue); break;
        case 4: analogWrite(FEED_2M_PIN, pwmValue); break;
        case 5: analogWrite(L3_PIN, pwmValue); break;
        case 6: analogWrite(L4_PIN, pwmValue); break;
        case 7: analogWrite(L1_PIN, pwmValue); break;
        case 8: analogWrite(L2_PIN, pwmValue); break;
        case 9: analogWrite(HEATER_PIN, pwmValue); break;
        default: Serial.println("Invalid PWM index"); return;
    }
    Serial.print("PWM");
    Serial.print(pinIndex);
    Serial.print(" set to ");
    Serial.println(pwmValue);
}

void setGPIOState(int pinIndex, bool state) {
    switch (pinIndex) {
        case 1: digitalWrite(EXTRUDER_1_EN_PIN, state); break;
        case 2: digitalWrite(EXTRUDER_1_DIR_PIN, state); break;
        case 3: digitalWrite(EXTRUDER_2_EN_PIN, state); break;
        case 4: digitalWrite(EXTRUDER_2_DIR_PIN, state); break;
        case 5: digitalWrite(FEED_1_EN_PIN, state); break;
        case 6: digitalWrite(FEED_1_DIR_PIN, state); break;
        case 7: digitalWrite(FEED_2_EN_PIN, state); break;
        case 8: digitalWrite(FEED_2_DIR_PIN, state); break;
        default: Serial.println("Invalid GPIO index"); return;
    }
    Serial.print("GPIO");
    Serial.print(pinIndex);
    Serial.print(" set to ");
    Serial.println(state ? "ON" : "OFF");
}