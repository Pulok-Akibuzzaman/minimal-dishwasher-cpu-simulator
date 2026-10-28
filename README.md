# AquaCore-8: Minimal CPU Architecture for Automated Dishwasher Control
### CSE360 – Computer Architecture Mini-Project | East West University (EWU)

[![GitHub Repository](https://img.shields.io/badge/GitHub-aquacore8--cpu--simulator-blue?logo=github)](https://github.com/Pulok-Akibuzzaman/aquacore8-cpu-simulator)
[![Language](https://img.shields.io/badge/Language-Pure%20C%20(C99)-00599C?logo=c)](src/)
[![Tests](https://img.shields.io/badge/Verification-37%2F37%20Passed%20(100%25)-brightgreen)](src/tests.c)
[![Architecture](https://img.shields.io/badge/Architecture-8--Bit%20Harvard%20RISC%20ASIP-orange)](docs/ISA_Specification.md)

---

## 👥 Project Team & Submission Details

* **Institution:** East West University (EWU), Dhaka, Bangladesh
* **Department:** Department of Computer Science and Engineering
* **Course:** CSE360 – Computer Architecture
* **Project Title:** *Suggest and design a minimal CPU architecture, including ISA, datapath, and control unit, for automating dishwasher control functions with sensor inputs, and verify it with a simulator.*
* **Repository:** [https://github.com/Pulok-Akibuzzaman/aquacore8-cpu-simulator](https://github.com/Pulok-Akibuzzaman/aquacore8-cpu-simulator)

### Team Members:
| Name | Student ID | University Email | Role / Contribution |
|:---|:---|:---|:---|
| **Nusrat Rahman Aurna** | `2023-3-60-057` | `2023-3-60-057@std.ewubd.edu` | Architecture Design, Control Unit Logic & Verilog RTL |
| **Pulok Akibuzzaman** | `2023-3-60-051` | `2023-3-60-051@std.ewubd.edu` | Pure C Simulation Engine, Cyber-Physical Plant & Test Suite |
| **Md. Jakaria Masud** | `2023-3-60-386` | `2023-3-60-386@std.ewubd.edu` | Dishwasher Assembly Firmware, Physical Models & Documentation |

---

## 💡 The Plain-English Explanation: What is Actually Happening Here?

If you are new to computer architecture or wondering what this project actually does, here is the simplest breakdown:

### 1. The Purpose (Why did we build this?)
Inside every modern household appliance (like a dishwasher, microwave, or washing machine), there is a tiny computer chip that controls the machine.
* In standard projects, people just buy an existing chip from Intel, Microchip (PIC), or Arduino.
* For **CSE360 Computer Architecture**, we did something much deeper: **we designed our very own custom 8-bit computer processor (CPU) from scratch on paper, and built a simulator in pure C to prove it works!**
* We named our custom processor **AquaCore-8**. Its purpose in life is to be the intelligent brain of a dishwasher: read buttons/sensors, calculate temperatures, and turn on water taps, spray motors, heaters, and fans.

### 2. How the Simulator Works (The Two Halves)
Because this processor runs on your PC (`aquacore8_sim.exe`), the simulator consists of **two halves working together in a loop**:
1. **The Virtual CPU (The Brain):**
   * Reads instructions from its program memory one by one.
   * Has 4 little "pockets" (Registers $R_0, R_1, R_2, R_3$) to hold numbers.
   * Reads sensors using the `IN` instruction.
   * Tells actuators to turn ON or OFF using the `OUT` instruction.
2. **The Physical Plant (The Dishwasher Machine):**
   * Simulates the real physical tub, heating element, and water pipes.
   * When the CPU outputs `"Open Water Valve"`, the physical plant simulator increases the water level (+2 Liters per tick).
   * When the CPU outputs `"Turn ON Heater"`, the physical plant simulator increases the water temperature (+0.1°C per tick).
   * When the CPU asks `"What is the water temperature?"`, the physical plant feeds the current temperature back into the CPU!

### 3. Is It a Real-Time Simulation?
**Yes, it can run in two ways:**
* **Interactive Real-Time Mode (`run.bat cli`):** Runs interactively. You can watch the real-time ASCII dashboard update on your screen, step cycle-by-cycle, or run with delays. You can type commands like `start`, `mode 1` (Eco), `door open`, or `temp 78` to inject faults and watch the CPU react in real time.
* **Fast Batch Mode (`run.bat batch`):** Runs the full 1,254-cycle wash cycle in a fraction of a second (about 0.05 seconds) to automatically prove that the whole wash sequence completes from start to finish with 0 errors.

### 4. What Are the Changes (Inputs & State Changes)?
* **Sensor Inputs (What changes in the environment):**
  * **Door Sensor:** Starts CLOSED; if opened by a curious user mid-wash, the CPU instantly freezes all pumps within 2 microseconds!
  * **Water Level:** Starts at 0 (empty), fills up to 80 (full tub), and drains to 0 between wash phases.
  * **Water Temperature:** Starts cold at 20°C (ambient tap water), heats up to 55°C (or 45°C in Eco mode), and cools down when drained.
  * **Water Turbidity (Cloudiness):** Starts clean (0), gets dirty (up to 70–90) as food and sauce dissolve off the plates, and drops back to 0 as dirty water drains out.
  * **Plate Cleanliness:** Plates start at 80% dirty and end at < 5% dirty (sparkling clean!).
* **CPU Internal State (What changes inside the brain):**
  * Program Counter ($PC$) moves from `0x00` through `0x95` as it executes instructions.
  * Registers ($R_0..R_3$) hold temporary numbers (like the target temperature = 55).
  * Flags ($Z, C, N, V$) update based on comparisons (e.g. Zero flag sets when a timer reaches 0).

### 5. What Are the Outputs (What actually happens)?
* **Physical Actuators Controlled:**
  * **Water Inlet Valve:** Opens to let fresh tap water in.
  * **Wash Spray Pump:** Blasts soapy water onto the plates.
  * **1800W Heater:** Warms the water to dissolve grease.
  * **Detergent Solenoid:** Clicks open to release the soap tablet (*CLICK!*).
  * **Drain Pump:** Pumps dirty water down the kitchen drain pipe.
  * **Drying Fan:** Blows warm air to dry the plates like a hairdryer.
  * **Buzzer:** Beeps happily (*BEEP BEEP BEEP!*) when dinner plates are ready!
* **Front-Panel Status LEDs:**
  * `[STANDBY]` ➔ `[PRE-WASH]` ➔ `[MAIN WASH]` ➔ `[RINSE]` ➔ `[DRYING]` ➔ `[CYCLE COMPLETE!]`.
  * `[DOOR AJAR WARN]`: Flashes if someone opens the door mid-wash.
  * `[FAULT RED LED]`: Illuminates if water exceeds safe thermal limits (>75°C).

---

## 🌟 Executive Summary

**AquaCore-8** is an application-specific 8-bit Harvard RISC-style microprocessor engineered from the ground up to automate complex household appliances. Configured here for an intelligent dishwasher, AquaCore-8 interfaces with real-time sensors (water temperature, fluid level, optical turbidity, door safety switch, puddle leak sensor) to dynamically modulate high-power actuators (inlet valves, wash pumps, 1800W heating elements, detergent/rinse-aid solenoids, drying blowers, and alarm buzzers).

### Core Highlights:
* **Implemented Exclusively in Pure C (C99):** Clean, minimal, zero third-party dependencies, bare-metal hardware mapping (`uint8_t`, `uint16_t`), and nanosecond-fast execution.
* **Custom 16-Bit Instruction Set (ISA):** 16 primary opcodes supporting R-Type arithmetic/logic, I-Type immediates, memory Load/Store, Port I/O, Subroutine Call/Ret, and conditional branches.
* **Single-Cycle Datapath:** Hardwired control unit ($CPI = 1.0$), 4 general-purpose registers (`R0`–`R3`), an 8-bit ALU with Status Flags ($Z, C, N, V$), and a 16-level hardware return stack.
* **Cyber-Physical Plant Simulation:** First-principles fluid dynamics, thermodynamic heating ($1800\text{ W}$ element), and Beer-Lambert optical turbidity equations.
* **Intelligent Assembly Firmware:** 216-instruction program featuring adaptive wash cycles that detect water soiling via optical sensors and dynamically trigger secondary rinse phases.
* **37 Automated Tests:** 100% pass rate across 20 CPU unit tests and 17 full-cycle cyber-physical integration & fault injection scenarios.
* **Printable A4 Soft-Binding Report:** Compliant with East West University guidelines, featuring 38 mathematically rigorous equations rendered in LaTeX/MathJax.

---

## 🚀 Quick Start Guide

### Option 1: Master Interactive Menu (Recommended)
Simply double-click [`run.bat`](run.bat) in Windows Explorer or execute in terminal:
```cmd
run.bat
```
This launches an interactive menu where you can run tests, start the terminal monitor, run full wash cycle traces, or open the HTML documentation with a single keypress!

---

### Option 2: Automated Build & Verification (One Click)
To compile the C source files and run the full 37-test suite:
```cmd
build.bat
```
* Compiles `src/*.c` using GCC into `aquacore8_sim.exe`.
* Executes all 20 CPU architecture tests and all 17 plant integration tests.
* Confirms 100% test pass rate in less than 1 second!

---

### Option 3: Command-Line Flags
You can run specific simulator modes directly from the terminal:
```cmd
run.bat test     # Execute all 37 unit and integration tests
run.bat batch    # Run end-to-end wash cycle batch trace (Pre-Wash -> Main -> Rinse -> Dry)
run.bat cli      # Launch interactive real-time cycle visualizer
```

---

## 🎓 Viva & Exam Defense Quick Reference

If your faculty asks you to explain the design during oral examination or project defense:

| Question | Your 1-Sentence Answer |
|:---|:---|
| **1. "What architecture does your CPU use?"** | *"It is an 8-bit Harvard RISC processor with 16-bit fixed-length instructions, 4 general-purpose registers ($R_0..R_3$), and single-cycle execution ($CPI = 1.0$)."* |
| **2. "Why Harvard instead of Von Neumann?"** | *"Harvard separates ROM and RAM so the CPU can fetch an instruction from ROM and access data in RAM in the same clock cycle without memory bus conflict."* |
| **3. "How does the CPU talk to the dishwasher sensors?"** | *"Through Port-Mapped I/O: the `IN` instruction reads sensor ports (0x00 for door/mode, 0x01 for water level, 0x02 for temperature), and the `OUT` instruction writes to actuator ports (0x10 for pumps and heaters)."* |
| **4. "What makes the dishwasher controller 'smart'?"** | *"An optical turbidity sensor reads water cloudiness after the main wash. If dirty sauce/curry remains in the water, our firmware dynamically executes an extra rinse cycle."* |
| **5. "What happens if a child opens the door mid-cycle?"** | *"Our safety subroutine polls the door sensor bit. In less than 2 microseconds, it cuts all power to sprayers and heaters, lights a warning LED, and resumes seamlessly when the door is re-closed."* |
| **6. "How did you verify the CPU works?"** | *"We built an automated test harness in C with 37 tests (20 unit tests for all ALU opcodes and flags, plus 17 end-to-end wash cycles and fault injections), achieving a 100% pass rate."* |

---

## 🧪 Verification & Test Results

The architecture and physical plant are verified against **37 automated regression tests** with a **100% pass rate**:

```text
===============================================================================
               AquaCore-8 Verification & Test Suite Summary
===============================================================================
[PASS] Unit Test 01: ADD R2 = 10 + 25                       (Got: 0x23)
[PASS] Unit Test 02: SUB R3 = 25 - 10                       (Got: 0x0F)
[PASS] Unit Test 03: Zero Flag state inspection             (Z = False)
[PASS] Unit Test 04: Negative Flag state inspection         (N = False)
[PASS] Unit Test 05: 8-Bit Overflow Wrap (0xFF + 1 = 0x00)  (Got: 0x00)
[PASS] Unit Test 06: Zero Flag assertion on zero result     (Z = True)
[PASS] Unit Test 07: Carry Flag assertion on 8-bit carry    (C = True)
[PASS] Unit Test 08: Bitwise AND Logic (0xAC & 0xF0)        (Got: 0xA0)
[PASS] Unit Test 09: Bitwise OR Logic  (0xAC | 0xF0)        (Got: 0xFC)
[PASS] Unit Test 10: Logical Shift Left (0x04 << 1)         (Got: 0x08)
[PASS] Unit Test 11: Logical Shift Right (0x04 >> 1)        (Got: 0x02)
[PASS] Unit Test 12: Immediate Arithmetic (ADDI & SUBI)     (Got: 0x0C)
[PASS] Unit Test 13: RAM Store & Load verification          (Got: 0x42)
[PASS] Unit Test 14: Direct RAM Cell Integrity [0x50]       (Got: 0x42)
[PASS] Unit Test 15: Sensor Input Port Read (IN)            (Got: 0x3C)
[PASS] Unit Test 16: Actuator Output Port Write (OUT)       (Got: 0x3C)
[PASS] Unit Test 17: Branch Loop Termination at Zero        (Got: 0x00)
[PASS] Unit Test 18: Branch Exit PC Alignment               (PC  = 0x04)
[PASS] Unit Test 19: Hardware Subroutine CALL & RET         (Got: 0x20)
[PASS] Unit Test 20: Stack Pointer Return Cleanliness       (SP  = 0x00)
-------------------------------------------------------------------------------
Unit Tests Passed: 20 / 20  [100% PERFECT]
-------------------------------------------------------------------------------
[PASS] Integration 01: Complete Normal Wash Cycle Execution
[PASS] Integration 02: Phase 1 Pre-Wash Inflow & Drainage
[PASS] Integration 03: Phase 2 Main Wash Thermal Ramp (>= 54°C)
[PASS] Integration 04: Detergent Solenoid Release
[PASS] Integration 05: Phase 3 Adaptive Multi-Rinse Cycle
[PASS] Integration 06: Phase 4 Heated Air Drying
[PASS] Integration 07: Phase 5 Cycle Complete Green LED
[PASS] Integration 08: Completion Notification Buzzer
[PASS] Integration 09: Final Plate Cleanliness (< 10% Residual Soil)
[PASS] Integration 10: Eco Mode Wash (Target 45°C Dynamic Regulation)
[PASS] Integration 11: Eco Mode Successful Completion
[PASS] Integration 12: Mid-Wash Door Opening Emergency Interlock
[PASS] Integration 13: Zero-Latency Actuator Cutoff on Open Door
[PASS] Integration 14: Front-Panel Flashing Door Warning LED
[PASS] Integration 15: Seamless Resumption after Door Closure
[PASS] Integration 16: Overheat Emergency Trip (> 75°C Water Temp)
[PASS] Integration 17: Red Fault LED & Error Code 0xAA Latched
-------------------------------------------------------------------------------
Integration Tests Passed: 17 / 17  [100% PERFECT]
===============================================================================
TOTAL TEST PASS RATE: 37 / 37 (100% SUCCESS)
===============================================================================
```

---

## 📂 Project Directory Structure

```text
Mini-Project/
├── run.bat                           ; Master interactive menu & mode dispatcher
├── build.bat                         ; One-click C compiler & 37-test runner
├── aquacore8_sim.exe                 ; Compiled high-performance simulator executable
├── README.md                         ; Master project README & documentation index
├── Miniproject description.pdf       ; Course assignment requirements & guidelines
│
├── src/                              ; Pure C Microprocessor & Simulator Engine
│   ├── aquacore8.h                   ; CPU core, registers, I/O ports & physical plant header
│   ├── aquacore8.c                   ; Execution loop, ALU, disassembler & physical plant
│   ├── tests.c                       ; 37 automated unit & full integration test cases
│   └── main.c                        ; Interactive CLI visualizer & batch trace runner
│
├── programs/                         ; Firmware & Machine Code
│   ├── dishwasher_controller.asm     ; Complete assembly firmware (216 instructions)
│   └── dishwasher_controller.hex     ; Compiled 16-bit hexadecimal machine code
│
├── verilog/                          ; Synthesizable Hardware RTL Design
│   ├── cpu_top.v                     ; Top-level synthesizable processor
│   ├── alu.v                         ; 8-bit Arithmetic Logic Unit with flags
│   ├── control_unit.v                ; Hardwired single-cycle instruction decoder
│   ├── datapath.v                    ; PC, ALU, Register File, Stack, and MUXes
│   ├── register_file.v               ; 4 x 8-bit register bank (R0..R3)
│   ├── instruction_memory.v          ; 256 x 16-bit ROM module
│   ├── data_memory.v                 ; 256 x 8-bit RAM module
│   ├── io_controller.v               ; Port-mapped sensor & actuator registers
│   └── dishwasher_tb.v               ; Hardware testbench & timing stimulus
│
└── docs/                             ; Formal Academic Documentation
    ├── Project_Report_CSE360.docx    ; Official Word document formatted for soft binding
    ├── Project_Report_CSE360.html    ; Printable A4 formatted report (Ctrl+P -> PDF)
    ├── Project_Report_CSE360.md      ; Full 9-section report in Markdown
    ├── Explain_Like_Im_5.html        ; Printable web version of the ELI5 guide
    ├── Explain_Like_Im_5.md          ; "Explain Like I'm 5" intuitive concept breakdown
    ├── ISA_Specification.md          ; Complete opcode and instruction set reference
    └── Architecture_Diagrams.md      ; Mermaid schematics & datapath diagrams
```

---

## 📖 Key Documentation Links

1. **[Academic Project Report (Word Document)](docs/Project_Report_CSE360.docx)**
   * Clean, editable Microsoft Word `.docx` file following the exact 9-section manual.
   * Features seminar/dissertation cover page, student names, IDs, emails, tables, equations, and references.
2. **[Academic Project Report (HTML Printable)](docs/Project_Report_CSE360.html) | [(Markdown)](docs/Project_Report_CSE360.md)**
   * Formatted strictly to the 9-section CSE360 syllabus outline with MathJax LaTeX formulas.
   * Ready for direct A4 soft-binding print (`Ctrl + P` in any browser).
3. **[Explain Like I'm 5 (ELI5) Guide (HTML)](docs/Explain_Like_Im_5.html) | [(Markdown)](docs/Explain_Like_Im_5.md)**
   * A friendly, engaging guide explaining every component using simple everyday metaphors (the cookbook, the whiteboard, the little pockets, the turbidity goggles, and kid safety).
4. **[Instruction Set Architecture (ISA) Specification (Markdown)](docs/ISA_Specification.md)**
   * Complete bitfield encodings, opcodes, register conventions, and cycle timings.
5. **[Micro-Architecture & Datapath Diagrams (Markdown)](docs/Architecture_Diagrams.md)**
   * Hardware block diagrams and finite-state machine (FSM) representations.

---

## 🖨️ Printing the Report for Soft Binding Submission

You can print the final report using either Microsoft Word or your web browser:
* **Option A (Microsoft Word - Recommended):** Open [`docs/Project_Report_CSE360.docx`](docs/Project_Report_CSE360.docx), review the formatting, and print directly to A4 paper.
* **Option B (Web Browser):** Open [`docs/Project_Report_CSE360.html`](docs/Project_Report_CSE360.html) in Chrome/Edge, press **`Ctrl + P`**, set paper to A4, enable background graphics, and print/save to PDF.

---

*Developed with pride for CSE360 Computer Architecture, Department of Computer Science and Engineering, East West University.*
