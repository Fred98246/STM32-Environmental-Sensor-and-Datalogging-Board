# Custom STM32 Environmental Sensor & Datalogging Board

## Project Overview
This repository contains the complete design files and source code for a custom, battery-powered STM32 microcontroller board designed for environmental monitoring and data logging. Measuring at a compact 40 x 58 mm, the board features integrated temperature, humidity, and ambient light sensors, alongside an SD card slot for local data storage. It includes a complete Li-Ion power management system with USB-C charging, load sharing, and battery protection. 

The user interface is driven by a rotary encoder and an external SPI display, making it suitable for standalone automated environments like humidity control or ambient monitoring.

## Repository Structure
*   **Hardware:** Contains the complete KiCad E.D.A. project files, including the schematic and the 4-layer PCB layout.
*   **Manufacturing:** Includes the generated Gerber files, Drill files, and sheets with the Bill of Materials (BOM) required for PCB fabrication and assembly.
*   **Firmware:** Contains the `STM32CubeMX` (`.ioc`) configuration file for peripheral initialization and the C/C++ source code.

## Hardware Implementation Details

### Core Processing
*   **Microcontroller:** The board is powered by the STM32F103C8T6 ARM Cortex-M3 MCU.
*   **Clocking:** A 16MHz crystal oscillator (LFXTAL085871) is implemented to provide the precise clocking required for stable USB communication.
*   **Debugging/Programming:** A low-profile Tag-Connect TC2030 footprint is used for SWD programming, saving critical board space.

### Power Management
The power architecture is designed for safe, continuous operation whether plugged in or running on battery:
*   **Battery Charging:** Handled by the MCP73831/2 IC, drawing 5V from the USB-C port to charge a connected Li-Ion cell[cite: 1].
*   **Battery Protection:** An AP9101C IC, paired with dual ZXMN3B14F N-Channel MOSFETs, protects the battery from overcharge, overdischarge, and overcurrent scenarios.
*   **Load Sharing:** An AO3401A P-Channel MOSFET and an SS14 Schottky diode create a seamless load-sharing circuit[cite: 1]. This ensures the system runs directly off USB power when plugged in, allowing the battery to charge optimally without system load interference.
*   **Regulation:** An AP2112K-3.3TRG1 LDO provides a stable 3.3V supply for the MCU and logic.

### Sensors & Interfaces
*   **Environmental Sensing:** Utilizes the SHT3x-DIS for high-precision temperature and humidity tracking via I2C.
*   **Light Sensing:** Incorporates the OPT3001DNPRQ1 ambient light sensor, also operating on the I2C bus.
*   **Storage:** A Molex 47219-2001 MicroSD card receptacle is routed via high-speed SPI for data logging.
*   **Connectivity:** USB-C port configured for USB 2.0 data and power. D3V3XA4B10LP TVS diode arrays are placed on both the USB data lines and the SD card lines to protect against electrostatic discharge (ESD).
*   **User Input:** Hardware debounced rotary encoder footprint with integrated push-button functionality.

## Design Challenges & Signal Integrity
Designing this board within a 40x58mm footprint required careful attention to PCB stackup and signal routing:

*   **4-Layer Stackup:** To accommodate the component density and maintain strict signal integrity, a 4-layer PCB was utilized (Top Signal, Ground Plane, Power Plane, Bottom Signal). This continuous internal ground plane is critical for providing short return paths for high-frequency signals and minimizing electromagnetic interference (EMI).
*   **USB Differential Routing:** The USB D+ and D- lines require 90-ohm differential impedance matching. The 4-layer stackup allowed for calculating precise trace widths and spacing over the internal ground plane to hit this impedance target, ensuring reliable USB enumeration and data transfer.
*   **MicroSD SPI Speeds:** Routing the SPI bus for the SD card required careful trace length matching and spacing. Ensuring the clock (SCK) and data lines (MISO/MOSI) run over an unbroken ground plane prevents cross-talk and signal reflection at higher frequencies.
*   **Mixed-Signal Placement:** Analog sensor lines (I2C for SHT3x and OPT3001) are physically separated from high-speed digital traces (USB, SPI, and the 16MHz crystal) to prevent digital switching noise from coupling into the sensitive environmental readings.
