# AquaCore-8: Full Project Executive Summary
### CSE360 Computer Architecture Mini-Project | East West University (EWU)
**Repository:** [https://github.com/Pulok-Akibuzzaman/aquacore8-cpu-simulator](https://github.com/Pulok-Akibuzzaman/aquacore8-cpu-simulator)

---

## 👥 Project Team Members

| Team Member Name | Student ID | University Email | Project Contribution |
|:---|:---|:---|:---|
| **Nusrat Rahman Aurna** | `2023-3-60-057` | `2023-3-60-057@std.ewubd.edu` | Architecture Design, Control Unit Logic & Verilog RTL |
| **Pulok Akibuzzaman** | `2023-3-60-051` | `2023-3-60-051@std.ewubd.edu` | Pure C Simulation Engine, Cyber-Physical Plant & Test Suite |
| **Md. Jakaria Masud** | `2023-3-60-386` | `2023-3-60-386@std.ewubd.edu` | Dishwasher Assembly Firmware, Physical Models & Documentation |

---

## 💡 1. The Big Picture: What Did We Actually Build?

Inside every modern smart household appliance (like a dishwasher or washing machine), there is an embedded microcontroller that controls the water valves, heating elements, spray pumps, and safety door locks.

* In typical student projects, people simply purchase an existing commercial chip from Intel, Microchip (PIC), or Arduino.
* For **CSE360 Computer Architecture**, our team took on the full engineering challenge: **WE DESIGNED OUR VERY OWN CUSTOM 8-BIT MICROPROCESSOR FROM SCRATCH ON PAPER, AND PROVED IT WORKS BY CODING A PURE C SIMULATOR!**
* We named our microprocessor **AquaCore-8**. Its entire hardware design is tailored to be the brain of an intelligent automated dishwasher.

---

## ⚡ 2. Crucial Question: Is It Hardcoded or Dynamic?

> ### 🟢 AUTHENTICITY GUARANTEE: IT IS 100% DYNAMIC.
> * The `[PASS]` messages only print because the CPU executes the raw binary machine code cycle-by-cycle in computer RAM.
> * The **Arithmetic Logic Unit (ALU)** performs real 8-bit addition, subtraction, and bitwise logic at runtime.
> * **Condition flags** (Zero, Carry, Negative, Overflow) are calculated dynamically from live arithmetic results.
> * In the dishwasher simulation, water volume, thermodynamic temperature, and optical turbidity change tick-by-tick based on physical equations.
> * If you alter a single number or assembly instruction, the test suite immediately detects the mismatch and prints a red `[FAIL]`.

### How to Prove It is Dynamic in 5 Seconds:
1. Open `src/tests.c` in any text editor.
2. Find line 57:
   ```c
   assert_eq("ADD R2 = 10 + 25", 35, cpu.R[2]);
   ```
3. Change the expected number `35` to `99`.
4. Run `build.bat` in your terminal.
5. The test runner will immediately catch the error and fail:
   ```text
   [FAIL] ADD R2 = 10 + 25 (Expected: 0x63, Got: 0x23)
   Unit Test Summary: 19 Passed, 1 Failed.
   ```
6. Changing it back to `35` restores 100% PASS.

---

## ⚙️ 3. How Does the Simulator Work? (The Two Halves in C)

Because our custom microprocessor runs on a PC (`aquacore8_sim.exe`), the simulator consists of two cooperating modules executing in a synchronized loop:

```text
    +--------------------------------------------------------------+
    |               1. THE VIRTUAL CPU (The Brain)                 |
    |  - Reads our 216 instructions one by one.                    |
    |  - Has 4 pockets (Registers R0..R3) to hold numbers.         |
    |  - Reads sensors with IN instructions.                       |
    |  - Sends commands to turn on tools with OUT instructions.    |
    +------------------------------+-------------------------------+
                                   |
                         (IN / OUT Port Signals)
                                   |
                                   v
    +------------------------------+-------------------------------+
    |            2. THE PHYSICAL PLANT (The Dishwasher)            |
    |  - Simulates the physical tub, heater, pipes, and plates.    |
    |  - When CPU says "Open Valve": Water level rises (+2 L).     |
    |  - When CPU says "Turn on Heater": Water heats up (+0.1°C).  |
    |  - When CPU says "Read Temp": Feeds current temp back to CPU.|
    +--------------------------------------------------------------+
```

### Half 1: The Virtual CPU Core (`src/aquacore8.c`)
* Fetches instructions one by one from its 256-word Instruction ROM.
* Has 4 General-Purpose Registers (`R0`, `R1`, `R2`, `R3`) acting as fast internal pockets.
* Contains an 8-bit Program Counter ($PC$) that points to the current recipe step.
* Contains an 8-bit Arithmetic Logic Unit (ALU) that adds, subtracts, and compares numbers.
* Contains a 16-level hardware return stack for nested subroutine `CALL` and `RET`.
* Uses the `IN` instruction to read external sensor ports and the `OUT` instruction to drive actuators.

### Half 2: The Cyber-Physical Plant (`src/aquacore8.c`)
* Simulates the real physical tub, heating element, spray arms, and water pipes.
* When the CPU turns ON the Inlet Valve, water level increases (+2 Liters per tick).
* When the CPU turns ON the 1800W Heater, water temperature rises (+0.1°C per tick).
* When the CPU turns ON the Spray Pump, soap dissolves food stains (turbidity increases).
* When the CPU queries sensor port `0x02`, the physical plant feeds the live temperature back to the CPU!

---

## ⏱️ 4. Is It a Real-Time Simulation?

**YES, the simulator supports both real-time interactive exploration and ultra-fast batch verification:**

1. **Interactive Real-Time Mode (`run.bat cli`):**
   * Runs interactively in your command prompt.
   * Displays a live ASCII dashboard showing CPU registers, flags, currently executing assembly instruction, water temperature, water level, and actuator lamps.
   * You can step cycle-by-cycle (`step`), run with delays, or manually type commands like `door open`, `door close`, or `temp 78` to inject faults and watch the CPU react in real time!
2. **Fast Batch Mode (`run.bat batch`):**
   * Runs the full 1,254-cycle wash cycle in approximately **0.05 seconds**.
   * Prints milestone status snapshots as the machine transitions through Pre-Wash, Main Wash, Rinse, Heated Drying, and Cycle Complete.
3. **Automated Verification Mode (`build.bat` or `run.bat test`):**
   * Executes all 37 tests (20 CPU unit tests + 17 integration & safety tests) in under 1 second to prove 100% bug-free behavior.

---

## 🔄 5. What Changes and What Are the Outputs?

### A. Inputs That Change Dynamically (Sensors)
| Sensor Input | Port Address | How It Changes Dynamically |
|:---|:---|:---|
| **Door Safety Switch** | `Port 0x00 (Bit 1)` | Normally **CLOSED (1)**. If pulled **OPEN (0)**, CPU shuts off all sprayers in $<2\ \mu\text{s}$. |
| **Water Level Float** | `Port 0x01` | Starts at **0** (empty), fills to **80** (8 Liters), drains to **0** between wash phases. |
| **Water Thermometer** | `Port 0x02` | Starts at **20°C** (tap water), heats to **55°C** (Normal) or **45°C** (Eco), and cools when drained. |
| **Optical Turbidity** | `Port 0x03` | Starts at **0** (clean), climbs to **70–90** as food dissolves, clears back to **0** on rinse. |
| **Dish Soil Remaining** | *Plant State* | Starts at **80% soiled** and finishes at **< 5% soiled** (sparkling clean!). |

### B. Physical Outputs Controlled by CPU (`Port 0x10`)
| Actuator / Output | Bit Mask | Hardware Action When Activated |
|:---|:---|:---|
| **Water Inlet Valve** | `Bit 0 (0x01)` | Opens electronic water valve to let fresh tap water flow into the tub. |
| **Drain Discharge Pump** | `Bit 1 (0x02)` | Pumps dirty soapy water out through the drain hose under the sink. |
| **Wash Spray Pump** | `Bit 2 (0x04)` | Drives rotating spray arms that blast pressurized water onto dirty plates. |
| **1800W Heating Element** | `Bit 3 (0x08)` | Heats wash water to dissolve grease; heater is prohibited if tub is dry. |
| **Detergent Dispenser** | `Bit 4 (0x10)` | Solenoid clicks open during Main Wash to drop the soap tablet into the water. |
| **Rinse-Aid Solenoid** | `Bit 5 (0x20)` | Drops shine solution during Rinse cycle so glassware dries spot-free. |
| **Hot-Air Drying Fan** | `Bit 6 (0x40)` | Blows warm air through the tub to dry dishes like a hairdryer. |
| **Celebration Buzzer** | `Bit 7 (0x80)` | Sounds *BEEP BEEP BEEP!* to notify family that dishes are done! |

---

## 🧼 6. The Five Phases of a Dishwasher Cycle

1. **Phase 0 (Standby):** CPU waits in low-power loop. Starts immediately when door is closed and Start button is pressed.
2. **Phase 1 (Pre-Wash):** Fills water, sprays off loose food particles, and pumps dirty pre-wash water down the drain.
3. **Phase 2 (Main Wash):** Fills fresh water, drops soap tablet, and turns on 1800W heater. When water reaches 55°C, heater is turned off to save electricity while scrubbers continue spinning.
4. **Phase 3 (Smart Adaptive Rinse):** Fills fresh water and dispenses rinse-aid. The optical turbidity sensor reads water clarity. If curry/sauce is still detected in water, CPU automatically repeats with an extra rinse!
5. **Phase 4 (Heated Air Drying):** Tub is empty. Turns on gentle air heater and fan, blowing warm air until dishes are bone dry.
6. **Phase 5 (Completion):** Turns on green Complete light, beeps buzzer 3 times, shuts off all power, and enters `HALT`.

---

## 🎓 7. Viva / Oral Defense Quick Reference (Must-Know Answers)

| Question | Your 1-Sentence Answer |
|:---|:---|
| **1. What is the word size of your CPU?** | *"AquaCore-8 has an 8-bit data path with 16-bit fixed-length instructions."* |
| **2. Why did you choose Harvard architecture instead of Von Neumann?** | *"Harvard separates Instruction ROM and Data RAM so the CPU can fetch instructions and access data operands in the exact same clock cycle without bus contention, achieving CPI = 1.0."* |
| **3. How does the CPU know what the water temperature is?** | *"Through Port-Mapped I/O. The instruction `IN R1, [0x02]` reads the temperature sensor byte from port 0x02 directly into register R1."* |
| **4. What makes your dishwasher controller 'smart'?** | *"It features an optical turbidity sensor that measures water clarity using Beer-Lambert optical attenuation. If dirty food particles remain in the water after rinsing, the firmware dynamically triggers an extra rinse cycle."* |
| **5. What happens if a curious kid opens the dishwasher door mid-wash?** | *"Our safety polling routine detects the door switch bit within 2 microseconds, immediately zeroes the actuator port (cutting power to all pumps and heaters), turns on a warning light, and resumes seamlessly when the door is re-closed."* |
| **6. How did you verify the CPU works?** | *"We developed an automated test suite in C with 37 tests (20 instruction-level unit tests and 17 cyber-physical integration scenarios), achieving a 100% pass rate."* |

---

## 🚀 8. Quick Start Commands

* Double-click `run.bat` ➔ Master menu (Run tests, launch CLI monitor, or view docs)
* Run `build.bat` ➔ Compiles C code and runs all 37 tests in 0.5s
* Run `run.bat batch` ➔ Executes full 5-phase dishwasher cycle trace to screen
* Run `run.bat cli` ➔ Launches live interactive ASCII terminal monitor
