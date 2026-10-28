/**
 * ==============================================================================
 * Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
 * Course:       CSE360 Computer Architecture
 * Institution:  East West University (EWU)
 * Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
 *               Pulok Akibuzzaman (ID: 2023-3-60-051)
 *               Md. Jakaria Masud (ID: 2023-3-60-386)
 * File:         main.c - Interactive CLI Dashboard & Batch Execution Engine
 * ==============================================================================
 */

#include "aquacore8.h"
#include "dishwasher_plant.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void run_unit_tests(void);
void run_integration_tests(const char *hex_file);

static void print_dashboard(const AquaCore8 *cpu, const DishwasherPlant *plant) {
    uint8_t act = cpu->io_ports[PORT_ACTUATORS & 0x1F];
    uint8_t leds = cpu->io_ports[PORT_STATUS_LEDS & 0x1F];
    uint8_t progress = cpu->io_ports[PORT_PROGRESS & 0x1F];

    char dis[64];
    cpu_disassemble(cpu->rom[cpu->pc], dis, sizeof(dis));

    const char *phase_name = "Standby";
    if (leds & LED_FAULT) phase_name = "FAULT / HALT";
    else if (leds & LED_DOOR_WARN) phase_name = "DOOR AJAR INTERLOCK";
    else if (progress == 1) phase_name = "1. Pre-Wash";
    else if (progress == 2) phase_name = "2. Main Wash";
    else if (progress == 3) phase_name = "3. Rinse";
    else if (progress == 4) phase_name = "4. Heated Drying";
    else if (progress == 5) phase_name = "5. Complete";

    const char *mode_names[] = {"Normal Wash (55C)", "Eco Wash (45C)", "Heavy Wash (65C)", "Custom"};
    const char *mode_str = (plant->selected_mode <= 2) ? mode_names[plant->selected_mode] : mode_names[3];

    printf("\n+---------------------------------------------------------------------------------+\n");
    printf("| CPU STATE:  PC: 0x%02X | Next Instruction: [0x%04X] %-25s |\n",
           cpu->pc, cpu->rom[cpu->pc], dis);
    printf("| REGISTERS:  R0: 0x%02X (%3d) | R1: 0x%02X (%3d) | R2: 0x%02X (%3d) | R3: 0x%02X (%3d)   |\n",
           cpu->R[0], cpu->R[0], cpu->R[1], cpu->R[1],
           cpu->R[2], cpu->R[2], cpu->R[3], cpu->R[3]);
    printf("| FLAGS:      [Z=%d  C=%d  N=%d  V=%d]   SP: %-2d   Total Cycles: %-10llu Halted: %-5s |\n",
           cpu->flag_z ? 1 : 0, cpu->flag_c ? 1 : 0,
           cpu->flag_n ? 1 : 0, cpu->flag_v ? 1 : 0,
           cpu->sp, (unsigned long long)cpu->total_cycles, cpu->halted ? "true" : "false");
    printf("+---------------------------------------------------------------------------------+\n");
    printf("| CYCLE STAGE: %-15s | Progress Port: 0x%02X | Mode: %-15s   |\n",
           phase_name, progress, mode_str);
    printf("| SENSORS:    Door: %-6s | Water Level: %3d/128 | Temp: %4.1f C | Turbidity: %-3d |\n",
           plant->door_closed ? "CLOSED" : "AJAR!!",
           plant->water_level, plant->water_temp, plant->turbidity);
    printf("| ACTUATORS:  Inlet: [%c] | Drain: [%c] | Spray: [%c] | Heater: [%c] | Fan: [%c] |\n",
           (act & ACT_INLET_VALVE) ? 'X' : ' ',
           (act & ACT_DRAIN_PUMP) ? 'X' : ' ',
           (act & ACT_SPRAY_PUMP) ? 'X' : ' ',
           (act & ACT_HEATER) ? 'X' : ' ',
           (act & ACT_DRY_FAN) ? 'X' : ' ');
    printf("|             DetDisp: [%c] | RinseDisp: [%c] | Buzzer: [%c]                        |\n",
           (act & ACT_DET_DISP) ? 'X' : ' ',
           (act & ACT_RINSE_DISP) ? 'X' : ' ',
           (act & ACT_BUZZER) ? 'X' : ' ');
    printf("| STATUS LED: %s%s%s%s%s%s%s%s\n",
           (leds & LED_STANDBY) ? "[STANDBY] " : "",
           (leds & LED_PREWASH) ? "[PRE-WASH] " : "",
           (leds & LED_MAINWASH) ? "[MAIN WASH] " : "",
           (leds & LED_RINSE) ? "[RINSE] " : "",
           (leds & LED_DRYING) ? "[DRYING] " : "",
           (leds & LED_COMPLETE) ? "[CYCLE COMPLETE!] " : "",
           (leds & LED_DOOR_WARN) ? "[DOOR AJAR ALERT!] " : "",
           (leds & LED_FAULT) ? "[SYSTEM FAULT!] " : "");
    printf("+---------------------------------------------------------------------------------+\n");
}

