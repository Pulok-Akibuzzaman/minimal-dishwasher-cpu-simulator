# EAST WEST UNIVERSITY
## Faculty of Sciences and Engineering
### Department of Computer Science and Engineering

---

<br><br><br>

# A MINIMAL CPU ARCHITECTURE (AquaCore-8) FOR AUTOMATING DISHWASHER CONTROL FUNCTIONS WITH SENSOR INPUTS AND SIMULATOR VERIFICATION

<br>

### Course: CSE360 – Computer Architecture
### Semester: Fall 2026

<br><br>

**Submitted By:**
* **Nusrat Rahman Aurna** — ID: 2023-3-60-057
* **Pulok Akibuzzaman** — ID: 2023-3-60-051
* **Md. Jakaria Masud** — ID: 2023-3-60-386

<br>

**Supervised By / Course Instructor:**
Department of Computer Science and Engineering  
East West University, Dhaka, Bangladesh

<br><br>
**Date of Submission:** October 2026  
**Format:** A4 Size Project Report

---

\newpage

# Table of Contents
1. [Title](#1-title)
2. [Objective](#2-objective)
3. [Theory](#3-theory)
   - 3.1 Principles of Application-Specific Instruction-Set Processors (ASIP)
   - 3.2 Harvard Architecture vs. Von Neumann Architecture
   - 3.3 Sensor Transduction & Mathematical Modeling
   - 3.4 Arithmetic Logic Unit (ALU) Boolean Derivations & Status Flags
   - 3.5 Single-Cycle Datapath Timing Analysis & Critical Path Formulation
4. [Design](#4-design)
   - 4.1 AquaCore-8 Architectural Specifications
   - 4.2 Instruction Set Architecture (ISA) & Encoding Formats
   - 4.3 Control Unit Logic & Truth Table
   - 4.4 Datapath Block Schematic
   - 4.5 Memory & I/O Port Addressing Map
   - 4.6 Automated Dishwasher Control State Machine & Algorithms
5. [Implementation](#5-implementation)
   - 5.1 System Architecture & Module Hierarchy
   - 5.2 Pure C Simulation Engine & Micro-Architecture Simulator
   - 5.3 Firmware Assembly Architecture & Machine Code Generation
   - 5.4 Dishwasher Cyber-Physical Plant Simulation
   - 5.5 Interactive Terminal Dashboard & Batch Cycle Engine
   - 5.6 Hardware Synthesizable Verilog RTL Implementation
   - 5.7 System Requirements
6. [Debugging and Test-Run](#6-debugging-and-test-run)
   - 6.1 Testing Methodology
   - 6.2 CPU Architecture Unit Tests (Instruction Level)
   - 6.3 End-to-End Dishwasher Integration Cycles
   - 6.4 Safety Interlock & Fault Injection Testing
   - 6.5 Execution Trace Logs
7. [Results Analysis](#7-results-analysis)
   - 7.1 Algorithmic Time and Space Complexity
   - 7.2 Microprocessor Cycle Performance & Instruction Profile
   - 7.3 Memory Footprint & Resource Efficiency
   - 7.4 Robustness & Failsafe Guarantees
8. [Conclusion and Future Improvements](#8-conclusion-and-future-improvements)
   - 8.1 Project Summary
   - 8.2 Scope Limitations
   - 8.3 Future Improvements
   - 8.4 Applications of the Proposed Architecture
9. [Bibliography](#9-bibliography)

---

\newpage

# 1. Title
**Suggest and Design a Minimal CPU Architecture, Including ISA, Datapath, and Control Unit, for Automating Dishwasher Control Functions with Sensor Inputs, and Verify It with a Simulator.**

---

# 2. Objective
The primary goal of this project is to research, design, implement, and verify a minimal, application-specific 8-bit microprocessor architecture named **AquaCore-8**, purpose-built for embedded home appliance automation—specifically an intelligent, sensor-driven automated dishwasher controller.

The detailed objectives include:
1. **Instruction Set Architecture (ISA) Design**: Formulating an orthogonal, minimal 16-bit fixed-length instruction set tailored for sensor acquisition, real-time actuator control, conditional branching, and modular subroutine handling.
2. **Datapath & Micro-Architecture Synthesis**: Engineering a clean single-cycle datapath incorporating a 4-register file, an 8-bit Arithmetic Logic Unit (ALU) with four status condition flags (Zero, Carry, Negative, Overflow), an 8-bit Program Counter (PC), and a hardware return stack for nested subroutine calls.
3. **Hardwired Control Unit Synthesis**: Formulating the control truth table and Boolean decoding equations that map 4-bit opcodes and 3-bit sub-functions into control lines without multi-cycle microcode overhead.
4. **Sensor and Actuator Interfacing**: Architecting dedicated Port-Mapped I/O channels linking the CPU to essential dishwasher sensors (Door Interlock, Water Level, Water Temperature, Optical Turbidity, and Auxiliary Leak/Detergent sensors) and actuators (Inlet Valve, Drain Pump, Spray Circulation Pump, Heating Element, Solenoid Dispensers, Drying Fan, and Buzzer).
5. **Software Toolchain & Machine Code Generation**: Translating human-readable assembly language (`.asm`) into binary and hexadecimal machine code (`.bin`/`.hex`).
6. **Physical Plant & Environmental Emulation**: Modeling fluid dynamics, thermodynamic heating, dish soil dissolving, and turbidity clearing in a concurrent software plant environment.
7. **Verification & Simulation**: Building an interactive real-time terminal visualizer and an automated regression test suite in pure C to prove 100% functional correctness, fault tolerance under emergency door-open conditions, and thermal overload shutoff.
8. **Hardware Verilog RTL Validation**: Providing synthesizable Verilog HDL descriptions for standard FPGA deployment.

---

# 3. Theory & Mathematical Formulations

## 3.1 Principles of Application-Specific Instruction-Set Processors (ASIP)
General-purpose microprocessors (e.g., x86, ARM Cortex-A) contain extensive architectural complexity, including multi-level cache hierarchies, out-of-order execution, branch prediction tables, and floating-point pipelines. For dedicated embedded appliances such as dishwashers, washing machines, and microwave ovens, such complexity is unnecessary, expensive, and power-inefficient. 

An **Application-Specific Instruction-Set Processor (ASIP)** balances hardware cost and programming flexibility. The processor provides:
- Word size sized strictly to sensor bit precision (8-bit ADC resolution).
- Deterministic instruction execution timing (1 cycle per instruction).
- Direct bitwise manipulation for peripheral control.
- Minimal transistor count and low silicon area.

## 3.2 Harvard Architecture vs. Von Neumann Architecture
In a classical **Von Neumann architecture**, instructions and data share a common physical bus and memory space. This causes the famous *Von Neumann Bottleneck*, where instruction fetch and data read/write cannot occur simultaneously in a single clock cycle.

In contrast, **AquaCore-8** adopts a **Harvard architecture**:
- **Instruction Memory (ROM)**: 256 words $\times$ 16 bits. Addressed exclusively by the Program Counter ($PC$).
- **Data Memory (RAM)**: 256 bytes $\times$ 8 bits. Addressed by operand address lines for variable storage and cycle parameters.
- **I/O Space**: 32 port-mapped peripheral addresses separating sensor/actuator registers from general data memory.

This spatial separation enables single-cycle fetch and execute: in the same clock period, the CPU fetches the next instruction from ROM while simultaneously reading/writing data memory or peripheral ports ($CPI = 1.0$).

## 3.3 Cyber-Physical Sensor Transduction & Analytical Formulations

An intelligent dishwasher controller operates as a closed-loop cyber-physical system interacting with fluid, thermodynamic, optical, and mechanical physical domains. The governing equations are derived as follows:

### 1. Hydrodynamic Fluid Continuity & Water Level Transduction
Water intake flow is controlled by an electromagnetic solenoid valve governed by Torricelli's orifice discharge equation:
$$Q_{in}(t) = C_d \cdot A_{valve} \cdot \sqrt{\frac{2(P_{mains} - P_{tub})}{\rho}} \cdot U_{inlet}(t) \tag{1}$$

Where:
- $C_d \approx 0.62$ is the dimensionless orifice discharge coefficient.
- $A_{valve}$ is the cross-sectional area of the valve orifice ($m^2$).
- $\Delta P = P_{mains} - P_{tub}$ is the pressure differential across the valve ($\approx 3.0\text{ bar} = 3 \times 10^5\text{ Pa}$).
- $\rho \approx 1000\text{ kg/m}^3$ is the density of water.
- $U_{inlet}(t) \in \{0, 1\}$ is the CPU binary driving signal from Port `0x10` bit 0.

Discharge flow pumped out by the centrifugal drain impeller is governed by the pump head characteristic curve:
$$Q_{drain}(t) = \eta_{pump} \cdot \left(Q_{max} - k_{loss} \cdot h(t)\right) \cdot U_{drain}(t) \tag{2}$$

Where $\eta_{pump} \approx 0.85$, $Q_{max}$ is maximum zero-head volumetric discharge, and $k_{loss}$ is the head loss coefficient.

Applying the **Principle of Conservation of Mass** to the wash tub of horizontal cross-sectional area $A_{tub}$, the liquid height $h(t)$ and volume $V(t)$ satisfy the continuity differential equation:
$$\frac{dV(t)}{dt} = A_{tub} \cdot \frac{dh(t)}{dt} = Q_{in}(t) - Q_{drain}(t) \tag{3}$$

The hydrostatic pressure at the sump transducer is:
$$P_{hydro}(t) = P_{atm} + \rho \cdot g \cdot h(t) \tag{4}$$

Which is digitized by the on-chip 8-bit Analog-to-Digital Converter (ADC) into an 8-bit integer level $L_{ADC} \in [0, 255]$:
$$L_{ADC}(t) = \min\left(255, \; \left\lfloor 255 \cdot \frac{V(t)}{V_{max}} \right\rfloor\right) \tag{5}$$

Defined architectural threshold registers:
- $L_{empty} = 0\text{x}00$ ($0\text{ L}$, tub empty).
- $L_{safe\_heater} = 0\text{x}10$ ($1.2\text{ L}$, anti-dry-burn threshold).
- $L_{normal} = 0\text{x}50$ ($4.0\text{ L}$, nominal fill level).
- $L_{heavy} = 0\text{x}60$ ($5.0\text{ L}$, heavy deep-fill level).
- $L_{overflow} = 0\text{x}80$ ($6.5\text{ L}$, emergency overflow trip).

### 2. Thermodynamic Energy Balance & Transient Heating Kinetics
Applying the **First Law of Thermodynamics** for a lumped-capacitance fluid-structure thermal system gives:
$$C_{th}(t) \cdot \frac{dT(t)}{dt} = P_{rated} \cdot \eta_{heat} \cdot U_{heater}(t) - U_{loss} A \cdot (T(t) - T_{amb}) - \dot{m}_{in}(t) c_p (T(t) - T_{in}) \tag{6}$$

Where:
- $P_{rated} = 1800\text{ W}$ is the electrical heating element nominal power rating.
- $\eta_{heat} \approx 0.98$ is the submerged joule heating efficiency.
- $U_{heater}(t) \in \{0, 1\}$ is Port `0x10` bit 3.
- $U_{loss} A \approx 18.5\text{ W/K}$ is the overall thermal conductance to ambient air.
- $c_p = 4184\text{ J/(kg}\cdot\text{K)}$ is the isobaric specific heat capacity of water.

The composite thermal capacitance of water and the stainless steel tub is:
$$C_{th}(t) = \rho \cdot V(t) \cdot c_p + M_{tub} \cdot c_{ss} \tag{7}$$

Where $M_{tub} \approx 12\text{ kg}$ is tub mass and $c_{ss} = 500\text{ J/(kg}\cdot\text{K)}$ is stainless steel specific heat capacity.

During active heating without water inflow ($\dot{m}_{in} = 0$, $U_{heater} = 1$), the closed-form analytical solution of the ordinary differential equation is:
$$T(t) = T_{amb} + \left(T_0 - T_{amb}\right) e^{-\frac{t}{\tau_{th}}} + \frac{P_{rated} \cdot \eta_{heat}}{U_{loss} A} \left(1 - e^{-\frac{t}{\tau_{th}}}\right) \tag{8}$$

With thermal time constant:
$$\tau_{th} = \frac{C_{th}}{U_{loss} A} = \frac{\rho V c_p + M_{tub} c_{ss}}{U_{loss} A} \tag{9}$$

### 3. NTC Thermistor Transduction & Inverse Steinhart-Hart Formulation
The thermistor resistance $R_T(T)$ decreases nonlinearly with absolute temperature $T$ (Kelvin) according to the exponential semiconductor relation:
$$R_T(T) = R_0 \cdot \exp\left[\beta \cdot \left(\frac{1}{T} - \frac{1}{T_0}\right)\right] \tag{10}$$

Where $R_0 = 10\text{ k}\Omega$ at reference temperature $T_0 = 298.15\text{ K}$ ($25^\circ\text{C}$), and $\beta \approx 3950\text{ K}$.

In a precision voltage divider with bias resistor $R_{bias} = 10\text{ k}\Omega$ and reference voltage $V_{ref}$:
$$V_{temp}(T) = V_{ref} \cdot \frac{R_T(T)}{R_{bias} + R_T(T)} \tag{11}$$

The exact temperature in degrees Celsius ($^\circ\text{C}$) is extracted via the inverted Steinhart-Hart formulation:
$$T(^\circ\text{C}) = \left[ \frac{1}{T_0} + \frac{1}{\beta}\ln\left(\frac{R_T}{R_0}\right) \right]^{-1} - 273.15 \tag{12}$$

Which is mapped to Port `0x02` as an unsigned 8-bit reading ($0$ to $100^\circ\text{C}$).

### 4. Optical Infrared Turbidity & Dish Cleaning Reaction Kinetics
Suspended food particulate concentration (soiling) attenuates optical transmission ($\lambda = 940\text{ nm}$) according to the **Beer-Lambert Extinction Law**:
$$I(\tau) = I_0 \cdot \exp\left[-(\mu_a + \mu_s) \cdot d\right] = I_0 \cdot \exp\left[-\kappa \cdot \text{NTU} \cdot d\right] \tag{13}$$

Where $\mu_a$ is absorption coefficient, $\mu_s$ is scattering coefficient proportional to Nephelometric Turbidity Units (NTU), and $d$ is optical path length.

The sensor output voltage across the phototransistor load resistor is:
$$V_{turb} = V_{clean} \cdot \exp\left[-\kappa \cdot d \cdot \text{NTU}\right] \tag{14}$$

The 8-bit turbidity index $\tau \in [0, 100]$ fed into Port `0x03` is:
$$\tau_{ADC} = \left\lfloor 100 \cdot \left(1 - \frac{V_{turb}}{V_{clean}}\right) \right\rfloor \tag{15}$$

The mechanical removal of dish soil $S_{soil}(t)$ follows a first-order chemical-mechanical rate reaction:
$$\frac{dS_{soil}(t)}{dt} = - k_{mech} \cdot \omega_{spray} \cdot \left(1 + \gamma_{det} \cdot U_{det}\right) \cdot S_{soil}(t) \tag{16}$$
$$S_{soil}(t) = S_0 \cdot \exp\left[-K_{eff} \cdot t\right] \tag{17}$$

Where $\omega_{spray}$ is wash spray pump circulation velocity, and $\gamma_{det}$ is detergent enzymatic acceleration factor.

### 5. Mathematical Safety Invariant Formulation
The system operating safety state $S_{safe}(t)$ is formally defined by the logical Boolean conjunction:
$$S_{safe}(t) \iff \left(D_{door}(t) = 1\right) \land \left(\overline{\text{Leak}(t)} = 1\right) \land \left(T(t) \le 75^\circ\text{C}\right) \tag{18}$$

The firmware and hardware safety interlock asserts the universal invariant:
$$\forall t, \quad \overline{S_{safe}(t)} \implies \left(U_{inlet}(t) = 0 \land U_{drain}(t) = 0 \land U_{spray}(t) = 0 \land U_{heater}(t) = 0\right) \tag{19}$$

---

## 3.4 Arithmetic Logic Unit (ALU) Boolean Equations & Flag Derivations

The AquaCore-8 ALU performs operations on two 8-bit binary vectors $A = [a_7, a_6, a_5, a_4, a_3, a_2, a_1, a_0]$ and $B = [b_7, b_6, b_5, b_4, b_3, b_2, b_1, b_0]$, yielding 8-bit result $S = [s_7, s_6, s_5, s_4, s_3, s_2, s_1, s_0]$.

### 1. Carry-Lookahead (CLA) Bit Equations
For each bit stage $i \in \{0, 1, \dots, 7\}$:
$$g_i = a_i \cdot b_i \quad \text{(Carry Generate)} \tag{20}$$
$$p_i = a_i \oplus b_i \quad \text{(Carry Propagate)} \tag{21}$$
$$c_{i+1} = g_i + p_i \cdot c_i \tag{22}$$
$$s_i = p_i \oplus c_i \tag{23}$$

### 2. Condition Flags Boolean Derivations
- **Zero Flag ($Z$):** True if and only if all result bits are logic zero:
  $$Z = \prod_{i=0}^{7} \overline{s_i} = \overline{\bigvee_{i=0}^{7} s_i} = \text{NOR}(s_7, s_6, s_5, s_4, s_3, s_2, s_1, s_0) \tag{24}$$

- **Negative Flag ($N$):** Equals the most-significant sign bit in two's complement notation:
  $$N = s_7 \tag{25}$$

- **Carry / Borrow Flag ($C$):** Represents arithmetic carry-out for addition, and unsigned borrow for subtraction/comparison:
  $$C_{add} = c_8 \tag{26}$$
  $$C_{sub} = \overline{c_8} \equiv (A < B) \tag{27}$$

- **Two's Complement Signed Overflow Flag ($V$):** Detects arithmetic wrap-around where the result sign contradicts the operand signs:
  - For Addition ($A + B$):
    $$V_{add} = \overline{a_7} \cdot \overline{b_7} \cdot s_7 + a_7 \cdot b_7 \cdot \overline{s_7} = (a_7 \oplus s_7) \cdot (b_7 \oplus s_7) \tag{28}$$
  - For Subtraction ($A - B$):
    $$V_{sub} = a_7 \cdot \overline{b_7} \cdot \overline{s_7} + \overline{a_7} \cdot b_7 \cdot s_7 = (a_7 \oplus b_7) \cdot (a_7 \oplus s_7) \tag{29}$$
  - In terms of stage carries:
    $$V = c_7 \oplus c_8 \tag{30}$$

### 3. Signed Relational Branching Logic Derivations
Using the status flags, signed relational comparisons evaluate as:
$$\text{Signed Less-Than: } \quad (A < B) \iff (N \oplus V = 1) \tag{31}$$
$$\text{Signed Greater-or-Equal: } \quad (A \ge B) \iff (N \oplus V = 0) \tag{32}$$
$$\text{Signed Greater-Than: } \quad (A > B) \iff \left(\overline{Z} \land (N \oplus V = 0)\right) \tag{33}$$
$$\text{Signed Less-or-Equal: } \quad (A \le B) \iff \left(Z \lor (N \oplus V = 1)\right) \tag{34}$$

---

## 3.5 Single-Cycle Datapath Timing Analysis & Critical Path Equations

In a single-cycle datapath, the clock period $T_{clk}$ must be greater than or equal to the total propagation delay through the longest structural path:
$$T_{clk} \ge T_{cq,PC} + T_{ROM} + T_{dec} + T_{rf\_read} + T_{ALU} + T_{mux} + T_{rf\_setup} + T_{skew} \tag{35}$$

Where:
- $T_{cq,PC} = 0.8\text{ ns}$ (Clock-to-Q delay of Program Counter register).
- $T_{ROM} = 4.2\text{ ns}$ (Instruction ROM access latency).
- $T_{dec} = 1.1\text{ ns}$ (Control unit decoder propagation delay).
- $T_{rf\_read} = 1.5\text{ ns}$ (Register file access delay).
- $T_{ALU} = 3.8\text{ ns}$ (8-bit carry-lookahead ALU propagation time).
- $T_{mux} = 0.6\text{ ns}$ (Writeback multiplexer select delay).
- $T_{rf\_setup} = 0.5\text{ ns}$ (Register file setup time).

$$T_{clk,min} = 0.8 + 4.2 + 1.1 + 1.5 + 3.8 + 0.6 + 0.5 = 12.5\text{ ns}$$

The maximum theoretical operating frequency is:
$$f_{max} = \frac{1}{T_{clk,min}} = \frac{1}{12.5 \times 10^{-9}\text{ s}} \approx 80.0\text{ MHz} \tag{36}$$

The dynamic power dissipation is governed by:
$$P_{dyn} = \alpha \cdot C_L \cdot V_{DD}^2 \cdot f_{clk} \tag{37}$$

At nominal operating conditions ($V_{DD} = 3.3\text{ V}$, $C_L \approx 15\text{ pF}$, $f_{clk} = 10\text{ MHz}$, activity factor $\alpha \approx 0.1$), dynamic power is:
$$P_{dyn} = 0.1 \cdot (15 \times 10^{-12}\text{ F}) \cdot (3.3\text{ V})^2 \cdot (10 \times 10^6\text{ Hz}) = 0.163\text{ mW} \tag{38}$$
Leaving ample thermal margin and enabling ultra-low power standby operation.

---

# 4. Design

## 4.1 AquaCore-8 Architectural Specifications
- **Data Bus Width**: 8-bit
- **Instruction Width**: 16-bit fixed-length
- **Instruction Memory**: 256 words (ROM, 16-bit)
- **Data Memory**: 256 bytes (RAM, 8-bit)
- **Register File**: 4 General-Purpose Registers: `R0`, `R1`, `R2`, `R3` (2-bit address)
- **Program Counter ($PC$)**: 8-bit (supports 256 instructions)
- **Hardware Return Stack**: 16-level deep circular stack for nested `CALL` and `RET`
- **Status Flags**: 4 condition flags: $Z$ (Zero), $C$ (Carry/Borrow), $N$ (Negative), $V$ (Overflow)
- **Execution Model**: Deterministic single-cycle execution ($CPI = 1.0$)

## 4.2 Instruction Set Architecture (ISA) & Encoding Formats

Every instruction is encoded in exactly 16 bits, structured into three primary formats:

### Format 1: R-Type (Register-Register Format)
```
 15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
+---------------+-------+-------+-------+-----------+-------+---+
|  Opcode=0000  |  Rd   |  Rs   |  Rt   |   Funct   | Resvd |SHR|
+---------------+-------+-------+-------+-----------+-------+---+
```
- `Opcode` [15:12] = `0000` ($0\text{x}0$)
- `Rd` [11:10]: Destination register ($00 = R_0, 01 = R_1, 10 = R_2, 11 = R_3$)
- `Rs` [9:8]: First source register
- `Rt` [7:6]: Second source register
- `Funct` [5:3]: ALU sub-function code:
  - `000`: `ADD Rd, Rs, Rt` ($\text{Rd} = \text{Rs} + \text{Rt}$)
  - `001`: `SUB Rd, Rs, Rt` ($\text{Rd} = \text{Rs} - \text{Rt}$)
  - `010`: `AND Rd, Rs, Rt` ($\text{Rd} = \text{Rs} \land \text{Rt}$)
  - `011`: `OR  Rd, Rs, Rt` ($\text{Rd} = \text{Rs} \lor \text{Rt}$)
  - `100`: `XOR Rd, Rs, Rt` ($\text{Rd} = \text{Rs} \oplus \text{Rt}$)
  - `101`: `CMP Rs, Rt` (Evaluate $\text{Rs} - \text{Rt}$, update $Z, C, N, V$)
  - `110`: `NOT Rd, Rs` ($\text{Rd} = \overline{\text{Rs}}$)
  - `111`: `SHF Rd, Rs` (Shift: Bit 0 determines direction: $0 = \text{SHL}, 1 = \text{SHR}$)

### Format 2: I-Type (Immediate / Memory / IO Format)
```
 15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
+---------------+-------+-------+-------------------------------+
|    Opcode     | Rd/Rs | Mode  |     Immediate / Address / Port|
+---------------+-------+-------+-------------------------------+
```
- `Opcode` [15:12]:
  - `0001` ($0\text{x}1$): `ADDI Rd, #imm8` ($\text{Rd} = \text{Rd} + imm8$)
  - `0010` ($0\text{x}2$): `SUBI Rd, #imm8` ($\text{Rd} = \text{Rd} - imm8$)
  - `0011` ($0\text{x}3$): `CMPI Rd, #imm8` (Compare $\text{Rd} - imm8$)
  - `0100` ($0\text{x}4$): `ANDI Rd, #imm8` ($\text{Rd} = \text{Rd} \land imm8$)
  - `0101` ($0\text{x}5$): `ORI  Rd, #imm8` ($\text{Rd} = \text{Rd} \lor imm8$)
  - `0110` ($0\text{x}6$): `LDI  Rd, #imm8` ($\text{Rd} = imm8$)
  - `0111` ($0\text{x}7$): `LOAD Rd, [addr8]` ($\text{Rd} = \text{RAM}[addr8]$)
  - `1000` ($0\text{x}8$): `STORE [addr8], Rs` ($\text{RAM}[addr8] = \text{Rs}$)
  - `1001` ($0\text{x}9$): `IN   Rd, [port8]` ($\text{Rd} = \text{PORT}[port8]$)
  - `1010` ($0\text{x}A$): `OUT  [port8], Rs` ($\text{PORT}[port8] = \text{Rs}$)

### Format 3: J-Type / B-Type (Control Flow & Branch Format)
```
 15  14  13  12  11  10   9   8   7   6   5   4   3   2   1   0
+---------------+---------------+-------------------------------+
|    Opcode     |   Condition   |         Target Address        |
+---------------+---------------+-------------------------------+
```
- `Opcode` [15:12]:
  - `1011` ($0\text{x}B$): `BRANCH cond, target8` (Conditional Jump)
    - Condition codes: `0=JZ, 1=JNZ, 2=JC, 3=JNC, 4=JN, 5=JP, 6=JV, 7=JGT, 8=JLT, 9=JGE, A=JLE`
  - `1100` ($0\text{x}C$): `JMP target8` (Unconditional Jump: $PC = target8$)
  - `1101` ($0\text{x}D$): Subroutine Control:
    - Bit 11 = $0$: `CALL target8` (Push $PC+1$, jump to target)
    - Bit 11 = $1$: `RET` (Pop $PC$ from return stack)
  - `1110` ($0\text{x}E$): `WAIT #ticks` (Synchronous hardware delay cycle)
  - `1111` ($0\text{x}F$): System Instructions (`0=NOP, 1=HALT`)

## 4.3 Control Unit Logic & Truth Table

The control unit is designed as a hardwired decoder producing active-high control lines:

| Instruction | Opcode | Funct | RegWrite | ALUSrc | ALUOp | MemRead | MemWrite | IORead | IOWrite | RegDstSrc | Branch/Jump |
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **ADD / SUB / LOGIC** | 0000 | 0..4,6,7 | 1 | 0 | Funct | 0 | 0 | 0 | 0 | 00 (ALU) | None |
| **CMP** | 0000 | 5 | 0 | 0 | SUB (1) | 0 | 0 | 0 | 0 | XX | None |
| **ADDI** | 0001 | X | 1 | 1 | ADD (0) | 0 | 0 | 0 | 0 | 00 (ALU) | None |
| **SUBI** | 0010 | X | 1 | 1 | SUB (1) | 0 | 0 | 0 | 0 | 00 (ALU) | None |
| **CMPI** | 0011 | X | 0 | 1 | SUB (1) | 0 | 0 | 0 | 0 | XX | None |
| **ANDI / ORI** | 0100..0101 | X | 1 | 1 | AND/OR | 0 | 0 | 0 | 0 | 00 (ALU) | None |
| **LDI** | 0110 | X | 1 | X | X | 0 | 0 | 0 | 0 | 11 (Imm) | None |
| **LOAD** | 0111 | X | 1 | X | X | 1 | 0 | 0 | 0 | 01 (Mem) | None |
| **STORE** | 1000 | X | 0 | X | X | 0 | 1 | 0 | 0 | XX | None |
| **IN** | 1001 | X | 1 | X | X | 0 | 0 | 1 | 0 | 10 (IO) | None |
| **OUT** | 1010 | X | 0 | X | X | 0 | 0 | 0 | 1 | XX | None |
| **BRANCH** | 1011 | Cond | 0 | X | X | 0 | 0 | 0 | 0 | XX | If Cond=1 |
| **JMP** | 1100 | X | 0 | X | X | 0 | 0 | 0 | 0 | XX | Target |
| **CALL** | 1101 | 0 | 0 | X | X | 0 | 0 | 0 | 0 | XX | Push & Jmp |
| **RET** | 1101 | 1 | 0 | X | X | 0 | 0 | 0 | 0 | XX | Pop to PC |
| **HALT** | 1111 | 1 | 0 | X | X | 0 | 0 | 0 | 0 | XX | Halt Core |

## 4.4 Datapath Block Schematic

```
                 +-------------------+
                 |  Program Counter  |<-----------------------+
                 |       (PC)        |                        |
                 +---------+---------+                        |
                           |                                  |
                           v                                  |
                 +-------------------+                        |
                 |Instruction Memory |                        |
                 |    (256 x 16)     |                        |
                 +---------+---------+                        |
                           |                                  |
        +------------------+------------------+               |
        | [15:12]          | [11:10]          | [7:0]         |
        v                  v                  v               |
+---------------+  +---------------+  +---------------+       |
| Control Unit  |  | Register File |  | Immediate Mux |       |
|               |  |  (4 x 8-bit)  |  +-------+-------+       |
| RegWrite      |  |               |          |               |
| ALUSrc        |  | Rs_data ----+ |          | imm8          |
| ALUOp         |  |             | |          v               |
| MemRead/Write |  | Rt_data --+ | |   +--------------+       |
| IORead/Write  |  +-----------|-+-+   | ALUSrc MUX   |       |
+---------------+              | |     +-------+------+       |
        |                      | +---------+   |              |
        |                      |           |   | Operand B    |
        |                      v           v   v              |
        |                 +----------------------+            |
        +---------------->| 8-Bit ALU with Flags |            |
                          |  [Z, C, N, V Flags]  |            |
                          +-----------+----------+            |
                                      |                       |
                +---------------------+                       |
                | Result              |                       |
                v                     v                       |
        +---------------+     +---------------+               |
        |  Data Memory  |     |  I/O Ports    |               |
        |  (256 x 8)    |     |  (Sensors &   |               |
        |  [RAM]        |     |   Actuators)  |               |
        +-------+-------+     +-------+-------+               |
                |                     |                       |
                +----------------+----+                       |
                                 |                            |
                                 v                            |
                      +--------------------+                  |
                      | Writeback Data MUX |------------------+
                      | (ALU/RAM/IO/Imm)   |
                      +--------------------+
```

## 4.5 Memory & I/O Port Addressing Map

### Input Ports (Sensors)
| Port Address | Port Symbol | Description | Bitfield Mapping |
|:---:|:---|:---|:---|
| `0x00` | `PORT_SYS_IN` | System Controls & Status | Bit 0: Start Button (1=Pressed)<br>Bit 1: Door Closed (1=Closed, 0=Ajar)<br>Bit 2: Cancel/Reset<br>Bits 4-7: Cycle Mode Selector (`0`=Normal, `1`=Eco, `2`=Heavy) |
| `0x01` | `PORT_WATER_LVL` | Water Level Transducer | `0x00` (Empty) to `0x80` (Overflow) |
| `0x02` | `PORT_WATER_TEMP`| Water Thermistor ADC | Calibrated direct reading in °C (`20` to `100`°C) |
| `0x03` | `PORT_TURBIDITY` | Optical IR Turbidity | `0x00` (Clear) to `0x64` (Heavily Soiled) |
| `0x04` | `PORT_AUX_SENS`  | Safety & Level Alarms | Bit 0: Detergent Empty<br>Bit 1: Rinse Aid Low<br>Bit 3: Tub Leak Detect Switch |

### Output Ports (Actuators & Display)
| Port Address | Port Symbol | Description | Bitfield Mapping |
|:---:|:---|:---|:---|
| `0x10` | `PORT_ACTUATORS` | Actuator Relay Drivers | Bit 0: Water Inlet Solenoid Valve (`0x01`)<br>Bit 1: Drain Discharge Pump (`0x02`)<br>Bit 2: Wash Spray Circulation Pump (`0x04`)<br>Bit 3: 1800W Heating Element (`0x08`)<br>Bit 4: Detergent Dispenser Solenoid (`0x10`)<br>Bit 5: Rinse Aid Solenoid (`0x20`)<br>Bit 6: Heated Air Drying Fan (`0x40`)<br>Bit 7: Alarm Buzzer (`0x80`) |
| `0x11` | `PORT_STATUS_LEDS`| Front Panel Status LEDs| Bit 0: Standby<br>Bit 1: Pre-Wash<br>Bit 2: Main Wash<br>Bit 3: Rinse<br>Bit 4: Drying<br>Bit 5: Complete<br>Bit 6: Door Open Warning<br>Bit 7: System Fault |
| `0x12` | `PORT_PROGRESS`  | 7-Segment Progress | Displays active stage number (`0..5`) or error code (`0xAA`=Overheat, `0xEF`=Leak) |

## 4.6 Automated Dishwasher Control State Machine & Algorithms

The dishwasher control sequence is governed by a finite state machine executed by `dishwasher_controller.asm`:

```
               [POWER ON / RESET]
                       |
                       v
                 +-----------+
                 |  STANDBY  |<-------------------------+
                 +-----+-----+                           |
                       | (Start Pressed & Door Closed)   |
                       v                                 |
              [Mode Configuration]                       |
         (Sets Temp, Level, Time in RAM)                 |
                       |                                 |
                       v                                 |
               +---------------+                         |
               | 1. PRE-WASH   |                         |
               | - Fill Tub    |                         |
               | - Spray Wash  |                         |
               | - Drain Waste |                         |
               +-------+-------+                         |
                       |                                 |
                       v                                 |
               +---------------+                         |
               | 2. MAIN WASH  |                         |
               | - Fresh Fill  |                         |
               | - Dispense Det|                         |
               | - Heat & Wash |                         |
               | - Check Turb. |                         |
               | - Drain Tub   |                         |
               +-------+-------+                         |
                       |                                 |
                       v                                 |
               +---------------+                         |
               | 3. RINSE      |<----+                   |
               | - Fill Tub    |     |                   |
               | - Rinse Aid   |     | (If Turbid > 18)  |
               | - Circulate   |     |                   |
               | - Drain Tub   |-----+                   |
               +-------+-------+                         |
                       |                                 |
                       v                                 |
               +---------------+                         |
               | 4. DRYING     |                         |
               | - Heat Element|                         |
               | - Air Blower  |                         |
               +-------+-------+                         |
                       |                                 |
                       v                                 |
               +---------------+                         |
               | 5. COMPLETE   |                         |
               | - Buzzer Beep |                         |
               | - LED Complete|                         |
               +-------+-------+                         |
                       |                                 |
                       +---------------------------------+
```

---

# 5. Implementation

## 5.1 System Architecture & Module Hierarchy
The software simulation is implemented exclusively in **pure C (C99 standard)** with synthesizable Verilog HDL descriptions for hardware validation:
1. **Microprocessor Simulation Engine (Pure C)**: Located in `src/`. Implements the exact register file, 8-bit ALU, single-cycle instruction decoder, Harvard ROM/RAM, machine code disassembler, and dishwasher cyber-physical plant in standard C (`uint8_t`, `uint16_t`). It compiles natively with GCC into a high-performance executable (`aquacore8_sim.exe`).
2. **Interactive Real-Time Console Visualizer & Batch Engine (C)**: Provides a real-time terminal visualizer displaying physical sensor dials, animated cycle stages, actuator lamps, and micro-architectural register states, alongside an automated batch runner.
3. **Hardware Description (Verilog HDL)**: Synthesizable RTL modules implementing the exact datapath, ALU, control unit, and memories.

The project structure is organized cleanly as follows:
```text
Mini-Project/
├── run.bat                           ; Master interactive menu launcher & mode runner
├── build.bat                         ; One-click C compiler & 37-test suite runner
├── README.md                         ; Master project README & documentation index
├── Miniproject description.pdf       ; Course assignment guidelines
├── src/
│   ├── aquacore8.h                   ; CPU core, register file, memory & I/O definitions
│   ├── aquacore8.c                   ; Execution core, ALU logic & machine code disassembler
│   ├── dishwasher_plant.h            ; Cyber-physical fluid & thermodynamic header
│   ├── dishwasher_plant.c            ; Water heating, inflow/drain & optical turbidity equations
│   ├── tests.c                       ; 37 automated unit and full integration tests in C
│   └── main.c                        ; Interactive CLI dashboard & batch runner
├── verilog/
│   ├── cpu_top.v                     ; Top-level synthesizable processor
│   ├── alu.v                         ; 8-bit Arithmetic Logic Unit
│   ├── control_unit.v                ; Hardwired instruction decoder
│   ├── datapath.v                    ; Register file, PC, stack, and ALUSrc MUX
│   ├── register_file.v               ; 4 x 8-bit register storage
│   ├── instruction_memory.v          ; 256 x 16-bit ROM
│   ├── data_memory.v                 ; 256 x 8-bit RAM
│   ├── io_controller.v               ; Port-mapped sensor/actuator registers
│   └── dishwasher_tb.v               ; Clock, reset, and sensor testbench
├── programs/
│   ├── dishwasher_controller.asm     ; Complete dishwasher assembly program (216 instructions)
│   └── dishwasher_controller.hex     ; Compiled 16-bit hexadecimal machine code
└── docs/
    ├── Project_Report_CSE360.md      ; Complete academic documentation (Markdown)
    ├── Project_Report_CSE360.html    ; Printable A4 formatted report (Ctrl+P -> PDF)
    ├── Explain_Like_Im_5.md          ; Intuitive 5-year-old concept breakdown
    ├── Explain_Like_Im_5.html        ; Printable web version of the ELI5 guide
    ├── ISA_Specification.md          ; Instruction set & opcode reference
    └── Architecture_Diagrams.md      ; Micro-architecture and datapath schematics
```

## 5.2 Pure C Simulation Engine & Micro-Architecture Simulator
- **`aquacore8.h` / `aquacore8.c`**: Implements the fetch-decode-execute cycle. Contains array registers `R[0..3]`, program counter `pc`, condition flags `flag_z, flag_c, flag_n, flag_v`, and a 16-level hardware return stack `stack[0..15]`. Includes a full instruction disassembler and `.hex` machine code loader.
- **`Memory Subsystem`**: Emulates separate ROM (256 $\times$ 16-bit) and RAM (256 $\times$ 8-bit) spaces.
- **`I/O Peripheral Port Map`**: Exposes 32 peripheral address ports with named constant masks for safety interlocks, sensor readings, and actuator drivers.

## 5.3 Firmware Assembly Architecture & Machine Code Generation
The dishwasher controller firmware (`dishwasher_controller.asm`) is assembled into 16-bit hex machine code:
- **Pass 1 (Symbol Discovery)**: Resolves labels (e.g., `FILL_WATER:`, `MAINWASH_LOOP:`), constant directives (`.EQU`), and tracks instruction addresses.
- **Pass 2 (Code Generation)**: Maps mnemonics to 4-bit opcodes, resolves register numbers ($R_0 \dots R_3$), calculates jump target addresses from the symbol table, and emits 16-bit hex machine code (`dishwasher_controller.hex`).

## 5.4 Dishwasher Cyber-Physical Plant Simulation
The `dishwasher_plant.c` module models the physical appliance:
- Fluid volume fills smoothly at $+2\text{ L}$ per tick during inlet actuation and drains at $-2\text{ L}$ per tick during pump actuation.
- Thermodynamic water temperature rises smoothly by $+0.1^\circ\text{C}$ per tick when the heating element is active with water present.
- Turbidity increases as soil dissolves from plates during spray pump circulation and clears during draining.
- Dry-burn protection: If the heater is engaged without water, a dry burn hazard alarm is asserted.

## 5.5 Interactive Terminal Dashboard & Batch Cycle Engine
The `main.c` module provides:
- **Real-Time Cycle Monitor**: Real-time ASCII dashboard displaying $R_0, R_1, R_2, R_3$, $PC$, Flags $[Z, C, N, V]$, the current dissembled instruction, live sensor readings, and actuator lamps.
- **Interactive Shell**: Allows users to step through cycles, open/close the door, change modes (Eco, Normal, Heavy), and inject overheat faults.
- **Automated Batch Runner**: Executes the complete 1,254-cycle wash trace to stdout with milestone dashboards.

## 5.6 Hardware Synthesizable Verilog RTL Implementation
The Verilog modules (`cpu_top.v`, `alu.v`, `control_unit.v`, `datapath.v`, `register_file.v`, `io_controller.v`) provide a fully synthesizable implementation of AquaCore-8, ready for synthesis with standard EDA tools such as Xilinx Vivado or Intel Quartus.

## 5.7 System Requirements
- **Operating System**: Microsoft Windows 10 / 11 (64-bit), Linux, or macOS.
- **Compiler**: GCC (GNU Compiler Collection), Clang, or TCC (Tiny C Compiler). C99 compliant.
- **Hardware Resources**: Minimum 64 MB RAM, 20 MB free disk space.
- **Verilog Simulator (Optional for HDL)**: ModelSim, Icarus Verilog, or Vivado.

---

# 6. Debugging and Test-Run

## 6.1 Testing Methodology
A two-tier verification strategy was employed in the native C test harness (`src/tests.c`):
1. **Instruction-Level Unit Testing**: Every opcode and ALU function was isolated and tested against known arithmetic, logic, and flag boundary values (20 unit tests).
2. **System-Level Cyber-Physical Integration Testing**: The complete compiled assembly program (`dishwasher_controller.asm`) was executed against simulated physical hardware to verify cycle progression, dynamic turbidity rinse extension, and safety fault recovery (17 integration tests).

## 6.2 CPU Architecture Unit Tests (Instruction Level)
The test harness executed 20 automated unit tests with **100% pass rate**:

```text
==================================================
   AquaCore-8 ISA & CPU Architecture Unit Tests   
==================================================

[PASS] ADD R2 = 10 + 25                       (Expected: 0x23, Got: 0x23)
[PASS] SUB R3 = 25 - 10                       (Expected: 0x0F, Got: 0x0F)
[PASS] Zero Flag is false                           
[PASS] Negative Flag is false                       
[PASS] ADD 0xFF + 1 = 0x00 (8-bit)            (Expected: 0x00, Got: 0x00)
[PASS] Zero Flag set on zero result                 
[PASS] Carry Flag set on 8-bit carry                
[PASS] AND R2 = 0xAC & 0xF0                   (Expected: 0xA0, Got: 0xA0)
[PASS] OR  R3 = 0xAC | 0xF0                   (Expected: 0xFC, Got: 0xFC)
[PASS] SHL R1 = 0x04 << 1                     (Expected: 0x08, Got: 0x08)
[PASS] SHR R2 = 0x04 >> 1                     (Expected: 0x02, Got: 0x02)
[PASS] ADDI & SUBI: (10 + 5 - 3) = 12         (Expected: 0x0C, Got: 0x0C)
[PASS] STORE to RAM[0x50] & LOAD to R1        (Expected: 0x42, Got: 0x42)
[PASS] Direct RAM check at 0x50               (Expected: 0x42, Got: 0x42)
[PASS] IN from Sensor Temp (55 + 5 = 60)      (Expected: 0x3C, Got: 0x3C)
[PASS] OUT to Actuator Port                   (Expected: 0x3C, Got: 0x3C)
[PASS] Countdown loop finished at 0           (Expected: 0x00, Got: 0x00)
[PASS] Loop exited and halted at PC=4               
[PASS] Subroutine CALL (10 + 20) and return (+ 2) = 32 (Expected: 0x20, Got: 0x20)
[PASS] Return stack pointer is back to 0      (Expected: 0x00, Got: 0x00)
==================================================
Unit Test Summary: 20 Passed, 0 Failed.
>>> ALL CPU ARCHITECTURE UNIT TESTS PASSED! <<<
```

## 6.3 End-to-End Dishwasher Integration Cycles
`src/tests.c` verified the complete operational sequence across 17 test cases:

```
==========================================================
   AquaCore-8 Dishwasher Full Integration Verification   
==========================================================

--- [Test 1] Complete Normal Wash Cycle Execution ---
[PASS] Phase 1: Pre-Wash phase executed             
[PASS] Phase 2: Main Wash phase executed            
[PASS] Detergent dispenser triggered during main wash
[PASS] Water reached target temperature (>= 55°C)   
[PASS] Phase 3: Rinse phase executed                
[PASS] Phase 4: Heated Drying phase executed        
[PASS] Phase 5: Cycle Complete LED asserted         
[PASS] CPU halted cleanly after buzzer notification 
[PASS] Dishes cleaned (remaining soil < 10%)        

--- [Test 2] Eco Wash Mode Execution (Target Temp 45°C) ---
[PASS] Eco Mode reached target temp around 45°C     
[PASS] Eco Mode finished successfully               
```

## 6.4 Safety Interlock & Fault Injection Testing

### 1. Door Ajar Interlock Test
Midway through the high-pressure wash cycle, the door switch was pulled low (`doorClosed = false`):
- **Verification Result**: The CPU detected `PORT_SYS_IN & 0x02 == 0` within 1 clock cycle, de-asserted all actuators (`PORT_ACTUATORS = 0x00`), saved the active LED state, and asserted `LED_DOOR_WARN`.
- Upon re-closing the door (`doorClosed = true`), the CPU automatically resumed spray circulation from the exact point of interruption without losing state.

### 2. Overheat Emergency Thermal Trip Test
A thermal surge ($78^\circ\text{C}$) exceeding the safety limit ($75^\circ\text{C}$) was injected:
- **Verification Result**: The CPU executed `SAFETY_CHECK`, detected $T > 75^\circ\text{C}$, jumped to `OVERHEAT_FAULT`, cut all power to the heating element, asserted `LED_FAULT` and the alarm buzzer, and permanently halted the machine with error code `0xAA` on `PORT_PROGRESS`.

```
--- [Test 3] Door Open Emergency Safety Interlock ---
  -> Injecting Fault: User opens dishwasher door mid-wash!
[PASS] All physical actuators shut down immediately (ACT = 0)
[PASS] Door open warning LED is illuminated         
  -> User closes door again; verifying seamless cycle resumption...
[PASS] Actuators resumed active washing after door closure

--- [Test 4] Overheat Safety Emergency Trip ---
  -> Injecting Fault: Water temperature surges past safe limit (78°C)!
[PASS] CPU halted immediately on thermal limit breach
[PASS] Fault LED illuminated                        
[PASS] Progress register displays Overheat Error Code 0xAA
==========================================================
Integration Test Results: 17 Passed, 0 Failed.
>>> ALL INTEGRATION AND SAFETY TESTS PASSED! <<<
```

## 6.5 Execution Trace Logs
During the automated batch cycle, the simulator captured transitions across all cycle stages:

```
+---------------------------------------------------------------------------------+
| CPU STATE:  PC: 0x8D | Next Instruction: [0x6080] LDI  R0, #0x80            |
| REGISTERS:  R0: 0x05 (  5) | R1: 0x00 (  0) | R2: 0x00 (  0) | R3: 0x00 (  0)   |
| FLAGS:      [Z=1  C=0  N=0  V=0]   SP: 0    Total Cycles: 1246       Halted: false |
+---------------------------------------------------------------------------------+
| CYCLE STAGE: 5. Complete     | Progress Port: 0x05 | Mode: Normal Wash (55°C)   |
| SENSORS:    Door: CLOSED | Water Level:   0/128 | Temp: 51.8 °C | Turbidity: 0   |
| ACTUATORS:  Inlet: [ ] | Drain: [ ] | Spray: [ ] | Heater: [ ] | Fan: [ ] |
|             DetDisp: [ ] | RinseDisp: [ ] | Buzzer: [ ]                        |
| STATUS LED: [CYCLE COMPLETE!] 
+---------------------------------------------------------------------------------+
Batch simulation finished in 1254 CPU clock cycles. Halted: true.
```

---

# 7. Results Analysis

## 7.1 Algorithmic Time and Space Complexity
- **Time Complexity**: The dishwasher control program executes sequential loops bounded by hardware timer constants:
  $$T_{total} = T_{prewash} + T_{mainwash} + k \cdot T_{rinse} + T_{drying}$$
  Where $k \in \{1, 2\}$ is dynamically set by the turbidity sensor. Each subroutine executes in $O(N)$ cycles relative to countdown ticks. Overall algorithmic time complexity is strictly linear: $O(N)$.
- **Space Complexity**: The assembly program stores state in fixed RAM addresses (`0x20` to `0x25`). Auxiliary space complexity is $O(1)$ (strictly constant memory footprint).

## 7.2 Microprocessor Cycle Performance & Instruction Profile
- **Clocks Per Instruction (CPI)**:
  Every instruction in AquaCore-8 executes in a single clock cycle ($CPI = 1.0$), ensuring deterministic response times.
- **Cycle Count**:
  The full automated wash cycle (Normal Mode) executes in **1,254 clock cycles**.
- At a modest $10\text{ MHz}$ embedded clock rate, 1,254 cycles require only $125.4\mu\text{s}$ of raw execution time, leaving over $99.9\%$ of processor bandwidth available for low-power sleep modes.

## 7.3 Memory Footprint & Resource Efficiency
- **Program Memory Utilization**: The complete controller program occupies **216 words** out of the 256 available ROM words ($84.3\%$ code density).
- **Data Memory Utilization**: Global cycle variables occupy **6 bytes** out of 256 RAM bytes ($2.3\%$ RAM utilization).
- **Hardware Return Stack**: Stack depth peaked at **1 level** during subroutine calls (`FILL_WATER`, `DRAIN_WATER`, `SAFETY_CHECK`), well within the 16-level hardware stack capacity.

## 7.4 Robustness & Failsafe Guarantees
1. **Safety Loop Execution Frequency**: The `SAFETY_CHECK` subroutine is evaluated once every loop iteration (every 15 to 20 clock cycles). At $10\text{ MHz}$, this yields an emergency sensor reaction time of under **$2\mu\text{s}$**, far outperforming commercial appliance microcontrollers.
2. **Deterministic State Recovery**: By preserving active actuator states in `RAM_CURR_ACT` and display states in `RAM_SAVED_LED`, the controller guarantees zero state loss across temporary pauses.

---

# 8. Conclusion and Future Improvements

## 8.1 Project Summary
This mini-project successfully designed, verified, and demonstrated a minimal 8-bit application-specific microprocessor architecture (**AquaCore-8**) tailored for home appliance automation. 
Key deliverables achieved:
1. Complete 16-bit orthogonal ISA and single-cycle datapath.
2. Hardwired control unit with full Boolean derivation and truth tables.
3. Assembly firmware generator and disassembler.
4. Intelligent automated dishwasher control program (`dishwasher_controller.asm`) supporting adaptive multi-rinse and dynamic thermal regulation.
5. Concurrent cyber-physical plant simulation with sensor feedback.
6. Pure C simulation suite and automated CLI regression suite with **100% test pass rate** (37/37 tests passing).
7. Synthesizable Verilog HDL RTL implementation.

## 8.2 Scope Limitations
- Pipelining was omitted in favor of a simpler, verified single-cycle architecture.
- Hardware interrupts (vectored ISR) were not implemented; polling-based safety checking was utilized instead.
- Multiplication and division hardware were omitted to keep silicon area minimal.

## 8.3 Future Improvements
1. **Hardware Interrupt Controller**: Incorporate edge-triggered interrupt lines for instantaneous door-ajar and leak detection without polling overhead.
2. **Serial Communication Interfaces (UART / I2C / SPI)**: Add hardware peripheral controllers to interface digital temperature sensors and communicate with smart home IoT networks.
3. **Pipelined Datapath**: Upgrade to a 3-stage pipeline (Fetch, Decode/Execute, Writeback) with hazard forwarding to achieve higher clock frequencies.
4. **Sleep & Low-Power Modes**: Introduce power gating (`SLEEP` instruction) to reduce standby power consumption to under $1\text{ mW}$.

## 8.4 Applications of the Proposed Architecture
The minimalist architectural principles of AquaCore-8 are directly applicable to:
- Smart washing machines and cloth dryers.
- Automated microwave ovens and air fryers.
- HVAC thermostats and temperature regulators.
- Industrial fluid chemical batching controllers.

---

# 9. Bibliography

1. **D. A. Patterson and J. L. Hennessy**, *Computer Organization and Design: The Hardware/Software Interface*, 5th ed. Waltham, MA: Morgan Kaufmann Publishers, 2014.
2. **M. M. Mano and M. D. Ciletti**, *Digital Design: With an Introduction to the Verilog HDL, VHDL, and SystemVerilog*, 6th ed. Upper Saddle River, NJ: Pearson, 2017.
3. **A. S. Tanenbaum and T. Austin**, *Structured Computer Organization*, 6th ed. Boston, MA: Pearson, 2012.
4. **IEEE Standard for Verilog Hardware Description Language**, *IEEE Std 1364-2005*, IEEE Computer Society, 2006.
5. **Microchip Technology Inc.**, *AVR Microcontroller Architecture and Instruction Set Manual*, Document DS40002198B, 2021.
6. **Whirlpool Corporation**, *Dishwasher Sensing and Adaptive Wash Cycle Control Systems*, U.S. Patent 7,445,014, Nov. 2008.
7. **J. Fraden**, *Handbook of Modern Sensors: Physics, Designs, and Applications*, 5th ed. New York: Springer, 2016.
