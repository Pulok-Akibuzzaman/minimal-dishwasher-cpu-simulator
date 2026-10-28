/**
 * ==============================================================================
 * Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
 * Course:       CSE360 Computer Architecture
 * Institution:  East West University (EWU)
 * Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
 *               Pulok Akibuzzaman (ID: 2023-3-60-051)
 *               Md. Jakaria Masud (ID: 2023-3-60-386)
 * File:         aquacore8.c - CPU Execution Core, ALU Logic & Machine Code Disassembler
 * ==============================================================================
 */

#include "aquacore8.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

void cpu_init(AquaCore8 *cpu) {
    memset(cpu, 0, sizeof(AquaCore8));
    cpu_reset(cpu);
}

void cpu_reset(AquaCore8 *cpu) {
    for (int i = 0; i < 4; i++) cpu->R[i] = 0;
    cpu->pc = 0;
    cpu->flag_z = false;
    cpu->flag_c = false;
    cpu->flag_n = false;
    cpu->flag_v = false;
    cpu->sp = 0;
    cpu->halted = false;
    cpu->total_cycles = 0;
    memset(cpu->ram, 0, sizeof(cpu->ram));
    memset(cpu->io_ports, 0, sizeof(cpu->io_ports));

    // Default safe initial sensor state
    cpu->io_ports[PORT_SYS_IN] = SYS_IN_DOOR_CLS;
    cpu->io_ports[PORT_WATER_LVL] = 0;
    cpu->io_ports[PORT_WATER_TEMP] = 20;
    cpu->io_ports[PORT_TURBIDITY] = 0;
}

static uint8_t alu_add(AquaCore8 *cpu, uint8_t a, uint8_t b) {
    uint16_t sum = (uint16_t)a + (uint16_t)b;
    uint8_t res = (uint8_t)(sum & 0xFF);
    cpu->flag_z = (res == 0);
    cpu->flag_c = (sum > 0xFF);
    cpu->flag_n = (res & 0x80) != 0;
    cpu->flag_v = (((a ^ res) & (b ^ res) & 0x80) != 0);
    return res;
}

static uint8_t alu_sub(AquaCore8 *cpu, uint8_t a, uint8_t b) {
    int diff = (int)a - (int)b;
    uint8_t res = (uint8_t)(diff & 0xFF);
    cpu->flag_z = (res == 0);
    cpu->flag_c = (a < b); // Borrow
    cpu->flag_n = (res & 0x80) != 0;
    cpu->flag_v = (((a ^ b) & (a ^ res) & 0x80) != 0);
    return res;
}

static void alu_logic_flags(AquaCore8 *cpu, uint8_t res) {
    cpu->flag_z = (res == 0);
    cpu->flag_c = false;
    cpu->flag_n = (res & 0x80) != 0;
    cpu->flag_v = false;
}

static bool eval_condition(const AquaCore8 *cpu, uint8_t cond) {
    switch (cond) {
        case COND_JZ:  return cpu->flag_z;
        case COND_JNZ: return !cpu->flag_z;
        case COND_JC:  return cpu->flag_c;
        case COND_JNC: return !cpu->flag_c;
        case COND_JN:  return cpu->flag_n;
        case COND_JP:  return !cpu->flag_n;
        case COND_JV:  return cpu->flag_v;
        case COND_JGT: return (!cpu->flag_z) && (cpu->flag_n == cpu->flag_v);
        case COND_JLT: return (cpu->flag_n != cpu->flag_v);
        case COND_JGE: return (cpu->flag_n == cpu->flag_v);
        case COND_JLE: return cpu->flag_z || (cpu->flag_n != cpu->flag_v);
        default: return false;
    }
}

