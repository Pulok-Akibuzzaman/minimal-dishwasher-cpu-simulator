/**
 * ==============================================================================
 * Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
 * Course:       CSE360 Computer Architecture
 * Institution:  East West University (EWU)
 * Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057, Email: 2023-3-60-057@std.ewubd.edu)
 *               Pulok Akibuzzaman   (ID: 2023-3-60-051, Email: 2023-3-60-051@std.ewubd.edu)
 *               Md. Jakaria Masud   (ID: 2023-3-60-386, Email: 2023-3-60-386@std.ewubd.edu)
 * File:         aquacore8.h - The CPU Blueprint & Definitions (Beginner-Friendly)
 * ==============================================================================
 * 
 * 💡 WHAT IS THIS FILE?
 * This header file is the "dictionary" and "blueprint" of our custom computer brain.
 * It defines the secret language (opcodes), the registers (pockets), and the I/O ports
 * (wires connected to water sensors and motors).
 */

#ifndef AQUACORE8_H
#define AQUACORE8_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ==============================================================================
// 1. PRIMARY OPCODES (The 4-bit commands the CPU understands: bits [15:12])
// ==============================================================================
#define OP_RTYPE   0x0  // Register-to-register math (ADD, SUB, AND, OR, etc.)
#define OP_ADDI    0x1  // Add immediate constant:      R[d] = R[d] + number
#define OP_SUBI    0x2  // Subtract immediate constant: R[d] = R[d] - number
#define OP_CMPI    0x3  // Compare with constant:       Compare R[d] with number
#define OP_ANDI    0x4  // Bitwise AND with constant
#define OP_ORI     0x5  // Bitwise OR with constant
#define OP_LDI     0x6  // Load constant into register: R[d] = number
#define OP_LOAD    0x7  // Load from RAM memory:        R[d] = RAM[address]
#define OP_STORE   0x8  // Save to RAM memory:          RAM[address] = R[d]
#define OP_IN      0x9  // Read sensor port:            R[d] = SensorPort[port]
#define OP_OUT     0xA  // Write actuator port:         ActuatorPort[port] = R[d]
#define OP_BRANCH  0xB  // Conditional jump (e.g., jump if zero, jump if greater)
#define OP_JMP     0xC  // Unconditional direct jump
#define OP_CALL    0xD  // Call subroutine (pushes return address to stack) / RET
#define OP_WAIT    0xE  // Wait idle cycles
#define OP_SYS     0xF  // System control: HALT or NOP

// ==============================================================================
// 2. ALU SUB-FUNCTIONS (Used when Opcode == OP_RTYPE: bits [5:3])
// ==============================================================================
#define FUNCT_ADD  0x0  // Addition:       R[d] = R[s] + R[t]
#define FUNCT_SUB  0x1  // Subtraction:    R[d] = R[s] - R[t]
#define FUNCT_AND  0x2  // Bitwise AND:    R[d] = R[s] & R[t]
#define FUNCT_OR   0x3  // Bitwise OR:     R[d] = R[s] | R[t]
#define FUNCT_XOR  0x4  // Bitwise XOR:    R[d] = R[s] ^ R[t]
#define FUNCT_CMP  0x5  // Compare:        Set flags based on (R[s] - R[t])
#define FUNCT_NOT  0x6  // Bitwise Invert: R[d] = ~R[s]
#define FUNCT_SHF  0x7  // Bit shift:      SHL (left << 1) or SHR (right >> 1)

// ==============================================================================
// 3. BRANCH CONDITIONS (Used when Opcode == OP_BRANCH: bits [11:8])
// ==============================================================================
#define COND_JZ    0x0  // Jump if Zero (answer was 0)
#define COND_JNZ   0x1  // Jump if Not Zero (answer was not 0)
#define COND_JC    0x2  // Jump if Carry (number overflowed 8 bits)
#define COND_JNC   0x3  // Jump if No Carry
#define COND_JN    0x4  // Jump if Negative
#define COND_JP    0x5  // Jump if Positive
#define COND_JV    0x6  // Jump if Signed Overflow
#define COND_JGT   0x7  // Jump if Greater Than
#define COND_JLT   0x8  // Jump if Less Than
#define COND_JGE   0x9  // Jump if Greater or Equal
#define COND_JLE   0xA  // Jump if Less or Equal

// System Sub-Opcodes
#define SYS_NOP    0x0  // Do nothing for 1 cycle
#define SYS_HALT   0x1  // Stop the CPU clock completely

// ==============================================================================
// 4. PORT-MAPPED I/O REGISTERS (The wires connected to the dishwasher)
// ==============================================================================
// Input Ports (Sensors) - Read using "IN Rd, [port]"
#define PORT_SYS_IN      0x00 // Front panel: Start button, Door sensor, Wash Mode
#define PORT_WATER_LVL   0x01 // Water level in tub (0 to 128)
#define PORT_WATER_TEMP  0x02 // Water temperature in degrees Celsius (°C)
#define PORT_TURBIDITY   0x03 // Optical soil sensor (0=clean, >40=dirty sauce)
#define PORT_AUX_SENS    0x04 // Auxiliary sensors (puddle leak detector)

// Output Ports (Actuators) - Controlled using "OUT [port], Rd"
#define PORT_ACTUATORS   0x10 // Motors, pumps, valves, heater switches
#define PORT_STATUS_LEDS 0x11 // Front panel LED lights
#define PORT_PROGRESS    0x12 // Cycle progress indicator (1..5)
#define PORT_DEBUG       0x13 // Debugging register

