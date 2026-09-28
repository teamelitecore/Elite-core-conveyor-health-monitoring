# ELITE-CORE

## Intelligent Monitoring and Prediction of Conveyor Belt Joint Rupture and Damages in Iron Ore Mining Industry

Elite-Core is an IoT-based system for monitoring conveyor belts, especially their joints and splices. It uses sensors, edge processing, fault analysis and a web dashboard to track belt condition.

## Problem

Conveyor belt and splice failures can cause downtime, maintenance problems and safety risks in mining operations.

## Proposed Solution

ELITE-CORE combines:

- Multi-sensor condition monitoring
- ESP32-based data acquisition
- Raspberry Pi edge AI and computer vision
- Sensor fusion
- Belt and splice health assessment
- Fault analysis
- Risk-based alerts
- Historical trend analysis
- Predictive maintenance workflow
- Web-based monitoring dashboard

## Current Prototype

The prototype is based on a 1-meter conveyor belt with two splices.

The current dashboard provides:

- Conveyor Overview
- Health Score
- Sensor Fusion
- Trend Analysis
- Root Cause Analysis
- Damage Map
- Replay / Event History
- What-If Simulation
- Splice Monitoring
- Technician Notes

## Dashboard

🌐 **Live Dashboard:**  
https://elite-core-conveyor.netlify.app

## Technology Stack

### Hardware
ESP32 • Raspberry Pi • MPU6050 • Temperature Sensor • HX711 + Load Cell • Speed Sensor / Encoder • Acoustic Sensor • Camera

### Backend
Python • Flask • SQLite

### Communication
Wi-Fi • MQTT

### AI / Computer Vision
Python • OpenCV • PyTorch • Ultralytics YOLO

### Frontend
HTML • CSS • JavaScript

## System Architecture

Physical Conveyor
       ↓
Multi-Sensor Data Acquisition
       ↓
ESP32 Edge Gateway
       +
Raspberry Pi Edge AI
       ↓
Data Processing
       ↓
Sensor Fusion & Fault Analysis
       ↓
Belt / Splice Health Assessment
       ↓
Risk & Alert System
       ↓
ELITE CORE Dashboard


## Wokwi Prototype

We are using Wokwi to test the ESP32 and sensor setup before connecting the actual hardware.


## Note

The dashboard currently uses simulated data for demonstration. Its health scores and fault results have not yet been tested with calibrated industrial data.
