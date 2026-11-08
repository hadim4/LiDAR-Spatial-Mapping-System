# LiDAR Spatial Mapping System

**MSP432E401Y | Embedded C | Python | Open3D**

Designed and implemented a **360° LiDAR-based spatial mapping system** using an MSP432E401Y (ARM Cortex-M4F) microcontroller and VL53L1X time-of-flight distance sensor. The system performs a full rotation, collecting distance measurements at **11.25° intervals** to capture the surrounding environment. Developed embedded C firmware to communicate with the LiDAR sensor through **I²C** and transmit the collected measurements to a computer using **UART**. The sensor data is then processed in Python and visualized using **Open3D**, converting the distance measurements into 3D point clouds that represent the spatial layout of the scanned environment.

### Key Features

- **360° Spatial Scanning:** Developed a LiDAR-based scanning system that collects distance measurements at **11.25° intervals**, providing 32 measurement points per full rotation to map the surrounding environment.
- **Embedded Firmware:** Programmed the **MSP432E401Y (ARM Cortex-M4F)** in bare-metal C to initialize the LiDAR sensor, manage measurements, and coordinate the scanning process.
- **Sensor Communication:** Implemented **I²C communication** to configure the VL53L1X sensor and retrieve distance measurements, using **UART** to transmit the collected data to a host computer for processing.
- **3D Point-Cloud Visualization:** Developed Python scripts to process LiDAR measurements and generate **3D point-cloud visualizations using Open3D**, representing the spatial layout of the scanned environment.
- **Mechanical Design:** Designed and 3D-printed custom mounting components using **Autodesk Inventor** to support the LiDAR sensor and scanning hardware.

## Project Gallery

### 3D Spatial Mapping Results

<p align="center">
  <img src="https://github.com/user-attachments/assets/6d1736dd-6cc9-4493-bd0d-3d3bb7451862" width="400" alt="Room spatial mapping">
  <img src="https://github.com/user-attachments/assets/822c064d-c304-437e-9773-2f9a02cd20ed" width="400" alt="Hallway spatial mapping">
</p>

### Hardware Setup

<p align="center">
  <img src="https://github.com/user-attachments/assets/820b2dcb-b3bd-49ac-86be-0f8557115e16" width="450" alt="LiDAR spatial mapping hardware">
</p>

## Technologies Used

`Embedded C` · `MSP432E401Y` · `ARM Cortex-M4F` · `I²C` · `UART` · `Python` · `Open3D` · `AutoCAD Inventor`
