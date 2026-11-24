/*
 * EAST WEST UNIVERSITY - Department of CSE
 * Course: CSE360 Computer Architecture | Fall 2026
 * Project: Suggest and design a minimal CPU architecture, including ISA,
 *          datapath, and control unit, for automating dishwasher control
 *          functions with sensor inputs, and verify it with a simulator.
 * Students: Nusrat Rahman Aurna (2023-3-60-057), Pulok Akibuzzaman (2023-3-60-051)
 *
 * Supervisor: Dr. Nawab Yousuf Ali
 *
 * Build: gcc -std=c99 -Wall dishwasher.c -o dishwasher
 * Run:   ./dishwasher [start_temp] [start_level] [door_closed]
 *        ./dishwasher            defaults: 20 C, 0 L, door closed
 *        ./dishwasher 20 0 0     door open: CPU must hold at PC 0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_CYCLES 100
#define IDLE (-1)

/* ---- 1. ISA: 5 instructions, each = 3-bit opcode + 6-bit operand ---- */
enum Opcode   { READ, SET, CMP, JNE, HALT };            /* opcodes         */
enum Sensor   { DOOR, LEVEL, TEMP };                    /* sensor ports    */
enum Actuator { INLET, HEATER, DRAIN, FAN, BUZZER };    /* actuator ports  */

/* Bit-field packing: shrinks each instruction from 8 bytes to 2 bytes */
typedef struct {
    uint16_t op  : 3;     /* 3-bit opcode  (0-7)              */
    uint16_t arg : 6;     /* 6-bit operand (0-63)             */
} Instr;

/* ---- 2. PROGRAM IN ROM (21 instructions, read-only memory) ---- */
static const Instr rom[] = {
    {READ, DOOR},  {CMP, 1},  {JNE, 0},                   /* 0-2   wait for door closed */
    {SET, INLET},  {READ, LEVEL}, {CMP, 50}, {JNE, 4},    /* 3-6   fill to 50 L         */
    {SET, HEATER}, {READ, TEMP},  {CMP, 55}, {JNE, 8},    /* 7-10  heat to 55 C         */
    {SET, DRAIN},  {READ, LEVEL}, {CMP, 0},  {JNE, 12},   /* 11-14 drain to 0 L         */
    {SET, FAN},    {READ, TEMP},  {CMP, 30}, {JNE, 16},   /* 15-18 cool to 30 C         */
    {SET, BUZZER}, {HALT, 0}                              /* 19-20 buzz and halt        */
};
#define ROM_SIZE ((int)(sizeof(rom) / sizeof(rom[0])))

/* ---- 3. DATAPATH REGISTERS AND PHYSICAL STATE (8-bit minimal widths) ---- */
int8_t  pc     = 0;       /* program counter                  */
uint8_t acc    = 0;       /* accumulator (data register)      */
uint8_t zf     = 0;       /* zero flag                        */
uint8_t halted = 0;       /* set by HALT                      */
int8_t  device = IDLE;    /* active actuator port             */

uint8_t door   = 1;       /* sensor: 1 = closed, 0 = open     */
uint8_t level  = 0;       /* sensor: water level in liters    */
uint8_t temp   = 20;      /* sensor: water temperature in C   */

static const char * const device_names[] = {
    "Water inlet (fill)", "Heater + pump (wash)", "Drain pump (drain)",
    "Drying fan (dry)", "Buzzer (done)"
};

/* ---- 4. CONTROL UNIT: fetch, decode, execute one instruction ---- */
/* Returns 0 if the PC points outside ROM (fault). */
int cpu_step(void) {
    if (pc < 0 || pc >= ROM_SIZE) return 0;

    Instr i = rom[pc++];                    /* fetch and advance PC */
    switch (i.op) {                         /* decode and execute   */
        case READ:
            switch (i.arg) {
                case DOOR:  acc = door;  break;
                case LEVEL: acc = level; break;
                case TEMP:  acc = temp;  break;
            }
            break;
        case SET:  device = (int8_t)i.arg;      break;
        case CMP:  zf = (acc == i.arg);         break;
        case JNE:  if (!zf) pc = (int8_t)i.arg; break;
        case HALT: halted = 1;                  break;
    }
    return 1;
}

/* ---- 5. PLANT MODEL: active device changes the environment each clock ---- */
void plant(void) {
    switch (device) {
        case INLET:  if (level < 50) level += 10; break;
        case HEATER: if (temp  < 55) temp  += 5;  break;
        case DRAIN:  if (level > 0)  level -= 10; break;
        case FAN:    if (temp  > 30) temp  -= 5;  break;
        default:     break;
    }
}

/* ---- 6. DRIVER ---- */
int main(int argc, char *argv[]) {
    int start_temp  = 20;
    int start_level = 0;
    int start_door  = 1;

    if (argc > 1) start_temp  = atoi(argv[1]);
    if (argc > 2) start_level = atoi(argv[2]);
    if (argc > 3) start_door  = atoi(argv[3]);

    /* Temp and level must be reachable in the step sizes, or the
       equality checks in the ROM never match and the run cannot finish. */
    if (start_temp < 15 || start_temp > 55 || start_temp % 5 != 0 ||
        start_level < 0 || start_level > 50 || start_level % 10 != 0 ||
        (start_door != 0 && start_door != 1)) {
        printf("Invalid input. Temp: 15-55 C in steps of 5. Level: 0-50 L in steps of 10. Door: 0 or 1.\n");
        return 1;
    }

    temp  = (uint8_t)start_temp;
    level = (uint8_t)start_level;
    door  = (uint8_t)start_door;

    printf("Start: temp=%dC  level=%dL  door=%s\n\n", temp, level, door ? "closed" : "open");
    printf("Cycle  PC  Level   Temp  Device\n");

    int cycles = 0;
    int fault = 0;

    while (!halted && cycles < MAX_CYCLES) {
        if (!cpu_step()) {
            fault = 1;
            break;
        }
        plant();
        cycles++;
        printf("%5d  %2d  %4dL  %4dC  %s\n", cycles, pc, level, temp,
               device >= 0 ? device_names[device] : "Idle");
    }

    if (halted)
        printf("\nSUCCESS: dishwasher cycle complete in %d clock cycles.\n", cycles);
    else if (fault)
        printf("\nFAULT: PC %d is outside ROM (missing HALT?).\n", pc);
    else
        printf("\nTIMEOUT: did not halt within %d cycles (CPU waiting or stuck).\n", MAX_CYCLES);

    return halted ? 0 : 1;
}