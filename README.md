
# Digital Scoliometer Based on ESP32 and MPU6050

An IoT-based digital scoliometer prototype designed to measure Angle of Trunk Rotation (ATR) using an MPU6050 accelerometer and gyroscope sensor, ESP32 microcontroller, and a web-based monitoring system.

## Overview

The Digital Scoliometer is an embedded system prototype developed to assist in measuring trunk rotation angles during scoliosis screening activities.

The device uses an MPU6050 sensor to detect changes in orientation. The ESP32 processes sensor data using a Kalman Filter to obtain a more stable angle measurement. The system then calculates and records the maximum ATR value during a measurement session.

Measurement results are transmitted to a Laravel-based web application through a WiFi connection, allowing measurement data to be associated with registered patients and stored in the application's measurement history.

## Project Objectives

- Develop a portable digital scoliometer prototype.
- Measure trunk rotation angles using an MPU6050 sensor.
- Implement Kalman Filtering for sensor data processing.
- Implement automatic zero-offset calibration.
- Capture the maximum ATR value during measurement.
- Display measurement information through a 16x4 I2C LCD.
- Integrate the device with a web-based patient measurement system.
- Implement battery voltage monitoring.

## Features

| Feature | Description |
|---|---|
| ATR Measurement | Measures trunk rotation angle using MPU6050 |
| Kalman Filter | Reduces measurement fluctuations |
| Auto-Zero Calibration | Establishes the initial sensor reference angle |
| Peak Detection | Records the maximum absolute ATR value |
| Position Selection | Selects Thoracic, Lumbar, or Cervical |
| Patient Queue | Retrieves patient information from the web application |
| WiFi Communication | Connects ESP32 to the Laravel API |
| Measurement Upload | Sends measurement results to the server |
| LCD Display | Displays sensor, position, patient, and battery information |
| Battery Monitoring | Estimates battery percentage from voltage readings |
| LED Indicator | Provides visual feedback for device operation |
| Buzzer | Provides audible feedback for user interaction |

## Hardware Components

| No. | Component | Function |
|---|---|---|
| 1 | ESP32 Development Board | Main microcontroller and WiFi communication |
| 2 | MPU6050 | Accelerometer and gyroscope sensor |
| 3 | LCD I2C 16x4 | Displays measurement information |
| 4 | I2C Interface | LCD communication interface |
| 5 | Push Button x2 | Position selection and measurement control |
| 6 | Green LED | Measurement and success indicator |
| 7 | Red LED | Error indicator |
| 8 | Buzzer | Audible notification |
| 9 | Lithium 18650 Battery | Main power source |
| 10 | TP4056 Module | Lithium battery charging and protection module (depending on board variant) |
| 11 | MT3608 DC-DC Boost Converter | Increases battery voltage for the ESP32 power input |
| 12 | Voltage Sensor Module | Measures battery voltage |
| 13 | Connecting Wires | Electrical connections |
| 14 | Enclosure | Physical protection and device housing |

## Software and Technologies

- Arduino IDE
- C/C++
- ESP32 Arduino Core
- ArduinoJson
- WiFi.h
- HTTPClient.h
- Wire.h
- MPU6050 Library
- I2Cdev Library
- LiquidCrystal_I2C
- Laravel REST API
- MySQL (web application database)

## System Architecture

The system consists of three main parts:

1. **Hardware Layer:** ESP32, MPU6050, LCD, buttons, indicators, and battery system.
2. **Processing Layer:** Sensor acquisition, Kalman Filter, auto-zero calibration, and peak ATR detection.
3. **Application Layer:** WiFi communication, Laravel API integration, patient queue retrieval, and measurement data storage.

See [System Architecture](docs/system-architecture.md) for more details.

## Measurement Workflow

1. Power on the device.
2. ESP32 initializes the MPU6050, LCD, and WiFi connection.
3. The system performs initial zero-offset calibration.
4. ESP32 checks the patient queue through the Laravel API.
5. The patient information is displayed when a valid queue is available.
6. Select the measurement position using the position button.
7. Press the start button to begin measurement.
8. The MPU6050 continuously captures orientation data.
9. The system tracks the maximum absolute ATR value.
10. Press the button again to stop measurement.
11. The maximum ATR value is sent to the Laravel server.
12. The server processes and stores the measurement result.

## Repository Structure

```text
scoliometer-digital/
├── README.md
├── .gitignore
├── LICENSE
├── firmware/
│   └── ESP32Scolio/
│       └── main.ino
├── docs/
│   ├── system-architecture.md
│   └── testing-results.md
└── images/
    ├── hardware/
    └── architecture/
```

## Testing and Validation

Testing activities include:

- MPU6050 initialization and sensor readings.
- Kalman Filter response.
- Initial zero-offset calibration.
- Position selection.
- Maximum ATR detection.
- LCD information display.
- WiFi connectivity.
- Patient queue retrieval.
- Measurement data transmission.
- Battery voltage monitoring.

Detailed results and verification status are documented in [Testing Results](docs/testing-results.md).

## Project Limitations

This project is a prototype and is not intended to replace professional medical examination or provide a definitive scoliosis diagnosis.

Measurement accuracy, clinical validity, and repeatability require further testing and validation against an appropriate reference instrument.

Some features may still require additional hardware testing and calibration.

## Future Development

- Improve sensor calibration and measurement accuracy.
- Perform repeatability and reference-instrument validation.
- Implement patient name synchronization and improved LCD information.
- Improve battery voltage measurement accuracy.
- Add measurement history synchronization and communication recovery.
- Improve enclosure design and ergonomics.
- Conduct usability and clinical validation.

## Author

**Firman Nur**

Informatics Engineering Student  
Interest: Embedded Systems, Internet of Things (IoT), and Hardware-Based Applications.

GitHub: [Firman270](https://github.com/Firman270)

---

**Disclaimer:** This device is an academic engineering prototype for research and development purposes. It is not a certified medical device and must not be used as the sole basis for clinical diagnosis or treatment decisions.
