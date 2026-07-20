# Custom STM32 Environmental Sensor & Datalogging Board

## Project Overview
This repository contains the complete design files and source code for a custom, battery-powered STM32 microcontroller board designed for environmental monitoring and data logging. Measuring at a compact 40 x 58 mm, the board features integrated temperature, humidity, and ambient light sensors, alongside an SD card slot for local data storage. It includes a complete Li-Ion power management system with USB-C charging, load sharing, and battery protection. 

The user interface is driven by a rotary encoder and an external SPI display, making it suitable for standalone automated environments like humidity control or ambient monitoring.

## Repository Structure
*   **Hardware:** Contains the complete KiCad E.D.A. project files, including the schematic and the 4-layer PCB layout.
*   **Manufacturing:** Includes the generated Gerber files, Drill files, and sheets with the Bill of Materials (BOM) required for PCB fabrication and assembly.
*   **Firmware:** Contains the `STM32CubeMX` (`.ioc`) configuration file for peripheral initialization and the C/C++ code with the needed drivers.
*   **docs**: Contains component datasheets and a detailed Bill of Materials (BOM) spreadsheet

## Hardware Implementation Details

### Core Processing
*   **Microcontroller:** The board is powered by the STM32F103C8T6 ARM Cortex-M3 MCU.
*   **Clocking:** A 16MHz crystal oscillator (LFXTAL085871) is implemented to provide the precise clocking required for stable USB communication. 
*   **Debugging/Programming:** Connector for SWD programming, via ST-Link V2.

### Power Management
The power architecture is designed for safe, continuous operation whether plugged in or running on battery:
*   **Battery Charging:** Handled by the MCP73831/2 IC, drawing 5V from the USB-C port to charge a connected Li-Ion cell.
*   **Battery Protection:** An AP9101C IC, paired with dual ZXMN3B14F N-Channel MOSFETs, protects the battery from overcharge, overdischarge, and overcurrent scenarios.
*   **Load Sharing:** An AO3401A P-Channel MOSFET and an SS14 Schottky diode create a seamless load-sharing circuit. This ensures the system runs directly off USB power when plugged in, allowing the battery to charge optimally without system load interference.
*   **Regulation:** An AP2112K-3.3TRG1 LDO provides a stable 3.3V supply for the MCU and logic.

### Sensors & Interfaces
*   **Environmental Sensing:** Utilizes the SHT3x-DIS for high-precision temperature and humidity tracking via I2C.
*   **Light Sensing:** Incorporates the OPT3001DNPRQ1 ambient light sensor, also operating on the I2C bus.
*   **Storage:** A Molex 47219-2001 MicroSD card receptacle is routed via high-speed SPI for data logging in txt file.
*   **Connectivity:** USB-C port configured for USB 2.0 data and power. D3V3XA4B10LP TVS diode arrays are placed on both the USB data lines and the SD card lines to protect against electrostatic discharge (ESD).
*   USB-CDC Virtual COM Port Interface: Integrated full-speed USB communication between the STM32 and host PC, enabling real-time serial telemetry and diagnostic logging for hardware bring-up and physical fault isolation.
*   **User Input:** Hardware debounced rotary encoder footprint with integrated push-button functionality.

## Design Challenges & Signal Integrity
Designing this board within a 40x58mm footprint required careful attention to PCB stackup and signal routing:

*   **Component Footprints & Tolerances:** Extensive review of manufacturer datasheets was required to ensure precise footprint creation and component placement within the restricted 40x58mm board area.
*   **4-Layer Stackup:** To accommodate the component density and maintain strict signal integrity, a 4-layer PCB was utilized (Top Signal, Ground Plane, Power Plane, Bottom Signal). This continuous internal ground plane is critical for providing short return paths for high-frequency signals and minimizing electromagnetic interference (EMI). 
*   **USB Differential Routing:** The USB D+ and D- lines require 90-ohm differential impedance matching. The 4-layer stackup allowed for calculating precise trace widths and spacing over the internal ground plane to hit this impedance target, ensuring reliable USB enumeration and data transfer.
*   **MicroSD SPI Speeds:** Because SD card SPI operates at moderate speeds, it does not require strict impedance matching or length tuning. The 4-layer stackup provided the flexibility to route these signals easily through vias while maintaining a solid ground reference to minimize general EMI.
*   **Mixed-Signal Placement:** Analog sensor lines (I2C for SHT3x and OPT3001) are physically separated from high-speed digital traces (USB, SPI, and the 16MHz crystal) to prevent digital switching noise from coupling into the sensitive environmental readings.
*   **Firmware Architecture:** The C/C++ codebase leverages STM32 HAL libraries, structured with modular driver functions and a streamlined main loop to ensure non-blocking sensor reads and efficient peripheral management.

## Operational Flow & User Interface: 
The firmware is structured around an event-driven state machine that handles user interaction, data logging, and strict power efficiency:

*   **Startup & Menu Navigation:** The system is awakened by a physical press of the rotary encoder. The TFT display initializes and presents an interactive menu where the user can select to view Temperature, Humidity, or Ambient Light.
*   **Active Monitoring:** Upon selection, the screen continuously updates with real-time readings from the designated I2C sensor.
*   **Concurrent Datalogging & Telemetry:** While the system is awake, it automatically mounts the FATFS file system and appends the sensor readings to a `.txt` file on the MicroSD card at strict 10-second intervals. Simultaneously, this data is continuously broadcast over the USB-CDC COM port to a connected PC for live monitoring.
*   **Low-Power Sleep Mode:** To conserve battery, if the system detects no rotary encoder rotation or button clicks for 2 minutes, it automatically gracefully closes the SD card file, powers down the display, and puts the STM32 into a low-power `STOP` state. A subsequent button press triggers an EXTI hardware interrupt, waking the system back to full operation instantly.

## 🚀 Future Scope: Closed-Loop Automation
While currently operating as a standalone datalogger, this board was engineered to serve as the precision sensor node for future automated control systems. By linking the onboard environmental telemetry to external physical actuators (such as fans, humidifiers, or motorized blinds), the hardware will transition into a dynamic closed-loop control environment. 

The high-resolution empirical data currently gathered via the SD card and USB interface provides the foundation for mathematical system identification. This allows the physical environment's behavior to be accurately modeled in software like MATLAB, enabling the future calculation of transfer functions and the implementation of tuned PID or automated sequential control logic directly onto the STM32.

*   **Schematic**:

<img width="1149" height="789" alt="image" src="https://github.com/user-attachments/assets/9d448743-a0b3-4858-af6b-89bec7121a91" />
<img width="1148" height="787" alt="image" src="https://github.com/user-attachments/assets/d7fae4d0-ccda-4465-9ec7-5a6cb2bcd7d1" />

* **PCB**:

<img width="624" height="864" alt="image" src="https://github.com/user-attachments/assets/58268a42-8cf1-4f6b-a486-341bee1de936" />

* **3D View**:

<img width="536" height="730" alt="image" src="https://github.com/user-attachments/assets/e380b3b4-6210-40be-927c-a049ae12406d" />







