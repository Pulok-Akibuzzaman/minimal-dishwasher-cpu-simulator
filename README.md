# Suggest and design a minimal CPU architecture, including ISA, datapath, and control unit, for automating dishwasher control functions with sensor inputs, and verify it with a simulator

### CSE360 – Computer Architecture Mini-Project | East West University (EWU)
**Semester:** Fall 2026 | **Section:** 01  
**Instructor:** Dr. Nawab Yousuf Ali, Professor, Department of Computer Science & Engineering  

**Submitted By:**
* **Nusrat Rahman Aurna** | Student ID: `2023-3-60-057` | `2023-3-60-057@std.ewubd.edu`
* **Pulok Akibuzzaman**   | Student ID: `2023-3-60-051` | `2023-3-60-051@std.ewubd.edu`

---

## 📋 Project Overview
This project presents the formal design, Instruction Set Architecture (ISA) specification, datapath organization, synchronous control unit logic, and cycle-accurate software simulation of **AquaCore-8**—a minimal 8-bit Application-Specific Instruction-set Processor (ASIP) engineered explicitly to automate domestic dishwasher operations.

AquaCore-8 achieves complete closed-loop appliance process automation (door safety interlocking, water filling, high-temperature wash agitation, hydraulic draining, and forced heated drying) using a lean 5-instruction architecture executed on a verified Fetch-Decode-Execute pipeline implemented in pure ANSI C in under 110 lines of self-documenting code.

---

## ⚙️ Architectural Specification

### 1. CPU Registers (Datapath)
* `program_counter` ($PC$): Holds the address of the next instruction word in ROM.
* `instruction_reg` ($IR$): Holds the currently fetched 8-bit instruction word `[Opcode | Operand]`.
* `data_register` ($R_0$ / Accumulator): Accumulator register holding sensor readings and comparison data.
* `zero_flag` ($ZF$): Single-bit condition flag set to `1` when a comparison between $R_0$ and the immediate operand matches.
* `is_processor_halted`: Set to `1` when the `HALT_CPU` instruction executes, halting the CPU clock generator.

### 2. Physical Sensors & Hardware Transducers (Input Ports)
* `SENSOR_DOOR` (Port 0): Safety door interlock switch (`1` = Closed, `0` = Open).
* `SENSOR_WATER_LEVEL` (Port 1): Hydrostatic pressure level sensor ($0\text{ to }50\text{ Liters}$).
* `SENSOR_WATER_TEMP` (Port 2): NTC thermistor temperature probe ($20^\circ\text{C to }55^\circ\text{C}$).

### 3. Actuator Output Subsystems (Output Ports)
* `WATER_INLET_VALVE` (Port 0): Solenoid intake valve ($+10\text{ L/cycle}$ fill rate).
* `HEATER_AND_PUMP` (Port 1): 2.2kW heating element and spray wash circulation pump ($+5^\circ\text{C/cycle}$ heat rate).
* `DRAIN_PUMP` (Port 2): Centrifugal drain evacuation pump ($-10\text{ L/cycle}$ drainage rate).
* `DRYING_FAN` (Port 3): Forced convection hot-air drying fan ($-5^\circ\text{C/cycle}$ cooling/drying rate).
* `FINISH_BUZZER` (Port 4): Piezoelectric completion audio beeper.

### 4. Minimal 5-Instruction ISA
| Opcode Mnemonic | Opcode ID | Operand Field | RTL Micro-Operation |
| :--- | :---: | :--- | :--- |
| `READ_SENSOR` | `0` | Sensor Port ID | $R_0 \leftarrow \text{Sensors}[\text{operand}]$ |
| `SET_DEVICE` | `1` | Device Port ID | $\text{active\_device} \leftarrow \text{operand}$ |
| `COMPARE_VALUE` | `2` | Immediate Constant | $ZF \leftarrow (R_0 == \text{operand})$ |
| `JUMP_IF_NOT_EQUAL` | `3` | ROM Target Address | $\text{if } (!ZF) \; PC \leftarrow \text{operand}$ |
| `HALT_CPU` | `4` | `0` (Unused) | $\text{is\_halted} \leftarrow 1$ |

---

## 🚀 How to Build and Run

### Prerequisites
* GCC (MinGW on Windows or GCC on Linux)

### Compilation
```bash
gcc -Wall -Wextra main.c -o aquacore8_sim.exe
```

### Execution
Run the executable directly:
```cmd
.\aquacore8_sim.exe
```
When launched, the simulator displays an interactive menu:
* **Option 1: Standard Automated Run** (Verified 45 Clock Cycles - Academic Baseline)
* **Option 2: Custom Environment & Sensor Input** (Configure initial water temperature, water volume, and optional step-by-step clock stepping)
* **Option 3: Safety Interlock Verification** (Simulates starting with door unlatched, demonstrating safe polling hold until user closes the door)

You can also pass the mode directly as a command-line argument for automated non-interactive runs:
```cmd
.\aquacore8_sim.exe 1   # Mode 1: Automated 45-cycle baseline
.\aquacore8_sim.exe 3   # Mode 3: Safety interlock demonstration
```
or via the provided batch scripts:
```cmd
build.bat
run.bat
```

---

## 📊 Verification Trace Output

```text
========================================================================================
        Minimal CPU Architecture Simulator for Automated Dishwasher Control            
 Course: CSE360 Computer Architecture | East West University                           
 Title : Suggest & design minimal CPU (ISA, datapath, control unit) for dishwasher     
========================================================================================
Clock Cycle | PC | Water Level | Temperature | Active Hardware Device
------------+----+-------------+-------------+------------------------------------------
      3     |  4 |     10L     |     20C     | Water Inlet Valve (Filling)
     13     |  8 |     50L     |     25C     | Heater & Spray Pump (Washing)
     23     | 12 |     40L     |     55C     | Drain Pump (Emptying)
     33     | 16 |      0L     |     50C     | Drying Fan (Drying)
     43     | 20 |      0L     |     30C     | Buzzer (Wash Completed)
     44     | 21 |      0L     |     30C     | Buzzer (Wash Completed)
========================================================================================
SUCCESS: Dishwasher automation verified! Completed cycle in 45 CPU clock cycles.
========================================================================================
```

---

## 📑 Project Deliverables
* [main.c](main.c): Pure ANSI C cycle-accurate CPU simulator in 110 lines.
* [Report_CSE360.md](Report_CSE360.md): Comprehensive academic project report in Markdown.
* [Report_CSE360.docx](Report_CSE360.docx): Formal academic report in standard formatted A4 Word document.
* [aquacore8_sim.exe](aquacore8_sim.exe): Precompiled executable binary.