static void run_batch(AquaCore8 *cpu, DishwasherPlant *plant) {
    printf("Running automatic batch simulation (Normal Cycle)...\n");
    plant->door_closed = true;
    plant->selected_mode = 0; // Normal
    plant->start_button_pressed = true;
    plant_sync_to_io(plant, cpu);

    int cycle = 0;
    int last_progress = -1;

    while (!cpu->halted && cycle < 15000) {
        cpu_step(cpu);
        plant_step(plant, cpu);
        cycle++;

        if (cycle == 20) {
            plant->start_button_pressed = false;
            plant_sync_to_io(plant, cpu);
        }

        int progress = cpu->io_ports[PORT_PROGRESS & 0x1F];
        if (progress != last_progress) {
            last_progress = progress;
            print_dashboard(cpu, plant);
        }
    }

    printf("\nBatch simulation finished in %d CPU clock cycles.\n", cycle);
    print_dashboard(cpu, plant);
}

int main(int argc, char *argv[]) {
    const char *hex_file = "programs/dishwasher_controller.hex";

    // Fallback search path
    FILE *fp = fopen(hex_file, "r");
    if (!fp) {
        hex_file = "F:/EWU/CSE360/Mini-Project/programs/dishwasher_controller.hex";
        fp = fopen(hex_file, "r");
    }
    if (fp) fclose(fp);

    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        run_unit_tests();
        run_integration_tests(hex_file);
        return 0;
    }

    AquaCore8 cpu;
    DishwasherPlant plant;
    cpu_init(&cpu);

    if (!cpu_load_hex_file(&cpu, hex_file)) {
        printf("Error: Could not load machine hex file from %s\n", hex_file);
        return 1;
    }

    plant_init(&plant, &cpu);

    if (argc > 1 && strcmp(argv[1], "--batch") == 0) {
        run_batch(&cpu, &plant);
        return 0;
    }

    // Interactive Mode
    printf("=======================================================================\n");
    printf("      AquaCore-8 Dishwasher CPU Architecture C Simulator (CLI)         \n");
    printf("=======================================================================\n");
    printf("Commands:\n");
    printf("  start              - Press Start Button\n");
    printf("  mode <0-2>         - Set Mode (0=Normal, 1=Eco, 2=Heavy)\n");
    printf("  door open / close  - Toggle door position (safety interlock)\n");
    printf("  temp <celsius>     - Inject water temperature sensor value\n");
    printf("  step [n]           - Step CPU and environment n cycles (default: 1)\n");
    printf("  run [n]            - Run n cycles (default: 500)\n");
    printf("  status             - Display current system dashboard\n");
    printf("  quit / exit        - Exit simulator\n\n");

    print_dashboard(&cpu, &plant);

    char line[128];
    while (1) {
        printf("\nAquaCore8-C> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        char cmd[32] = {0}, arg[32] = {0};
        sscanf(line, "%31s %31s", cmd, arg);

        if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "status") == 0) {
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "start") == 0) {
            plant.start_button_pressed = true;
            plant_sync_to_io(&plant, &cpu);
            printf("Start button pressed! Stepping 20 cycles...\n");
            for (int i = 0; i < 20; i++) {
                cpu_step(&cpu);
                plant_step(&plant, &cpu);
            }
            plant.start_button_pressed = false;
            plant_sync_to_io(&plant, &cpu);
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "mode") == 0) {
            int m = atoi(arg);
            plant.selected_mode = m;
            plant_sync_to_io(&plant, &cpu);
            printf("Mode set to %d\n", m);
        } else if (strcmp(cmd, "door") == 0) {
            if (strcmp(arg, "open") == 0) {
                plant.door_closed = false;
                printf("[INTERLOCK] Dishwasher door OPENED!\n");
            } else {
                plant.door_closed = true;
                printf("[INTERLOCK] Dishwasher door CLOSED.\n");
            }
            plant_sync_to_io(&plant, &cpu);
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "temp") == 0) {
            double t = atof(arg);
            plant.water_temp = t;
            plant_sync_to_io(&plant, &cpu);
            printf("Injected water temperature: %.1f C\n", t);
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "step") == 0) {
            int steps = (strlen(arg) > 0) ? atoi(arg) : 1;
            for (int i = 0; i < steps; i++) {
                if (cpu.halted) {
                    printf("CPU is HALTED.\n");
                    break;
                }
                cpu_step(&cpu);
                plant_step(&plant, &cpu);
            }
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "run") == 0) {
            int cycles = (strlen(arg) > 0) ? atoi(arg) : 500;
            int ran = 0;
            while (ran < cycles && !cpu.halted) {
                cpu_step(&cpu);
                plant_step(&plant, &cpu);
                ran++;
            }
            printf("Executed %d cycles. CPU Halted: %s\n", ran, cpu.halted ? "true" : "false");
            print_dashboard(&cpu, &plant);
        } else {
            printf("Unknown command: %s. Type 'status', 'start', 'step', 'run', or 'quit'.\n", cmd);
        }
    }

    return 0;
}