bool cpu_step(AquaCore8 *cpu) {
    if (cpu->halted) return false;

    // 1. Fetch
    uint16_t raw = cpu->rom[cpu->pc];
    uint8_t next_pc = (cpu->pc + 1) & 0xFF;

    // 2. Decode
    uint8_t opcode = (raw >> 12) & 0x0F;
    uint8_t rd     = (raw >> 10) & 0x03;
    uint8_t rs     = (raw >> 8)  & 0x03;
    uint8_t rt     = (raw >> 6)  & 0x03;
    uint8_t funct  = (raw >> 3)  & 0x07;
    uint8_t cond   = (raw >> 8)  & 0x0F;
    uint8_t imm8   = raw & 0xFF;

    // 3. Execute
    switch (opcode) {
        case OP_RTYPE:
            switch (funct) {
                case FUNCT_ADD:
                    cpu->R[rd] = alu_add(cpu, cpu->R[rs], cpu->R[rt]);
                    break;
                case FUNCT_SUB:
                    cpu->R[rd] = alu_sub(cpu, cpu->R[rs], cpu->R[rt]);
                    break;
                case FUNCT_AND:
                    cpu->R[rd] = cpu->R[rs] & cpu->R[rt];
                    alu_logic_flags(cpu, cpu->R[rd]);
                    break;
                case FUNCT_OR:
                    cpu->R[rd] = cpu->R[rs] | cpu->R[rt];
                    alu_logic_flags(cpu, cpu->R[rd]);
                    break;
                case FUNCT_XOR:
                    cpu->R[rd] = cpu->R[rs] ^ cpu->R[rt];
                    alu_logic_flags(cpu, cpu->R[rd]);
                    break;
                case FUNCT_CMP:
                    alu_sub(cpu, cpu->R[rs], cpu->R[rt]); // Flags updated, no register write
                    break;
                case FUNCT_NOT:
                    cpu->R[rd] = ~cpu->R[rs];
                    alu_logic_flags(cpu, cpu->R[rd]);
                    break;
                case FUNCT_SHF: {
                    bool is_shr = (raw & 1) == 1;
                    uint8_t val = cpu->R[rs];
                    uint8_t res;
                    if (is_shr) {
                        cpu->flag_c = (val & 0x01) != 0;
                        res = (val >> 1) & 0x7F;
                    } else {
                        cpu->flag_c = (val & 0x80) != 0;
                        res = (val << 1) & 0xFF;
                    }
                    cpu->R[rd] = res;
                    cpu->flag_z = (res == 0);
                    cpu->flag_n = (res & 0x80) != 0;
                    cpu->flag_v = false;
                    break;
                }
                default: break;
            }
            cpu->pc = next_pc;
            break;

        case OP_ADDI:
            cpu->R[rd] = alu_add(cpu, cpu->R[rd], imm8);
            cpu->pc = next_pc;
            break;

        case OP_SUBI:
            cpu->R[rd] = alu_sub(cpu, cpu->R[rd], imm8);
            cpu->pc = next_pc;
            break;

        case OP_CMPI:
            alu_sub(cpu, cpu->R[rd], imm8);
            cpu->pc = next_pc;
            break;

        case OP_ANDI:
            cpu->R[rd] = cpu->R[rd] & imm8;
            alu_logic_flags(cpu, cpu->R[rd]);
            cpu->pc = next_pc;
            break;

        case OP_ORI:
            cpu->R[rd] = cpu->R[rd] | imm8;
            alu_logic_flags(cpu, cpu->R[rd]);
            cpu->pc = next_pc;
            break;

        case OP_LDI:
            cpu->R[rd] = imm8;
            cpu->pc = next_pc;
            break;

        case OP_LOAD:
            cpu->R[rd] = cpu->ram[imm8];
            cpu->pc = next_pc;
            break;

        case OP_STORE:
            cpu->ram[imm8] = cpu->R[rd];
            cpu->pc = next_pc;
            break;

        case OP_IN:
            cpu->R[rd] = cpu->io_ports[imm8 & 0x1F];
            cpu->pc = next_pc;
            break;

        case OP_OUT:
            cpu->io_ports[imm8 & 0x1F] = cpu->R[rd];
            cpu->pc = next_pc;
            break;

        case OP_BRANCH:
            if (eval_condition(cpu, cond)) {
                cpu->pc = imm8;
            } else {
                cpu->pc = next_pc;
            }
            break;

        case OP_JMP:
            cpu->pc = imm8;
            break;

        case OP_CALL: {
            bool is_ret = ((raw >> 11) & 1) == 1;
            if (is_ret) {
                if (cpu->sp > 0) {
                    cpu->sp--;
                    cpu->pc = cpu->stack[cpu->sp];
                } else {
                    cpu->halted = true;
                }
            } else {
                if (cpu->sp < STACK_DEPTH) {
                    cpu->stack[cpu->sp] = next_pc;
                    cpu->sp++;
                    cpu->pc = imm8;
                } else {
                    cpu->halted = true;
                }
            }
            break;
        }

        case OP_WAIT:
            cpu->pc = next_pc;
            break;

        case OP_SYS:
            if (cond == SYS_HALT) {
                cpu->halted = true;
            }
            cpu->pc = next_pc;
            break;

        default:
            cpu->pc = next_pc;
            break;
    }

    cpu->total_cycles++;
    return !cpu->halted;
}

