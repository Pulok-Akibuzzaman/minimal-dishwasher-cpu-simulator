/**
 * ==============================================================================
 * AquaCore-8: Minimal 8-Bit CPU Architecture for Dishwasher Automation
 * Course: CSE360 Computer Architecture | East West University (EWU)
 * Authors: Nusrat Rahman Aurna (2023-3-60-057),
 *          Pulok Akibuzzaman (2023-3-60-051),
 *          Md. Jakaria Masud (2023-3-60-386)
 * ==============================================================================
 */

#ifndef AQUACORE8_H
#define AQUACORE8_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// --- Primary Opcodes (4-bit: bits [15:12]) ---
#define OP_RTYPE   0x0
#define OP_ADDI    0x1
#define OP_SUBI    0x2
#define OP_CMPI    0x3
#define OP_ANDI    0x4
#define OP_ORI     0x5
#define OP_LDI     0x6
#define OP_LOAD    0x7
#define OP_STORE   0x8
#define OP_IN      0x9
#define OP_OUT     0xA
#define OP_BRANCH  0xB
#define OP_JMP     0xC
#define OP_CALL    0xD
#define OP_WAIT    0xE
#define OP_SYS     0xF

// --- R-Type Funct Codes (3-bit: bits [5:3]) ---
#define FUNCT_ADD  0x0
#define FUNCT_SUB  0x1
#define FUNCT_AND  0x2
#define FUNCT_OR   0x3
#define FUNCT_XOR  0x4
#define FUNCT_CMP  0x5
#define FUNCT_NOT  0x6
#define FUNCT_SHF  0x7

// --- Branch Conditions (bits [11:8] when opcode == OP_BRANCH) ---
#define COND_JZ    0x0
#define COND_JNZ   0x1
#define COND_JC    0x2
#define COND_JNC   0x3
#define COND_JN    0x4
#define COND_JP    0x5
#define COND_JV    0x6
#define COND_JGT   0x7
#define COND_JLT   0x8
#define COND_JGE   0x9
#define COND_JLE   0xA

// --- System Sub-Opcodes ---
#define SYS_NOP    0x0
#define SYS_HALT   0x1

// --- Port-Mapped I/O Registers ---
#define PORT_SYS_IN      0x00
#define PORT_WATER_LVL   0x01
#define PORT_WATER_TEMP  0x02
#define PORT_TURBIDITY   0x03
#define PORT_AUX_SENS    0x04

#define PORT_ACTUATORS   0x10
#define PORT_STATUS_LEDS 0x11
#define PORT_PROGRESS    0x12
#define PORT_DEBUG       0x13

// --- Actuator Bitmasks (Port 0x10) ---
#define ACT_OFF          0x00
#define ACT_INLET_VALVE  (1 << 0) // 0x01
#define ACT_DRAIN_PUMP   (1 << 1) // 0x02
#define ACT_SPRAY_PUMP   (1 << 2) // 0x04
#define ACT_HEATER       (1 << 3) // 0x08
#define ACT_DET_DISP     (1 << 4) // 0x10
#define ACT_RINSE_DISP   (1 << 5) // 0x20
#define ACT_DRY_FAN      (1 << 6) // 0x40
#define ACT_BUZZER       (1 << 7) // 0x80

// --- Status LED Bitmasks (Port 0x11) ---
#define LED_STANDBY      (1 << 0) // 0x01
#define LED_PREWASH      (1 << 1) // 0x02
#define LED_MAINWASH     (1 << 2) // 0x04
#define LED_RINSE        (1 << 3) // 0x08
#define LED_DRYING       (1 << 4) // 0x10
#define LED_COMPLETE     (1 << 5) // 0x20
#define LED_DOOR_WARN    (1 << 6) // 0x40
#define LED_FAULT        (1 << 7) // 0x80

// --- System Input Bitmasks (Port 0x00) ---
#define SYS_IN_START     (1 << 0) // 0x01
#define SYS_IN_DOOR_CLS  (1 << 1) // 0x02

// --- Memory Space Bounds ---
#define ROM_SIZE         256
#define RAM_SIZE         256
#define IO_PORTS_SIZE    32
#define STACK_DEPTH      16

// --- CPU State Structure ---
typedef struct {
    uint8_t  R[4];                // 4 General-Purpose 8-bit Registers (R0..R3)
    uint8_t  pc;                  // 8-bit Program Counter
    bool     flag_z;              // Zero Flag
    bool     flag_c;              // Carry / Borrow Flag
    bool     flag_n;              // Negative Flag
    bool     flag_v;              // Signed Overflow Flag
    uint8_t  stack[STACK_DEPTH];  // 16-level hardware return stack for CALL/RET
    uint8_t  sp;                  // Stack Pointer
    uint16_t rom[ROM_SIZE];       // 256 x 16-bit Instruction Memory (Harvard ROM)
    uint8_t  ram[RAM_SIZE];       // 256 x 8-bit Data Memory (Harvard RAM)
    uint8_t  io_ports[IO_PORTS_SIZE]; // Port-Mapped I/O Registers
    bool     halted;              // CPU Clock Halt Flag
    uint64_t total_cycles;        // Execution Cycle Counter
} AquaCore8;

// Function Prototypes
void cpu_init(AquaCore8 *cpu);
void cpu_reset(AquaCore8 *cpu);
bool cpu_step(AquaCore8 *cpu);
bool cpu_load_hex_file(AquaCore8 *cpu, const char *filepath);
void cpu_disassemble(uint16_t instr, char *buf, size_t bufsize);

#endif // AQUACORE8_H
