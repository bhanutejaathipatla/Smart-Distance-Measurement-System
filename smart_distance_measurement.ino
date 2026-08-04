// Smart Distance Measurement System
// ESP32 (NodeMCU-32S) + RCWL-1655 (JSN-SR04T) waterproof ultrasonic sensor
//
// Works on Ultrasonic Time-of-Flight: times how long a sound pulse takes to
// hit an object and echo back, then converts that into a distance in cm.

#define TRIG_PIN 5   // ESP32 GPIO wired to sensor TRIG
#define ECHO_PIN 18  // ESP32 GPIO wired to sensor ECHO (through a voltage divider - see README)

const float SOUND_SPEED_CM_PER_US = 0.0343; // speed of sound at ~20C

const unsigned long MEASURE_INTERVAL_MS = 500;   // how often to take a reading
const unsigned long PULSE_TIMEOUT_US    = 30000; // give up after this, roughly 5m range

// Sensor can't reliably see anything closer than this - its own "blind zone"
const float MIN_VALID_DISTANCE_CM = 20.0;
const float MAX_VALID_DISTANCE_CM = 450.0;

// proximity thresholds
const float NEAR_THRESHOLD_CM = 200.0;
const float VERY_NEAR_THRESHOLD_CM = 100.0;

unsigned long lastMeasureTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  Serial.println("Smart Distance Measurement System - Ready");
}

void loop() {
  if (millis() - lastMeasureTime >= MEASURE_INTERVAL_MS) {
    lastMeasureTime = millis();

    float distanceCm = measureDistanceCm();
    handleReading(distanceCm);
  }
}

// sends a 10us trigger pulse, times the echo, and converts that into cm.
// returns -1 if no echo came back, or if the reading is outside the sensor's
// usable range (blind zone or too far).
float measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long durationUs = pulseIn(ECHO_PIN, HIGH, PULSE_TIMEOUT_US);

  if (durationUs == 0) {
    return -1; // no echo received
  }

  float distanceCm = (durationUs * SOUND_SPEED_CM_PER_US) / 2.0;

  if (distanceCm < MIN_VALID_DISTANCE_CM || distanceCm > MAX_VALID_DISTANCE_CM) {
    return -1; // outside the sensor's reliable range
  }

  return distanceCm;
}

// classifies the reading and prints it
void handleReading(float distanceCm) {
  if (distanceCm < 0) {
    Serial.println("No obstacle detected (out of range or blind zone)");
    return;
  }

  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.print(" cm -> ");

  if (distanceCm <= VERY_NEAR_THRESHOLD_CM) {
    Serial.println("ALERT: obstacle very near (<= 100 cm)");
  } else if (distanceCm <= NEAR_THRESHOLD_CM) {
    Serial.println("NOTICE: obstacle within 200 cm");
  } else {
    Serial.println("Clear");
  }
}