bool cpu_load_hex_file(AquaCore8 *cpu, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    memset(cpu->rom, 0, sizeof(cpu->rom));
    char line[128];
    int addr = 0;

    while (fgets(line, sizeof(line), fp) && addr < ROM_SIZE) {
        // Strip whitespace
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\r' || *p == '\n' || *p == ';') continue;

        unsigned int word = 0;
        if (sscanf(p, "%x", &word) == 1) {
            cpu->rom[addr++] = (uint16_t)(word & 0xFFFF);
        }
    }

    fclose(fp);
    return (addr > 0);
}

void cpu_disassemble(uint16_t raw, char *buf, size_t bufsize) {
    uint8_t opcode = (raw >> 12) & 0x0F;
    uint8_t rd     = (raw >> 10) & 0x03;
    uint8_t rs     = (raw >> 8)  & 0x03;
    uint8_t rt     = (raw >> 6)  & 0x03;
    uint8_t funct  = (raw >> 3)  & 0x07;
    uint8_t cond   = (raw >> 8)  & 0x0F;
    uint8_t imm8   = raw & 0xFF;

    switch (opcode) {
        case OP_RTYPE:
            switch (funct) {
                case FUNCT_ADD: snprintf(buf, bufsize, "ADD R%d, R%d, R%d", rd, rs, rt); break;
                case FUNCT_SUB: snprintf(buf, bufsize, "SUB R%d, R%d, R%d", rd, rs, rt); break;
                case FUNCT_AND: snprintf(buf, bufsize, "AND R%d, R%d, R%d", rd, rs, rt); break;
                case FUNCT_OR:  snprintf(buf, bufsize, "OR  R%d, R%d, R%d", rd, rs, rt); break;
                case FUNCT_XOR: snprintf(buf, bufsize, "XOR R%d, R%d, R%d", rd, rs, rt); break;
                case FUNCT_CMP: snprintf(buf, bufsize, "CMP R%d, R%d", rs, rt); break;
                case FUNCT_NOT: snprintf(buf, bufsize, "NOT R%d, R%d", rd, rs); break;
                case FUNCT_SHF:
                    if (raw & 1) snprintf(buf, bufsize, "SHR R%d, R%d", rd, rs);
                    else         snprintf(buf, bufsize, "SHL R%d, R%d", rd, rs);
                    break;
                default: snprintf(buf, bufsize, "UNK_R 0x%04X", raw); break;
            }
            break;
        case OP_ADDI:  snprintf(buf, bufsize, "ADDI R%d, #0x%02X", rd, imm8); break;
        case OP_SUBI:  snprintf(buf, bufsize, "SUBI R%d, #0x%02X", rd, imm8); break;
        case OP_CMPI:  snprintf(buf, bufsize, "CMPI R%d, #0x%02X", rd, imm8); break;
        case OP_ANDI:  snprintf(buf, bufsize, "ANDI R%d, #0x%02X", rd, imm8); break;
        case OP_ORI:   snprintf(buf, bufsize, "ORI  R%d, #0x%02X", rd, imm8); break;
        case OP_LDI:   snprintf(buf, bufsize, "LDI  R%d, #0x%02X", rd, imm8); break;
        case OP_LOAD:  snprintf(buf, bufsize, "LOAD R%d, [0x%02X]", rd, imm8); break;
        case OP_STORE: snprintf(buf, bufsize, "STORE [0x%02X], R%d", imm8, rd); break;
        case OP_IN:    snprintf(buf, bufsize, "IN   R%d, [0x%02X]", rd, imm8); break;
        case OP_OUT:   snprintf(buf, bufsize, "OUT  [0x%02X], R%d", imm8, rd); break;
        case OP_BRANCH: {
            const char *cnames[] = {"JZ", "JNZ", "JC", "JNC", "JN", "JP", "JV", "JGT", "JLT", "JGE", "JLE"};
            const char *cn = (cond <= COND_JLE) ? cnames[cond] : "J??";
            snprintf(buf, bufsize, "%s 0x%02X", cn, imm8);
            break;
        }
        case OP_JMP:   snprintf(buf, bufsize, "JMP 0x%02X", imm8); break;
        case OP_CALL:
            if ((raw >> 11) & 1) snprintf(buf, bufsize, "RET");
            else snprintf(buf, bufsize, "CALL 0x%02X", imm8);
            break;
        case OP_WAIT:  snprintf(buf, bufsize, "WAIT #%d", imm8); break;
        case OP_SYS:
            if (cond == SYS_HALT) snprintf(buf, bufsize, "HALT");
            else                  snprintf(buf, bufsize, "NOP");
            break;
        default:
            snprintf(buf, bufsize, "RAW 0x%04X", raw);
            break;
    }
}

// ==============================================================================
// Dishwasher Cyber-Physical Plant Simulation
// ==============================================================================

