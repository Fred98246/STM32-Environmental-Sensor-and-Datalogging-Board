# STM-32--Sensor-Board-for-plant-monitoring
# Description:
An open-source STM32F103C8T6 development board designed for plant and environmental monitoring, featuring integrated Li-Ion battery management, USB-C, MicroSD data logging, and onboard I2C sensors for air quality, temperature, humidity, and ambient light.

# STM32 Plant & Environmental Monitor

## Overview
This repository contains the KiCad hardware design files for a custom environmental and plant monitoring board. The system is built around the STM32F103C8T6 ARM Cortex-M3 microcontroller and is fully equipped for standalone data logging. It features a complete onboard power management system with seamless load sharing, allowing the board to run on a Li-Ion battery or via USB-C. 

## Key Features

### Core & Interface
*   **Microcontroller:** STM 32 Microconroler (STM32F103C8T6)
*   **Programming/Debugging:** Manual BOOT0 and NRST switches integrated for easy flashing.
*   **Storage:** MicroSD Card interface (47219-2001) for offline environmental data logging.
*   **Display:** SPI/I2C breakout header for attaching external OLED or TFT screens.

### Environmental Sensors
The board utilizes a dedicated I2C bus to monitor multiple environmental parameters:
*   **Temp/Humidity Sensor:** SHT3x-DIS.
*   **Air Quality Sensor:** ENS160-BGLM (measures eCO2 and TVOCs).
*   **Light sensor:** OPT3001DNPRQ1 (for precise ambient light measurement).

### Power Management & Battery Protection
*   **USB & ESD:** USB-C receptacle protected by a TVS DIODE ARRAY for USB (D3V3XA4B10LP).
*   **Voltage Regulation:** Onboard LDO Power 3.3V (AP2112K-3.3TRG1).
*   **Battery Charging:** Integrated Li-Ion Battery Charger (MCP73831/2).
*   **Battery Protection:** Dedicated IC (AP9101C) paired with Dual N-Channel Mosfets (EFC2J013NUZ) to prevent over-charge/discharge.
*   **Load Sharing:** A power-path circuit utilizing a P-Channel MOSFET (AO3401A) and a Schottky diode (SS14) allows the system to switch seamlessly between USB power and battery power without dropping voltage.

## Repository Structure
*   `/hardware` - Contains the KiCad 9.0 project files (`.pro`, `.sch`, `.kicad_pcb`).
*   `/manufacturing` - Contains the generated Gerber files, Bill of Materials (BOM), and Pick & Place/Centroid files for PCBA assembly.
*   `/docs` - Datasheets for the main components and PDF exports of the schematic.
*   `/firmware` - (Optional) Example code to initialize the sensors and read data.

## Hardware Manufacturing 
All necessary files to manufacture this board are located in the `/manufacturing` directory. The total BOM cost for the primary integrated circuits and connectors is approximately 19.97 € per board Ensure that your PCB manufacturer follows standard 2-layer or 4-layer stackups as defined in the KiCad design rules.

## Author
# FDC