// Actuator Bitmasks (for PORT_ACTUATORS = 0x10)
#define ACT_OFF          0x00
#define ACT_INLET_VALVE  (1 << 0) // Bit 0: Fresh water tap valve
#define ACT_DRAIN_PUMP   (1 << 1) // Bit 1: Dirty water drain pump
#define ACT_SPRAY_PUMP   (1 << 2) // Bit 2: High-pressure spray wash pump
#define ACT_HEATER       (1 << 3) // Bit 3: 1800W electric heating element
#define ACT_DET_DISP     (1 << 4) // Bit 4: Detergent soap tablet door
#define ACT_RINSE_DISP   (1 << 5) // Bit 5: Rinse-aid shine dispenser
#define ACT_DRY_FAN      (1 << 6) // Bit 6: Warm air drying blower fan
#define ACT_BUZZER       (1 << 7) // Bit 7: Celebration buzzer alarm (beep!)

// Status LED Bitmasks (for PORT_STATUS_LEDS = 0x11)
#define LED_STANDBY      (1 << 0) // Bit 0: Waiting for Start
#define LED_PREWASH      (1 << 1) // Bit 1: Pre-Wash in progress
#define LED_MAINWASH     (1 << 2) // Bit 2: Main Wash in progress
#define LED_RINSE        (1 << 3) // Bit 3: Rinse cycle in progress
#define LED_DRYING       (1 << 4) // Bit 4: Heated Drying in progress
#define LED_COMPLETE     (1 << 5) // Bit 5: Green "Dishes Ready!" LED
#define LED_DOOR_WARN    (1 << 6) // Bit 6: Flashing Red Door Ajar Warning
#define LED_FAULT        (1 << 7) // Bit 7: Red Emergency Fault Alarm LED

// Front-Panel Buttons (for PORT_SYS_IN = 0x00)
#define SYS_IN_START     (1 << 0) // Bit 0: User pressed Start button
#define SYS_IN_DOOR_CLS  (1 << 1) // Bit 1: Door is closed tight (1=safe, 0=open)

// ==============================================================================
// 5. MEMORY SIZES
// ==============================================================================
#define ROM_SIZE         256  // 256 instruction words (16-bit each)
#define RAM_SIZE         256  // 256 bytes data memory (8-bit each)
#define IO_PORTS_SIZE    32   // 32 hardware I/O ports
#define STACK_DEPTH      16   // 16 nested function calls supported

// ==============================================================================
// 6. THE CPU STATE STRUCTURE (The whole processor in RAM)
// ==============================================================================
typedef struct {
    uint8_t  R[4];                // 4 General-Purpose 8-bit Registers (R0, R1, R2, R3)
    uint8_t  pc;                  // 8-bit Program Counter (points to current instruction)
    bool     flag_z;              // Zero Flag (1 if last math result was 0)
    bool     flag_c;              // Carry Flag (1 if 8-bit math overflowed > 255)
    bool     flag_n;              // Negative Flag (1 if answer has high sign bit set)
    bool     flag_v;              // Overflow Flag (1 if signed numbers wrapped around)
    uint8_t  stack[STACK_DEPTH];  // 16-level hardware return stack for CALL / RET
    uint8_t  sp;                  // Stack Pointer (depth of nested calls)
    uint16_t rom[ROM_SIZE];       // 256 x 16-bit Harvard Instruction Memory (ROM)
    uint8_t  ram[RAM_SIZE];       // 256 x 8-bit Harvard Data Scratchpad (RAM)
    uint8_t  io_ports[IO_PORTS_SIZE]; // Port-Mapped I/O registers
    bool     halted;              // Has the CPU halted? (true = sleep/done)
    uint64_t total_cycles;        // Total clock cycles elapsed
} AquaCore8;

// ==============================================================================
// 7. THE PHYSICAL DISHWASHER PLANT STRUCTURE (The simulated machine)
// ==============================================================================
#define WATER_LEVEL_EMPTY 0x00
#define WATER_LEVEL_LOW   0x10
#define WATER_LEVEL_FULL  0x50
#define WATER_LEVEL_MAX   0x80

typedef struct {
    bool   door_closed;          // Is the door shut? (true=safe, false=ajar)
    bool   start_button_pressed;  // Did user push Start?
    int    selected_mode;         // 0=Normal (55°C), 1=Eco (45°C), 2=Heavy (65°C)
    int    water_level;           // Tub fluid volume (0 to 128)
    double water_temp;            // Tub water temperature in Celsius (°C)
    int    turbidity;             // Optical cloudiness (0=clean, >40=dirty sauce)
    int    dish_soil_remaining;   // Plate dirtiness percentage (100% -> 0%)
    bool   detergent_released;    // Has soap tablet dropped?
    bool   rinse_aid_released;    // Has rinse shine liquid dropped?
    bool   leak_detected;         // Water puddle on kitchen floor?
    bool   dry_burn_alarm;        // Heater turned on with no water?
} DishwasherPlant;

// ==============================================================================
// 8. FUNCTION DECLARATIONS
// ==============================================================================
// CPU Functions
void cpu_init(AquaCore8 *cpu);
void cpu_reset(AquaCore8 *cpu);
bool cpu_step(AquaCore8 *cpu);
bool cpu_load_hex_file(AquaCore8 *cpu, const char *filepath);
void cpu_disassemble(uint16_t instr, char *buf, size_t bufsize);

// Dishwasher Physical Plant Functions
void plant_init(DishwasherPlant *p, AquaCore8 *cpu);
void plant_step(DishwasherPlant *p, AquaCore8 *cpu);
void plant_sync_to_io(DishwasherPlant *p, AquaCore8 *cpu);

#endif // AQUACORE8_H