void plant_init(DishwasherPlant *p, AquaCore8 *cpu) {
    p->door_closed = true;
    p->start_button_pressed = false;
    p->selected_mode = 0;
    p->water_level = 0;
    p->water_temp = 20.0;
    p->turbidity = 0;
    p->dish_soil_remaining = 80;
    p->detergent_released = false;
    p->rinse_aid_released = false;
    p->leak_detected = false;
    p->dry_burn_alarm = false;

    plant_sync_to_io(p, cpu);
}

void plant_step(DishwasherPlant *p, AquaCore8 *cpu) {
    uint8_t act = cpu->io_ports[PORT_ACTUATORS & 0x1F];

    bool inlet_valve   = (act & ACT_INLET_VALVE) != 0;
    bool drain_pump    = (act & ACT_DRAIN_PUMP) != 0;
    bool spray_pump    = (act & ACT_SPRAY_PUMP) != 0;
    bool heater        = (act & ACT_HEATER) != 0;
    bool det_dispenser = (act & ACT_DET_DISP) != 0;
    bool rinse_disp    = (act & ACT_RINSE_DISP) != 0;
    bool dry_fan       = (act & ACT_DRY_FAN) != 0;

    // 1. Fluid Dynamics
    if (inlet_valve && !drain_pump) {
        if (p->water_level + 2 <= WATER_LEVEL_MAX) p->water_level += 2;
        else p->water_level = WATER_LEVEL_MAX;
    } else if (drain_pump && !inlet_valve) {
        if (p->water_level - 2 >= WATER_LEVEL_EMPTY) p->water_level -= 2;
        else p->water_level = WATER_LEVEL_EMPTY;
    }

    // 2. Thermodynamic Temperature Physics
    if (heater) {
        if (p->water_level >= WATER_LEVEL_LOW) {
            if (p->water_temp + 0.1 <= 70.0) p->water_temp += 0.1;
            else p->water_temp = 70.0;
        } else if (dry_fan) {
            // Heated air drying
            if (p->water_temp + 0.1 <= 55.0) p->water_temp += 0.1;
            else p->water_temp = 55.0;
        } else {
            // Dry burn condition (heater with no water or fan)
            p->dry_burn_alarm = true;
            if (p->water_temp + 1.0 <= 95.0) p->water_temp += 1.0;
            else p->water_temp = 95.0;
        }
    } else {
        // Natural cooling
        if (p->water_temp > 20.0) {
            p->water_temp -= 0.05;
            if (p->water_temp < 20.0) p->water_temp = 20.0;
        }
    }

    // 3. Washing & Turbidity Dynamics
    if (det_dispenser) p->detergent_released = true;
    if (rinse_disp) p->rinse_aid_released = true;

    if (spray_pump && p->water_level >= WATER_LEVEL_LOW) {
        if (p->dish_soil_remaining > 0) {
            int rate = p->detergent_released ? 2 : 1;
            int cleaned = (p->dish_soil_remaining < rate) ? p->dish_soil_remaining : rate;
            p->dish_soil_remaining -= cleaned;
            if (p->turbidity + cleaned <= 90) p->turbidity += cleaned;
            else p->turbidity = 90;
        }
    }

    // Draining water clears suspended turbidity
    if (drain_pump && p->water_level <= WATER_LEVEL_LOW) {
        if (p->turbidity - 10 >= 0) p->turbidity -= 10;
        else p->turbidity = 0;
    }

    // 4. Update CPU sensor input registers
    plant_sync_to_io(p, cpu);
}

void plant_sync_to_io(DishwasherPlant *p, AquaCore8 *cpu) {
    // Port 0x00: System inputs
    uint8_t sys_in = 0;
    if (p->start_button_pressed) sys_in |= SYS_IN_START;
    if (p->door_closed)         sys_in |= SYS_IN_DOOR_CLS;
    sys_in |= (uint8_t)((p->selected_mode & 0x0F) << 4);
    cpu->io_ports[PORT_SYS_IN & 0x1F] = sys_in;

    // Port 0x01: Water level
    cpu->io_ports[PORT_WATER_LVL & 0x1F] = (uint8_t)(p->water_level & 0xFF);

    // Port 0x02: Water temperature
    cpu->io_ports[PORT_WATER_TEMP & 0x1F] = (uint8_t)lround(p->water_temp);

    // Port 0x03: Turbidity
    cpu->io_ports[PORT_TURBIDITY & 0x1F] = (uint8_t)(p->turbidity & 0xFF);

    // Port 0x04: Aux sensor (leak)
    uint8_t aux = 0;
    if (p->leak_detected) aux |= (1 << 3);
    cpu->io_ports[PORT_AUX_SENS & 0x1F] = aux;
}
