// I2C library
#include <Wire.h>

// On most arduino, pins 2 and 3 are the external interrupts INT0 and INT1
// These two pins have their own interrupt function.
const uint8_t pins[] = {2, 3};

// Motor phases or pulses per full motor rotation
const uint8_t phases[2] = {12, 12};


// counters
volatile uint32_t counters[2] = {0};

// time of previous calculation
unsigned long previous_millis = 0;

// latest rpm measurement
uint32_t rpm[2] = {0};

void setup() {
  // Configure interrupts
  pinMode(pins[0], INPUT);
  pinMode(pins[1], INPUT);
  attachInterrupt(digitalPinToInterrupt(pins[0]), pin_0_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(pins[1]), pin_1_interrupt, FALLING);

  // From slave_sender example
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  // Nothing to do in the loop, maybe want to reset some state here after some timeout
  delay(100); // necessary for the i2c request
}

void requestEvent() {
  unsigned long current_millis = millis();
  unsigned long delta = current_millis - previous_millis;
  previous_millis = current_millis;

  // Briefly disable interrupts, because I am not sure if uint32 write operations are atomic
//  noInterrupts();

  // Make sure not to divide by zero
  if (delta > 0) {
    rpm[0] = (counters[0] * 60000) / (phases[0] * delta);
    rpm[1] = (counters[1] * 60000) / (phases[1] * delta);

  } else {
    rpm[0] = rpm[1] = 0;

  }

  counters[0] = counters[1] = 0;
//  interrupts();

  Wire.write((byte*)rpm, 8);
}

// interrupt functions
void pin_0_interrupt() {
  counters[0]++;
}

void pin_1_interrupt() {
  counters[1]++;
}
