
# Testing Results and Validation

## 1. Introduction

This document describes the testing activities conducted on the Digital Scoliometer prototype based on ESP32 and MPU6050.

The testing process focuses on hardware functionality, sensor processing, measurement control, display functionality, battery monitoring, and communication with the Laravel web application.

Testing results should be updated based on actual hardware observations and recorded measurements.

## 2. Hardware Testing

| No. | Component | Testing Objective | Status |
|---|---|---|---|
| 1 | ESP32 | Verify microcontroller initialization | To be documented |
| 2 | MPU6050 | Verify accelerometer and gyroscope readings | To be documented |
| 3 | LCD 16x4 | Verify display functionality | To be documented |
| 4 | Position Button | Verify position selection | To be documented |
| 5 | Start/Stop Button | Verify measurement control | To be documented |
| 6 | Green LED | Verify indicator functionality | To be documented |
| 7 | Red LED | Verify error indication | To be documented |
| 8 | Buzzer | Verify audible feedback | To be documented |
| 9 | Battery System | Verify power supply functionality | To be documented |
| 10 | Voltage Sensor | Verify battery voltage readings | To be documented |

## 3. Sensor and Processing Testing

### 3.1 MPU6050 Sensor Reading

Objective:

Verify that the MPU6050 provides accelerometer and gyroscope readings through I2C communication.

Observation:

Record the sensor response when the device is stationary and when its orientation changes.

### 3.2 Kalman Filter Testing

Objective:

Evaluate the behavior of the Kalman Filter when processing sensor measurements.

Observation:

Compare raw accelerometer readings with filtered angle readings.

The evaluation should include:

- Sensor fluctuations while stationary.
- Response when the sensor changes orientation.
- Stability after movement stops.
- Response time during angle changes.

### 3.3 Auto-Zero Calibration

Objective:

Verify that the system establishes a reference angle during initialization.

Procedure:

1. Place the device in its intended initial reference position.
2. Power on the device.
3. Allow the calibration process to complete.
4. Observe the calculated zero offset.
5. Check whether the ATR reading is close to zero in the reference position.

### 3.4 Peak ATR Detection

Objective:

Verify that the system records the maximum absolute ATR value during an active measurement session.

Procedure:

1. Start measurement.
2. Change the sensor orientation.
3. Observe the current ATR value.
4. Move the sensor through different angles.
5. Stop measurement.
6. Compare the recorded maximum ATR value with the observed readings.

## 4. User Interface Testing

| No. | Feature | Expected Behavior | Status |
|---|---|---|---|
| 1 | LCD Row 1 | Displays current ATR angle | To be documented |
| 2 | LCD Row 2 | Displays selected position | To be documented |
| 3 | LCD Row 3 | Displays patient name | To be documented |
| 4 | LCD Row 4 | Displays estimated battery percentage | To be documented |
| 5 | Position Button | Changes measurement position | To be documented |
| 6 | Start/Stop Button | Starts and stops measurement | To be documented |

## 5. IoT Communication Testing

### 5.1 WiFi Connection

Objective:

Verify that the ESP32 can connect to the configured WiFi network.

Expected behavior:

The device successfully connects to the network and can communicate with the Laravel server.

### 5.2 Patient Queue Retrieval

Endpoint:

```http
GET /api/iot/check-queue
```

Objective:

Verify that the ESP32 can retrieve patient information from the web application.

Expected behavior:

- The device checks the patient queue.
- A valid patient ID is retrieved when a measurement session is available.
- Patient information is displayed when provided by the API.
- Measurement cannot start without a valid patient.

### 5.3 Measurement Data Transmission

Endpoint:

```http
POST /api/iot/send-measure
```

Objective:

Verify that measurement results can be transmitted to the Laravel application.

Example JSON payload:

```json
{
    "patient_id": 15,
    "atr_angle": 12.5,
    "position": "Thoracic"
}
```

Expected behavior:

- The ESP32 sends the patient ID.
- The ESP32 sends the maximum ATR value.
- The ESP32 sends the selected measurement position.
- The server validates and processes the request.
- The measurement is stored according to the web application's implementation.

## 6. Battery Monitoring Testing

Objective:

Verify that the voltage sensor provides analog readings corresponding to the battery voltage.

Testing procedure:

1. Measure the battery voltage using a multimeter.
2. Read the analog value from ESP32 GPIO34.
3. Calculate the voltage reported by the firmware.
4. Compare the firmware reading with the multimeter.
5. Adjust the voltage conversion factor if necessary.
6. Verify the estimated battery percentage.

Important:

Battery percentage estimation is based on voltage thresholds and should not be interpreted as an exact measurement of remaining battery capacity.

## 7. Test Result Documentation

Use the following table to record actual test results.

| Test | Expected Result | Actual Result | Status |
|---|---|---|---|
| ESP32 initialization | Successful initialization | Not recorded | Pending |
| MPU6050 reading | Sensor data available | Not recorded | Pending |
| Kalman Filter | More stable angle estimation | Not recorded | Pending |
| Auto-zero calibration | Reference angle established | Not recorded | Pending |
| Position selection | Correct position displayed | Not recorded | Pending |
| Peak ATR detection | Highest absolute value recorded | Not recorded | Pending |
| LCD display | Information displayed correctly | Not recorded | Pending |
| WiFi connection | Server reachable | Not recorded | Pending |
| Patient queue | Patient ID retrieved | Not recorded | Pending |
| Measurement upload | Data accepted by server | Not recorded | Pending |
| Battery monitoring | Voltage reading available | Not recorded | Pending |

## 8. Known Limitations

The following aspects require additional testing and validation:

1. Measurement accuracy against a calibrated reference instrument.
2. Repeatability of ATR measurements.
3. Sensor placement and mounting consistency.
4. Kalman Filter optimization.
5. Battery voltage conversion accuracy.
6. Battery percentage estimation under different loads.
7. Long-term WiFi communication reliability.
8. Server-side data integrity and measurement synchronization.

## 9. Conclusion

The testing process is intended to evaluate the functionality and reliability of the Digital Scoliometer prototype.

The system combines sensor acquisition, embedded data processing, measurement control, user interface, and IoT communication.

Further testing is necessary to establish measurement accuracy, repeatability, and suitability for its intended screening application.

All test results should be updated using actual measurements obtained from the physical prototype.
