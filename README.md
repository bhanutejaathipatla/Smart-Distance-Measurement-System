# Smart-Distance-Measurement-System
## What it does

Measures distance without touching the object, using an ESP32 and a waterproof ultrasonic sensor (RCWL-1655 / JSN-SR04T). Regular ultrasonic sensors like the HC-SR04 have exposed boards that can't handle moisture, so this one was picked specifically for outdoor/wet use cases — think water tank level monitoring.

## The idea: time-of-flight

Send a sound pulse, time how long it takes to bounce back off an object, and use the speed of sound to work out the distance:

```
distance (cm) = (time_of_flight_us * 0.0343) / 2
```

Divided by 2 because the timed pulse covers the round trip (sensor to object and back), not just one way.

## Hardware

- ESP32 (NodeMCU-32S)
- RCWL-1655 / JSN-SR04T ultrasonic sensor
- A couple of resistors for a voltage divider

### Wiring

| Sensor | ESP32 |
|---|---|
| VDD | 5V |
| GND | GND |
| TRIG | GPIO 5 |
| ECHO | GPIO 18 (through a voltage divider - see below) |

**Don't skip the voltage divider.** The sensor's ECHO pin outputs 5V, but the ESP32's GPIOs are only rated for 3.3V. A simple 1kΩ/2kΩ divider on that line brings it down to a safe level before it reaches the board.

## How the code works

1. Send a 10us pulse on TRIG
2. Sensor fires an ultrasonic burst, ECHO goes high
3. ECHO drops low the moment the echo comes back - `pulseIn()` measures how long it stayed high
4. Convert that time into distance using the formula above
5. Ignore anything under ~20cm (the sensor's blind spot) or over 450cm (out of range)
6. Flag the distance as "very near" (under 100cm), "near" (under 200cm), or clear, and print it to Serial

## Setup

1. Install ESP32 board support in Arduino IDE
2. Wire up the sensor per the table above, voltage divider included
3. Upload `smart_distance_measurement.ino`
4. Open Serial Monitor at 115200 baud

## Results from testing

- Reliable between 25cm and 450cm
- About ±1cm accuracy

## What we'd improve

- No temperature compensation - the speed of sound constant assumes ~20°C, so accuracy drifts if it's a lot hotter or colder
- No filtering or averaging, so a noisy environment could give jittery readings
- Only prints to Serial right now - an actual deployment (like a water tank) would need a display, alert, or WiFi reporting instead of needing a laptop plugged in the whole time

## References

- RCWL-1655/JSN-SR04T datasheet, v2.0, 2019
- ESP32 datasheet, Espressif, v3.0, 2023
- Arduino `pulseIn()` documentation

## Author
Bhanuteja Athipatla
