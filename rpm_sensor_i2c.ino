// I2C library
#include <Wire.h>

// On most arduino, pins 2 and 3 are the external interrupts INT0 and INT1
// These two pins have their own interrupt function.
const uint8_t pins[] = {2, 3};

// Optional pin to signal 'data ready' to the I2C master
const uint8_t data_ready_pin = 4;

// Interval at which to perform the a measurement (milliseconds)
const long interval  = 100; // 100ms = 10Hz

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

  pinMode(data_ready_pin, OUTPUT);
  digitalWrite(data_ready_pin, LOW);  

  // From slave_sender example
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  // Busy-wait until sufficient time has past
  // See example blink without delay
  
  unsigned long current_millis = millis();

  if (current_millis - previous_millis >= interval) {
    previous_millis = current_millis;

    // Briefly disable interrupts, because I am not sure if uint32 write operations are atomic
    noInterrupts();
    rpm[0] = (counters[0] * 60000) / (phases[0] * interval);
    rpm[1] = (counters[1] * 60000) / (phases[1] * interval);
    interrupts();

    // Signal data-ready to master device
    digitalWrite(data_ready_pin, HIGH);
  }
}

void requestEvent() {
  digitalWrite(data_ready_pin, LOW);
  Wire.write((byte*)rpm, sizeof(rpm));
}

// interrupt functions
void pin_0_interrupt() {
  counters[0]++;
}

void pin_1_interrupt() {
  counters[1]++;
}
