# AquaCore-8: Micro-Architecture & System Diagrams

---

## 1. High-Level Processor Datapath & Control Architecture

```mermaid
flowchart TD
    subgraph FetchStage ["1. Instruction Fetch Unit"]
        PC["Program Counter (PC)<br>8-bit Register"] -->|Address [7:0]| ROM["Instruction ROM<br>256 x 16-bit"]
        PC -->|PC| Inc["+1 Adder"]
        Inc -->|PC + 1| PCMux["Next PC Mux"]
        PCMux -->|New PC| PC
    end

    subgraph ControlStage ["2. Instruction Decode & Control"]
        ROM -->|Opcode [15:12]| CU["Hardwired Control Unit"]
        ROM -->|Funct [5:3]| CU
        ROM -->|Cond [11:8]| CU
        CU -->|RegWrite| RegFile
        CU -->|ALUSrc| ALUMux
        CU -->|ALUOp| ALU
        CU -->|MemRead/Write| RAM
        CU -->|IORead/Write| IO
        CU -->|Branch/Jump| PCMux
    end

    subgraph ExecuteStage ["3. Execution & Register File"]
        ROM -->|Rd [11:10]| RegFile["Register File<br>4 x 8-bit (R0..R3)"]
        ROM -->|Rs [9:8]| RegFile
        ROM -->|Rt [7:6]| RegFile
        RegFile -->|Read Data 1| ALU["8-Bit Arithmetic Logic Unit"]
        RegFile -->|Read Data 2| ALUMux["ALUSrc MUX"]
        ROM -->|Immediate [7:0]| ALUMux
        ALUMux -->|Operand B| ALU
        ALU -->|Result [7:0]| WBMux["Writeback MUX"]
        ALU -->|Flags [Z, C, N, V]| CU
    end

    subgraph SubroutineStack ["4. Subroutine Call Stack"]
        Inc -->|Return Addr (PC+1)| Stack["Hardware Return Stack<br>16 x 8-bit"]
        Stack -->|Pop PC (RET)| PCMux
        ROM -->|Target Addr (CALL)| PCMux
    end

    subgraph MemoryIOStage ["5. Memory & Peripheral I/O"]
        ALU -->|Address [7:0]| RAM["Data Memory (RAM)<br>256 x 8-bit"]
        RegFile -->|Write Data| RAM
        RAM -->|Read Data| WBMux
        ALU -->|Port Address [7:0]| IO["I/O Port Controller<br>32 Peripheral Registers"]
        RegFile -->|Write Data| IO
        IO -->|Read Data| WBMux
        WBMux -->|Writeback Data| RegFile
    end
```

---

## 2. Cyber-Physical Dishwasher Closed-Loop Automation

```mermaid
flowchart LR
    subgraph Sensors ["Sensors (Inputs to Port 0x00 - 0x04)"]
        Door["Door Safety Switch<br>(0x00 Bit 1)"]
        Lvl["Water Level Transducer<br>(0x01)"]
        Temp["NTC Thermistor Temp<br>(0x02)"]
        Turb["Optical Turbidity Sensor<br>(0x03)"]
        Leak["Tub Leak Detector<br>(0x04 Bit 3)"]
    end

    subgraph Core ["AquaCore-8 ASIP"]
        CPU["AquaCore-8 CPU<br>Single-Cycle Engine<br>10 MHz"]
        FSM["Firmware State Machine<br>(dishwasher_controller.asm)"]
        CPU <--> FSM
    end

    subgraph Actuators ["Actuators (Outputs from Port 0x10 - 0x12)"]
        Inlet["Water Inlet Valve"]
        Drain["Drain Discharge Pump"]
        Spray["Wash Spray Circulation Pump"]
        Heater["1800W Heating Element"]
        DetDisp["Detergent Solenoid"]
        RinseDisp["Rinse-Aid Solenoid"]
        Fan["Air Blower Fan"]
        Buzzer["Alarm Buzzer"]
        LEDs["Status Panel LEDs"]
    end

    Door --> Core
    Lvl --> Core
    Temp --> Core
    Turb --> Core
    Leak --> Core

    Core --> Inlet
    Core --> Drain
    Core --> Spray
    Core --> Heater
    Core --> DetDisp
    Core --> RinseDisp
    Core --> Fan
    Core --> Buzzer
    Core --> LEDs
```

---

## 3. Dishwasher Cycle Control State Diagram

```mermaid
stateDiagram-v2
    [*] --> Standby: Power On / Reset

    Standby --> Standby: Door Open OR Start not pressed
    Standby --> ModeConfig: Start Pressed & Door Closed

    state ModeConfig {
        [*] --> ReadMode
        ReadMode --> Normal: Mode = 0 (55°C, 15 ticks)
        ReadMode --> Eco: Mode = 1 (45°C, 10 ticks)
        ReadMode --> Heavy: Mode = 2 (65°C, 22 ticks)
    }

    ModeConfig --> PreWash: Target Parameters Stored in RAM

    state PreWash {
        [*] --> FillTubPre
        FillTubPre --> CirculatePre: Water Level Reached
        CirculatePre --> DrainPre: 6 Ticks Complete
        DrainPre --> [*]: Level Empty
    }

    PreWash --> MainWash

    state MainWash {
        [*] --> FillTubMain
        FillTubMain --> DispenseDetergent: Full Level Reached
        DispenseDetergent --> WashAndHeat: Solenoid Fired
        WashAndHeat --> CheckTurbidity: Target Temp & Countdown Complete
        CheckTurbidity --> DrainMain: Dynamic Extra Rinse if Soiled
        DrainMain --> [*]: Level Empty
    }

    MainWash --> RinseCycle

    state RinseCycle {
        [*] --> FillTubRinse
        FillTubRinse --> DispenseRinseAid
        DispenseRinseAid --> CirculateRinse
        CirculateRinse --> DrainRinse
        DrainRinse --> CheckRinseCount
        CheckRinseCount --> FillTubRinse: Rinse Count > 0
        CheckRinseCount --> [*]: All Rinses Done
    }

    RinseCycle --> HeatedDrying

    state HeatedDrying {
        [*] --> FanAndHeaterOn
        FanAndHeaterOn --> CoolDown: 10 Ticks Complete
        CoolDown --> [*]: Actuators Off
    }

    HeatedDrying --> Complete

    state Complete {
        [*] --> BeepChime
        BeepChime --> HaltState
    }

    state EmergencyFault {
        [*] --> ShutOffAllActuators
        ShutOffAllActuators --> AssertFaultLED
        AssertFaultLED --> PermanentHalt
    }

    PreWash --> EmergencyFault: Temp > 75°C OR Leak
    MainWash --> EmergencyFault: Temp > 75°C OR Leak
    RinseCycle --> EmergencyFault: Temp > 75°C OR Leak
    HeatedDrying --> EmergencyFault: Temp > 75°C OR Leak
```
