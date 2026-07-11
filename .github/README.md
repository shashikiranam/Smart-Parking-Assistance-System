# Smart Parking Assistance System

<p align="center">
  <img src="images/banner.png" alt="Smart Parking Assistance System Banner" width="100%">
</p>

<p align="center">

![License](https://img.shields.io/badge/License-MIT-green)
![Embedded C](https://img.shields.io/badge/Language-Embedded%20C-blue)
![STM32](https://img.shields.io/badge/MCU-STM32G071RB-orange)
![Platform](https://img.shields.io/badge/Platform-STM32CubeIDE-red)
![Status](https://img.shields.io/badge/Status-Completed-success)
![Automotive](https://img.shields.io/badge/Domain-Automotive%20Electronics-blueviolet)

</p>

---

# Smart Parking Assistance System

An Embedded Systems project that implements an intelligent parking assistance system using the **STM32 NUCLEO-G071RB** microcontroller.

The system continuously monitors obstacles surrounding a vehicle using multiple ultrasonic sensors and provides parking guidance through LEDs, buzzer alerts, LCD display, and steering assistance logic inspired by modern automotive parking assistance systems.

> **Disclaimer**
>
> This project is an original educational implementation inspired by the behavior of modern automotive parking assistance systems. It is **not** affiliated with, endorsed by, or derived from Tesla or any other manufacturer.

---

# Table of Contents

- Project Overview
- Problem Statement
- Objectives
- Features
- Applications
- Working Principle
- Hardware Used
- Software Used
- Development Environment
- Project Architecture
- Flowcharts
- Hardware Design
- Repository Structure
- Installation
- Build Instructions
- Testing Procedure
- Expected Results
- Advantages
- Limitations
- Future Improvements
- Skills Demonstrated
- Author
- License

---

# Project Overview

Parking large vehicles in confined spaces can be difficult due to limited visibility and blind spots.

This project demonstrates how embedded systems can assist drivers by monitoring the surroundings using ultrasonic sensors and providing intuitive parking guidance through embedded firmware.

The firmware is developed using STM32 HAL drivers with a modular architecture suitable for real-world embedded software development.

---

# Problem Statement

Manual parking increases the risk of:

- Vehicle collisions
- Blind spot accidents
- Driver stress
- Parking inaccuracies

A smart embedded parking assistance system improves parking safety by continuously detecting nearby obstacles and alerting the driver.

---

# Objectives

- Detect nearby obstacles
- Measure obstacle distance
- Guide the driver during parking
- Generate audible warnings
- Display parking information
- Demonstrate modular embedded firmware architecture

---

# Features

- Four HC-SR04 ultrasonic sensors
- Real-time distance measurement
- Distance-based buzzer alerts
- LCD parking status
- LED indicators
- Parking mode selection
- Modular firmware
- STM32 HAL implementation
- Automotive-inspired software architecture

---

# Applications

- Passenger vehicles
- Driver assistance systems
- Embedded systems education
- Automotive electronics training
- STM32 firmware development

---

# Working Principle

1. STM32 initializes all peripherals.
2. Ultrasonic sensors continuously measure obstacle distance.
3. Distance values are filtered.
4. Parking algorithm evaluates obstacle positions.
5. Driver receives guidance through:

- LCD
- LEDs
- Buzzer

---

# Hardware Used

| Component | Quantity |
|------------|----------|
| STM32 NUCLEO-G071RB | 1 |
| HC-SR04 Ultrasonic Sensor | 4 |
| 16x2 LCD (I2C) | 1 |
| Buzzer | 1 |
| LEDs | 2 |
| Push Button | 1 |
| Servo Motor (Optional) | 1 |
| USB Power | 1 |

---

# Software Used

- STM32CubeIDE
- STM32 HAL
- Embedded C
- Git
- GitHub

---

# Development Environment

| Item | Details |
|------|----------|
| IDE | STM32CubeIDE |
| Language | Embedded C |
| MCU | STM32G071RB |
| Framework | STM32 HAL |
| Version Control | Git |

---

# Project Architecture

## System Architecture

<p align="center">
<img src="images/architecture/system_architecture.png" width="90%">
</p>

---

## Software Architecture

<p align="center">
<img src="images/architecture/software_architecture.png" width="90%">
</p>

---

## Firmware Architecture

<p align="center">
<img src="images/architecture/firmware_architecture.png" width="90%">
</p>

---

## Data Flow

<p align="center">
<img src="images/architecture/data_flow.png" width="90%">
</p>

---

# Flowcharts

## System Flowchart

<p align="center">
<img src="images/flowcharts/system_flowchart.png" width="90%">
</p>

---

## Firmware Flowchart

<p align="center">
<img src="images/flowcharts/firmware_flowchart.png" width="90%">
</p>

---

## Parking Decision Flowchart

<p align="center">
<img src="images/flowcharts/parking_decision_flowchart.png" width="90%">
</p>

---

## State Machine

<p align="center">
<img src="images/flowcharts/state_machine.png" width="90%">
</p>

---

# Hardware Design

## Hardware Block Diagram

<p align="center">
<img src="images/hardware/hardware_block_diagram.png" width="90%">
</p>

---

## Circuit Diagram

<p align="center">
<img src="images/hardware/circuit_diagram.png" width="90%">
</p>

---

## Sensor Layout

<p align="center">
<img src="images/hardware/sensor_layout.png" width="90%">
</p>

---

## Pin Connection Diagram

<p align="center">
<img src="images/hardware/pin_connection_diagram.png" width="90%">
</p>

---

## Component Overview

<p align="center">
<img src="images/hardware/component_overview.png" width="90%">
</p>

---

# Concept Demonstrations

> The following images are **concept illustrations** created for documentation purposes and do not represent actual hardware runtime captures.

## Dashboard Concept

<p align="center">
<img src="images/results/concept_dashboard.png" width="90%">
</p>

---

## LCD Output Concept

<p align="center">
<img src="images/results/concept_lcd_output.png" width="70%">
</p>

---

## Parking Assistance Concept

<p align="center">
<img src="images/results/concept_parking_assistance.png" width="90%">
</p>

---

## Obstacle Detection Concept

<p align="center">
<img src="images/results/concept_obstacle_detection.png" width="90%">
</p>

---

# Repository Structure

```text
Smart-Parking-Assistance-System
│
├── .github
├── datasheets
├── docs
├── firmware
├── hardware
├── images
├── simulations
│
├── README.md
├── LICENSE
├── CHANGELOG.md
├── CONTRIBUTING.md
├── CODE_OF_CONDUCT.md
├── .gitignore
└── CITATION.cff
```

---

# Installation

```bash
git clone https://github.com/yourusername/Smart-Parking-Assistance-System.git
```

---

# Build Instructions

1. Open STM32CubeIDE.
2. Import the firmware project.
3. Build the project.
4. Flash the STM32 board.
5. Connect sensors.
6. Test parking assistance.

---

# Testing Procedure

- Verify sensor readings.
- Check LCD updates.
- Validate buzzer alerts.
- Verify LED indications.
- Test obstacle detection.
- Evaluate parking guidance.

---

# Expected Results

- Accurate obstacle detection
- Reliable distance measurement
- Smooth parking guidance
- Responsive driver alerts
- Stable embedded firmware

---

# Advantages

- Low-cost implementation
- Modular firmware
- Expandable architecture
- Easy maintenance
- Educational value

---

# Limitations

- Indoor testing only
- Ultrasonic sensor limitations
- Environmental sensitivity
- Basic parking logic

---

# Future Improvements

- CAN Bus integration
- Camera support
- Automatic steering
- Bluetooth connectivity
- Mobile application
- FreeRTOS migration
- Sensor fusion
- Machine learning-based parking prediction

---

# Skills Demonstrated

- Embedded C
- STM32 HAL
- GPIO
- Timers
- Interrupts
- Ultrasonic interfacing
- LCD interfacing
- Embedded firmware architecture
- Automotive electronics
- Driver assistance systems
- Technical documentation
- Git & GitHub

---

# Author

**Shashi Kiran**

Embedded Systems Engineer

Automotive Electronics Engineer

---

# License

This project is licensed under the MIT License.

See the **LICENSE** file for more information.

---

⭐ If you found this project useful, consider giving it a star.
