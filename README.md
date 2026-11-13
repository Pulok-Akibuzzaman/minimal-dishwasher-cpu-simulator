# AquaCore-8: Simple Dishwasher CPU Architecture Simulator
### CSE360 – Computer Architecture Mini-Project | East West University (EWU)

---

## 📋 Overview
A minimal, readable CPU simulator in pure C (**89 lines**) designed for **CSE360 Computer Architecture**. It models an 8-bit application-specific CPU controlling an automated dishwasher using sensors and actuators.

All variables and functions use clear, self-explanatory English words for easy presentation and viva explanation.

---

## ⚙️ Architecture Reference (Variables & Functions)

### 1. CPU Registers & Datapath
* `program_counter`: Address of the current instruction to fetch.
* `data_register`: Working register to store sensor values and intermediate data.
* `zero_flag`: Condition flag set to `1` when a comparison matches.
* `is_processor_halted`: Set to `1` when the `HALT_CPU` instruction executes.

### 2. Dishwasher Physical Sensors & Hardware
* `is_door_closed`: Safety sensor (`1` = closed, `0` = open).
* `current_water_level`: Tub water volume in Liters.
* `current_water_temperature`: Water temperature in °C.
* `active_hardware_device`: Currently running appliance component.

### 3. Core Functions
* `execute_cpu_cycle()`: Implements the **Fetch $\to$ Decode $\to$ Execute** control unit cycle.
* `simulate_dishwasher_hardware()`: Cyber-physical simulation updating water levels and temperatures.

---

## 🚀 How to Run
```cmd
build.bat
run.bat
```
or directly with GCC:
```bash
gcc main.c -o aquacore8_sim.exe
./aquacore8_sim.exe
```
