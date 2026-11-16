
# System Architecture

## 1. Introduction

The Digital Scoliometer is an IoT-based measurement prototype developed using an ESP32 microcontroller and an MPU6050 motion sensor.

The system measures trunk rotation angles by capturing changes in sensor orientation. The collected data is processed by the ESP32 and transmitted to a Laravel-based web application through a WiFi connection.

The architecture consists of three primary layers:

1. Hardware Layer
2. Embedded Processing Layer
3. Web Application and Communication Layer

## 2. Hardware Architecture

### 2.1 ESP32 Microcontroller

The ESP32 serves as the main controller of the device.

Its responsibilities include:

- Initializing connected hardware components.
- Reading MPU6050 sensor data.
- Processing orientation data.
- Managing measurement states.
- Controlling the LCD, buttons, LED indicators, and buzzer.
- Monitoring battery voltage.
- Communicating with the Laravel API through WiFi.

### 2.2 MPU6050 Sensor

The MPU6050 combines a three-axis accelerometer and a three-axis gyroscope.

The sensor provides motion and orientation information used to calculate the trunk rotation angle.

The ESP32 reads accelerometer and gyroscope values through the I2C communication interface.

### 2.3 LCD 16x4

The LCD is used to display device information.

The intended display arrangement is:

| LCD Row | Information |
|---|---|
| Row 1 | Current ATR angle |
| Row 2 | Selected measurement position |
| Row 3 | Patient name |
| Row 4 | Battery percentage |

The patient name is obtained from the patient queue response when the API provides the corresponding field.

### 2.4 Input and Output Components

The device uses two push buttons:

- Position Button: Changes the measurement position.
- Start/Stop Button: Starts or stops the measurement process.

Output components:

- Green LED: Measurement activity and successful transmission indication.
- Red LED: Error indication.
- Buzzer: Audible feedback for button interaction and system events.

## 3. Power Supply Architecture

The power system uses a Lithium 18650 battery as the primary energy source.

The power supply configuration consists of:

- Lithium 18650 Battery
- TP4056 Charging Module
- Switch
- MT3608 DC-DC Boost Converter
- Voltage Sensor Module
- ESP32

### Power Connection

```text
       Lithium 18650 Battery
                 |
              TP4056
                 |
               Switch
                 |
          +------+------+
          |             |
      MT3608 VIN+   Voltage Sensor VCC
          |
      MT3608 Boost
          |
      ESP32 VIN

TP4056 OUT- -------- MT3608 VIN-
       |
       +------------ Voltage Sensor GND

Voltage Sensor S -------- ESP32 GPIO34
```

The MT3608 increases the battery voltage to the required input voltage for the ESP32.

The voltage sensor module provides an analog signal that is read through GPIO34.

**Important:** The voltage divider ratio and ADC calibration must be verified against a multimeter. The percentage displayed by the firmware is an estimated battery level, not a direct measurement of remaining battery capacity.

## 4. Embedded Processing Architecture

### 4.1 Sensor Acquisition

The MPU6050 provides accelerometer and gyroscope readings.

The accelerometer values are used to calculate the initial inclination angle:

```cpp
pitchAcc = atan2(accY, accZ) * 180 / PI;
```

The gyroscope provides angular velocity information used during sensor fusion.

### 4.2 Kalman Filter

The system implements a Kalman Filter to combine accelerometer and gyroscope information.

The filter estimates the sensor angle while reducing fluctuations caused by sensor noise.

The implementation uses:

- Q_angle
- Q_bias
- R_measure
- Angle estimation
- Bias estimation
- Error covariance matrix

### 4.3 Auto-Zero Calibration

During initialization, the system collects multiple accelerometer readings.

The average angle is calculated and stored as the zero reference offset.

```cpp
zeroOffset = sum / 200;
```

The ATR value is calculated by subtracting the reference angle from the filtered angle:

```cpp
atrAngle = pitch - zeroOffset;
```

### 4.4 Peak ATR Detection

When measurement starts, the maximum ATR variable is reset.

During measurement, the system compares the absolute current angle with the previously recorded maximum value.

```cpp
if (abs(atrAngle) > maxATR) {
    maxATR = abs(atrAngle);
}
```

The highest absolute angle is retained until measurement stops.

### 4.5 Measurement State

The system uses a measurement state variable:

- `measuring = false`: Measurement is inactive.
- `measuring = true`: Measurement is active.

The Start/Stop button controls the measurement state.

When measurement stops, the maximum ATR value is submitted to the server.

## 5. Communication Architecture

The ESP32 communicates with the Laravel application using HTTP requests over WiFi.

Base API URL:

```text
http://10.99.88.101:8000/api/iot
```

### 5.1 Patient Queue Retrieval

Endpoint:

```http
GET /check-queue
```

Purpose:

- Checks whether a patient is available for measurement.
- Retrieves the patient ID.
- Retrieves patient information when provided by the API.

The ESP32 periodically checks the queue until a valid patient is available.

Example response structure:

```json
{
    "status": "ready",
    "patient_id": 15,
    "patient_name": "Example Patient"
}
```

This is an illustrative response structure; the actual response must follow the Laravel API implementation.

### 5.2 Measurement Data Transmission

Endpoint:

```http
POST /send-measure
```

The ESP32 sends measurement data in JSON format.

Example request:

```json
{
    "patient_id": 15,
    "atr_angle": 12.5,
    "position": "Thoracic"
}
```

The server receives the data and processes it according to the Laravel application's validation and database logic.

## 6. End-to-End Data Flow

```text
+----------------------+
|     MPU6050 Sensor   |
+----------+-----------+
           |
           v
+----------------------+
|    ESP32 Controller  |
|                      |
|  Sensor Acquisition  |
|  Kalman Filter       |
|  Auto-Zero           |
|  Peak ATR Detection  |
+----------+-----------+
           |
           +------------------+
           |                  |
           v                  v
+----------------+   +------------------+
|   LCD 16x4     |   |   WiFi Module    |
+----------------+   +--------+---------+
                              |
                              v
                    +-------------------+
                    |   Laravel API     |
                    |                   |
                    |  check-queue      |
                    |  send-measure     |
                    +---------+---------+
                              |
                              v
                    +-------------------+
                    |   MySQL Database  |
                    +-------------------+
```

## 7. Measurement Sequence

1. Device initialization.
2. WiFi connection.
3. MPU6050 initialization.
4. Automatic zero-offset calibration.
5. Patient queue retrieval.
6. Patient selection through the web application.
7. Measurement position selection.
8. Start measurement.
9. Continuous sensor acquisition.
10. Kalman Filter processing.
11. Maximum ATR calculation.
12. Stop measurement.
13. HTTP POST transmission.
14. Server-side data processing and storage.

## 8. Technical Notes

- Communication between ESP32 and MPU6050 uses I2C.
- LCD communication uses an I2C interface.
- WiFi is used for communication with the Laravel API.
- HTTP is used in the current firmware implementation.
- Battery percentage is estimated from analog voltage readings.
- Actual API response fields must match the firmware JSON parsing implementation.

## 9. Architecture Limitations

The following aspects require further validation:

- Accuracy of ATR measurements.
- Sensor mounting consistency.
- Kalman Filter parameter optimization.
- Battery ADC calibration.
- Reliability of WiFi communication.
- Server-side validation and database consistency.
- Clinical comparison with a reference measurement instrument.
