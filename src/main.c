/**
 * ==============================================================================
 * Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
 * Course:       CSE360 Computer Architecture
 * Institution:  East West University (EWU)
 * Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057, Email: 2023-3-60-057@std.ewubd.edu)
 *               Pulok Akibuzzaman   (ID: 2023-3-60-051, Email: 2023-3-60-051@std.ewubd.edu)
 *               Md. Jakaria Masud   (ID: 2023-3-60-386, Email: 2023-3-60-386@std.ewubd.edu)
 * File:         main.c - Main Entry Point, Interactive Monitor & Test Suite
 * ==============================================================================
 * 
 * 💡 WHAT IS THIS FILE?
 * This is the main driver program for our project. It does three things:
 *  1. [Tests]:       Runs 37 automated tests to verify the CPU and dishwasher.
 *  2. [Batch Trace]: Runs the full dishwasher cycle from start to finish.
 *  3. [Interactive]: Lets you control the dishwasher cycle-by-cycle like a game.
 */

#include "aquacore8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ==============================================================================
// PART 1: TEST ASSERTION HELPERS
// ==============================================================================
static int passed_count = 0;
static int failed_count = 0;

static void assert_eq(const char *name, int expected, int actual) {
    if (expected == actual) {
        passed_count++;
        printf("[PASS] %-38s (Expected: 0x%02X, Got: 0x%02X)\n", name, expected, actual);
    } else {
        failed_count++;
        printf("[FAIL] %-38s (Expected: 0x%02X, Got: 0x%02X)\n", name, expected, actual);
    }
}

static void assert_true(const char *name, bool condition) {
    if (condition) {
        passed_count++;
        printf("[PASS] %-45s\n", name);
    } else {
        failed_count++;
        printf("[FAIL] %-45s\n", name);
    }
}

// ==============================================================================
// PART 2: 20 INSTRUCTION-LEVEL UNIT TESTS (Tests the CPU ALU and Registers)
// ==============================================================================
void run_unit_tests(void) {
    printf("==================================================\n");
    printf("   AquaCore-8 ISA & CPU Architecture Unit Tests   \n");
    printf("==================================================\n\n");

    AquaCore8 cpu;
    cpu_init(&cpu);

    // Test 1: ADD and SUB operations
    // LDI R0, 10; LDI R1, 25; ADD R2, R0, R1; SUB R3, R1, R0; HALT
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 10;
    cpu.rom[1] = (OP_LDI << 12) | (1 << 10) | 25;
    cpu.rom[2] = (OP_RTYPE << 12) | (2 << 10) | (0 << 8) | (1 << 6) | (FUNCT_ADD << 3);
    cpu.rom[3] = (OP_RTYPE << 12) | (3 << 10) | (1 << 8) | (0 << 6) | (FUNCT_SUB << 3);
    cpu.rom[4] = (OP_SYS << 12) | (SYS_HALT << 8);

    while (cpu_step(&cpu)); // Run until HALT
    assert_eq("ADD R2 = 10 + 25", 35, cpu.R[2]);
    assert_eq("SUB R3 = 25 - 10", 15, cpu.R[3]);
    assert_true("Zero Flag is false", !cpu.flag_z);
    assert_true("Negative Flag is false", !cpu.flag_n);

    // Test 2: Flags on 8-bit overflow (255 + 1 = 0)
    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 0xFF;
    cpu.rom[1] = (OP_LDI << 12) | (1 << 10) | 0x01;
    cpu.rom[2] = (OP_RTYPE << 12) | (2 << 10) | (0 << 8) | (1 << 6) | (FUNCT_ADD << 3);
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    while (cpu_step(&cpu));
    assert_eq("ADD 0xFF + 1 = 0x00 (8-bit)", 0, cpu.R[2]);
    assert_true("Zero Flag set on zero result", cpu.flag_z);
    assert_true("Carry Flag set on 8-bit carry", cpu.flag_c);

    // Test 3: Bitwise Logic (AND, OR, SHL, SHR)
    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 0xAC;
    cpu.rom[1] = (OP_LDI << 12) | (1 << 10) | 0xF0;
    cpu.rom[2] = (OP_RTYPE << 12) | (2 << 10) | (0 << 8) | (1 << 6) | (FUNCT_AND << 3);
    cpu.rom[3] = (OP_RTYPE << 12) | (3 << 10) | (0 << 8) | (1 << 6) | (FUNCT_OR << 3);
    cpu.rom[4] = (OP_SYS << 12) | (SYS_HALT << 8);
    while (cpu_step(&cpu));
    assert_eq("AND R2 = 0xAC & 0xF0", 0xA0, cpu.R[2]);
    assert_eq("OR  R3 = 0xAC | 0xF0", 0xFC, cpu.R[3]);

    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 0x04;
    cpu.rom[1] = (OP_RTYPE << 12) | (1 << 10) | (0 << 8) | (FUNCT_SHF << 3) | 0; // SHL
    cpu.rom[2] = (OP_RTYPE << 12) | (2 << 10) | (0 << 8) | (FUNCT_SHF << 3) | 1; // SHR
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    while (cpu_step(&cpu));
    assert_eq("SHL R1 = 0x04 << 1", 0x08, cpu.R[1]);
    assert_eq("SHR R2 = 0x04 >> 1", 0x02, cpu.R[2]);

    // Test 4: Immediate Arithmetic (ADDI & SUBI)
    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 10;
    cpu.rom[1] = (OP_ADDI << 12) | (0 << 10) | 5;
    cpu.rom[2] = (OP_SUBI << 12) | (0 << 10) | 3;
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    while (cpu_step(&cpu));
    assert_eq("ADDI & SUBI: (10 + 5 - 3) = 12", 12, cpu.R[0]);

    // Test 5: RAM Store and Load
    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 0x42;
    cpu.rom[1] = (OP_STORE << 12) | (0 << 10) | 0x50;
    cpu.rom[2] = (OP_LOAD << 12) | (1 << 10) | 0x50;
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    while (cpu_step(&cpu));
    assert_eq("STORE to RAM[0x50] & LOAD to R1", 0x42, cpu.R[1]);
    assert_eq("Direct RAM check at 0x50", 0x42, cpu.ram[0x50]);

    // Test 6: Sensor and Actuator I/O Ports
    cpu_reset(&cpu);
    cpu.io_ports[PORT_WATER_TEMP] = 55;
    cpu.rom[0] = (OP_IN << 12) | (0 << 10) | PORT_WATER_TEMP;
    cpu.rom[1] = (OP_ADDI << 12) | (0 << 10) | 5;
    cpu.rom[2] = (OP_OUT << 12) | (0 << 10) | PORT_ACTUATORS;
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    while (cpu_step(&cpu));
    assert_eq("IN from Sensor Temp (55 + 5 = 60)", 60, cpu.R[0]);
    assert_eq("OUT to Actuator Port", 60, cpu.io_ports[PORT_ACTUATORS]);

    // Test 7: Branch Countdown Loop
    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 5;
    cpu.rom[1] = (OP_SUBI << 12) | (0 << 10) | 1;
    cpu.rom[2] = (OP_BRANCH << 12) | (COND_JNZ << 8) | 1; // Jump back to line 1 if R0 != 0
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    int limit = 100;
    while (cpu_step(&cpu) && --limit > 0);
    assert_eq("Countdown loop finished at 0", 0, cpu.R[0]);
    assert_true("Loop exited and halted at PC=4", cpu.pc == 4);

    // Test 8: Subroutine CALL & RET
    cpu_reset(&cpu);
    cpu.rom[0] = (OP_LDI << 12) | (0 << 10) | 10;
    cpu.rom[1] = (OP_CALL << 12) | (0 << 11) | 5; // Call subroutine at line 5
    cpu.rom[2] = (OP_ADDI << 12) | (0 << 10) | 2;
    cpu.rom[3] = (OP_SYS << 12) | (SYS_HALT << 8);
    cpu.rom[4] = (OP_SYS << 12) | (SYS_NOP << 8);
    cpu.rom[5] = (OP_ADDI << 12) | (0 << 10) | 20;
    cpu.rom[6] = (OP_CALL << 12) | (1 << 11); // RET instruction
    limit = 50;
    while (cpu_step(&cpu) && --limit > 0);
    assert_eq("Subroutine CALL (10 + 20) and return (+ 2) = 32", 32, cpu.R[0]);
    assert_eq("Return stack pointer is back to 0", 0, cpu.sp);

    printf("==================================================\n");
    printf("Unit Test Summary: %d Passed, %d Failed.\n\n", passed_count, failed_count);
}

// ==============================================================================
// PART 3: 17 FULL INTEGRATION & SAFETY TESTS (Tests the Dishwasher System)
// ==============================================================================
void run_integration_tests(const char *hex_file) {
    printf("==========================================================\n");
    printf("   AquaCore-8 Dishwasher Full Integration Verification   \n");
    printf("==========================================================\n\n");

    int start_p = passed_count;
    int start_f = failed_count;

    // --- TEST 1: Full Normal Wash Cycle Execution ---
    printf("--- [Test 1] Complete Normal Wash Cycle Execution ---\n");
    AquaCore8 cpu;
    DishwasherPlant plant;
    cpu_init(&cpu);

    if (!cpu_load_hex_file(&cpu, hex_file)) {
        printf("[FATAL] Could not load hex file from %s\n", hex_file);
        return;
    }

    plant_init(&plant, &cpu);
    plant.door_closed = true;
    plant.selected_mode = 0; // 0 = Normal Mode
    plant.start_button_pressed = true;
    plant_sync_to_io(&plant, &cpu);

    bool reached_prewash = false;
    bool reached_mainwash = false;
    bool det_dispensed = false;
    bool reached_temp = false;
    bool reached_rinse = false;
    bool reached_drying = false;
    bool reached_complete = false;

    int cycle = 0;
    while (cycle < 15000 && !cpu.halted) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        cycle++;

        uint8_t leds = cpu.io_ports[PORT_STATUS_LEDS & 0x1F];
        if (leds & LED_PREWASH)  reached_prewash = true;
        if (leds & LED_MAINWASH) reached_mainwash = true;
        if (leds & LED_RINSE)    reached_rinse = true;
        if (leds & LED_DRYING)   reached_drying = true;
        if (leds & LED_COMPLETE) reached_complete = true;

        if (plant.detergent_released) det_dispensed = true;
        if (plant.water_temp >= 54.0) reached_temp = true;

        if (cycle == 20) {
            plant.start_button_pressed = false;
            plant_sync_to_io(&plant, &cpu);
        }
    }

    assert_true("Phase 1: Pre-Wash phase executed", reached_prewash);
    assert_true("Phase 2: Main Wash phase executed", reached_mainwash);
    assert_true("Detergent dispenser triggered during main wash", det_dispensed);
    assert_true("Water reached target temperature (>= 54°C)", reached_temp);
    assert_true("Phase 3: Rinse phase executed", reached_rinse);
    assert_true("Phase 4: Heated Drying phase executed", reached_drying);
    assert_true("Phase 5: Cycle Complete LED asserted", reached_complete);
    assert_true("CPU halted cleanly after buzzer notification", cpu.halted);
    assert_true("Dishes cleaned (remaining soil < 10%)", plant.dish_soil_remaining < 10);

    // --- TEST 2: Eco Mode Wash Cycle ---
    printf("\n--- [Test 2] Eco Wash Mode Execution (Target Temp 45°C) ---\n");
    cpu_init(&cpu);
    cpu_load_hex_file(&cpu, hex_file);
    plant_init(&plant, &cpu);

    plant.door_closed = true;
    plant.selected_mode = 1; // 1 = Eco Mode
    plant.start_button_pressed = true;
    plant_sync_to_io(&plant, &cpu);

    cycle = 0;
    double max_wash_temp = 20.0;
    while (cycle < 12000 && !cpu.halted) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        cycle++;

        uint8_t leds = cpu.io_ports[PORT_STATUS_LEDS & 0x1F];
        if ((leds & LED_MAINWASH) && plant.water_temp > max_wash_temp) {
            max_wash_temp = plant.water_temp;
        }

        if (cycle == 20) {
            plant.start_button_pressed = false;
            plant_sync_to_io(&plant, &cpu);
        }
    }

    assert_true("Eco Mode reached target temp around 45°C", max_wash_temp >= 45.0 && max_wash_temp < 52.0);
    assert_true("Eco Mode finished successfully", (cpu.io_ports[PORT_STATUS_LEDS & 0x1F] & LED_COMPLETE) != 0);

    // --- TEST 3: Door Open Emergency Safety Interlock ---
    printf("\n--- [Test 3] Door Open Emergency Safety Interlock ---\n");
    cpu_init(&cpu);
    cpu_load_hex_file(&cpu, hex_file);
    plant_init(&plant, &cpu);

    plant.door_closed = true;
    plant.selected_mode = 0;
    plant.start_button_pressed = true;
    plant_sync_to_io(&plant, &cpu);

    // Run until spray pump is running in main wash
    cycle = 0;
    while (cycle < 5000) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        cycle++;
        if (cycle == 20) {
            plant.start_button_pressed = false;
            plant_sync_to_io(&plant, &cpu);
        }
        uint8_t leds = cpu.io_ports[PORT_STATUS_LEDS & 0x1F];
        uint8_t acts = cpu.io_ports[PORT_ACTUATORS & 0x1F];
        if ((leds & LED_MAINWASH) && (acts & ACT_SPRAY_PUMP)) break;
    }

    printf("  -> Injecting Fault: User opens dishwasher door mid-wash!\n");
    plant.door_closed = false;
    plant_sync_to_io(&plant, &cpu);

    for (int i = 0; i < 40; i++) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        if (cpu.io_ports[PORT_STATUS_LEDS & 0x1F] & LED_DOOR_WARN) break;
    }

    uint8_t acts_during_door = cpu.io_ports[PORT_ACTUATORS & 0x1F];
    uint8_t leds_during_door = cpu.io_ports[PORT_STATUS_LEDS & 0x1F];

    assert_true("All physical actuators shut down immediately (ACT = 0)", acts_during_door == 0);
    assert_true("Door open warning LED is illuminated", (leds_during_door & LED_DOOR_WARN) != 0);

    printf("  -> User closes door again; verifying seamless cycle resumption...\n");
    plant.door_closed = true;
    plant_sync_to_io(&plant, &cpu);

    for (int i = 0; i < 40; i++) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        if (cpu.io_ports[PORT_ACTUATORS & 0x1F] > 0) break;
    }
    assert_true("Actuators resumed active washing after door closure", cpu.io_ports[PORT_ACTUATORS & 0x1F] > 0);

    // --- TEST 4: Overheat Safety Emergency Trip ---
    printf("\n--- [Test 4] Overheat Safety Emergency Trip ---\n");
    cpu_init(&cpu);
    cpu_load_hex_file(&cpu, hex_file);
    plant_init(&plant, &cpu);

    plant.door_closed = true;
    plant.selected_mode = 0;
    plant.start_button_pressed = true;
    plant_sync_to_io(&plant, &cpu);

    for (int i = 0; i < 300; i++) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        if (i == 20) {
            plant.start_button_pressed = false;
            plant_sync_to_io(&plant, &cpu);
        }
    }

    printf("  -> Injecting Fault: Water temperature surges past safe limit (78°C)!\n");
    plant.water_temp = 78.0;
    plant_sync_to_io(&plant, &cpu);

    for (int i = 0; i < 50; i++) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        plant.water_temp = 78.0;
        plant_sync_to_io(&plant, &cpu);
        if (cpu.halted) break;
    }

    uint8_t fault_leds = cpu.io_ports[PORT_STATUS_LEDS & 0x1F];
    uint8_t progress_err = cpu.io_ports[PORT_PROGRESS & 0x1F];

    assert_true("CPU halted immediately on thermal limit breach", cpu.halted);
    assert_true("Fault LED illuminated", (fault_leds & LED_FAULT) != 0);
    assert_true("Progress register displays Overheat Error Code 0xAA", progress_err == 0xAA);

    printf("==========================================================\n");
    printf("Integration Tests: %d Passed, %d Failed.\n", (passed_count - start_p), (failed_count - start_f));
}

// ==============================================================================
// PART 4: REAL-TIME ASCII DASHBOARD VISUALIZER
// ==============================================================================
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
    printf("| CPU STATE:  PC: 0x%02X | Next Instruction: [0x%04X] %-30s |\n", cpu->pc, cpu->rom[cpu->pc], dis);
    printf("| REGISTERS:  R0: 0x%02X (%3d) | R1: 0x%02X (%3d) | R2: 0x%02X (%3d) | R3: 0x%02X (%3d)   |\n",
           cpu->R[0], cpu->R[0], cpu->R[1], cpu->R[1], cpu->R[2], cpu->R[2], cpu->R[3], cpu->R[3]);
    printf("| FLAGS:      [Z=%d  C=%d  N=%d  V=%d]   SP: %d    Total Cycles: %-10llu Halted: %-5s |\n",
           cpu->flag_z, cpu->flag_c, cpu->flag_n, cpu->flag_v, cpu->sp,
           (unsigned long long)cpu->total_cycles, cpu->halted ? "true" : "false");
    printf("+---------------------------------------------------------------------------------+\n");
    printf("| CYCLE STAGE: %-15s | Progress Port: 0x%02X | Mode: %-22s |\n", phase_name, progress, mode_str);
    printf("| SENSORS:    Door: %-6s | Water Level: %3d/128 | Temp: %4.1f C | Turbidity: %-3d   |\n",
           plant->door_closed ? "CLOSED" : "OPEN", plant->water_level, plant->water_temp, plant->turbidity);
    printf("| ACTUATORS:  Inlet: [%c] | Drain: [%c] | Spray: [%c] | Heater: [%c] | Fan: [%c] |\n",
           (act & ACT_INLET_VALVE) ? '*' : ' ', (act & ACT_DRAIN_PUMP) ? '*' : ' ',
           (act & ACT_SPRAY_PUMP)  ? '*' : ' ', (act & ACT_HEATER)     ? '*' : ' ',
           (act & ACT_DRY_FAN)     ? '*' : ' ');
    printf("|             DetDisp: [%c] | RinseDisp: [%c] | Buzzer: [%c]                        |\n",
           (act & ACT_DET_DISP)   ? '*' : ' ', (act & ACT_RINSE_DISP)  ? '*' : ' ',
           (act & ACT_BUZZER)     ? '*' : ' ');
    printf("| STATUS LED: ");
    if (leds & LED_STANDBY)   printf("[STANDBY] ");
    if (leds & LED_PREWASH)   printf("[PRE-WASH] ");
    if (leds & LED_MAINWASH)  printf("[MAIN WASH] ");
    if (leds & LED_RINSE)     printf("[RINSE] ");
    if (leds & LED_DRYING)    printf("[DRYING] ");
    if (leds & LED_COMPLETE)  printf("[CYCLE COMPLETE!] ");
    if (leds & LED_DOOR_WARN) printf("[DOOR AJAR WARNING!] ");
    if (leds & LED_FAULT)     printf("[EMERGENCY FAULT!] ");
    printf("\n+---------------------------------------------------------------------------------+\n");
}

// ==============================================================================
// PART 5: FAST BATCH CYCLE EXECUTION ENGINE
// ==============================================================================
static void run_batch_simulation(const char *hex_file) {
    AquaCore8 cpu;
    DishwasherPlant plant;
    cpu_init(&cpu);

    if (!cpu_load_hex_file(&cpu, hex_file)) {
        printf("Error: Could not load %s\n", hex_file);
        return;
    }

    plant_init(&plant, &cpu);
    plant.door_closed = true;
    plant.selected_mode = 0; // Normal Mode
    plant.start_button_pressed = true;
    plant_sync_to_io(&plant, &cpu);

    printf("Running automatic batch simulation (Normal Cycle)...\n");

    uint8_t last_progress = 0xFF;
    int cycle = 0;

    while (cycle < 15000 && !cpu.halted) {
        cpu_step(&cpu);
        plant_step(&plant, &cpu);
        cycle++;

        if (cycle == 20) {
            plant.start_button_pressed = false;
            plant_sync_to_io(&plant, &cpu);
        }

        uint8_t progress = cpu.io_ports[PORT_PROGRESS & 0x1F];
        if (progress != last_progress) {
            last_progress = progress;
            print_dashboard(&cpu, &plant);
        }
    }

    printf("\nBatch simulation finished in %d CPU clock cycles.\n", cycle);
    print_dashboard(&cpu, &plant);
}

// ==============================================================================
// PART 6: INTERACTIVE TERMINAL MONITOR (Real-Time Play)
// ==============================================================================
static void run_interactive_cli(const char *hex_file) {
    AquaCore8 cpu;
    DishwasherPlant plant;
    cpu_init(&cpu);

    if (!cpu_load_hex_file(&cpu, hex_file)) {
        printf("Error: Could not load machine code from %s\n", hex_file);
        return;
    }

    plant_init(&plant, &cpu);

    printf("=======================================================================\n");
    printf("     AquaCore-8 Dishwasher CPU Architecture Interactive Monitor        \n");
    printf("=======================================================================\n");
    printf(" Commands: start | mode <0-2> | door open | door close | temp <c>      \n");
    printf("           step [n] | run [n] | status | quit                          \n");
    printf("=======================================================================\n");

    print_dashboard(&cpu, &plant);

    char line[128];
    while (1) {
        printf("\naquacore8> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        char cmd[32];
        int arg = 0;
        int n = sscanf(line, "%31s %d", cmd, &arg);
        if (n <= 0) continue;

        if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            break;
        } else if (strcmp(cmd, "start") == 0) {
            plant.start_button_pressed = true;
            plant_sync_to_io(&plant, &cpu);
            printf("Start button pressed.\n");
        } else if (strcmp(cmd, "mode") == 0) {
            plant.selected_mode = arg & 0x03;
            plant_sync_to_io(&plant, &cpu);
            printf("Mode set to: %d\n", plant.selected_mode);
        } else if (strcmp(cmd, "door") == 0) {
            char subcmd[32];
            sscanf(line, "%*s %31s", subcmd);
            if (strcmp(subcmd, "open") == 0) {
                plant.door_closed = false;
                printf("Door is OPEN. Interlock activated.\n");
            } else {
                plant.door_closed = true;
                printf("Door is CLOSED.\n");
            }
            plant_sync_to_io(&plant, &cpu);
        } else if (strcmp(cmd, "temp") == 0) {
            plant.water_temp = (double)arg;
            plant_sync_to_io(&plant, &cpu);
            printf("Water temperature injected: %d C\n", arg);
        } else if (strcmp(cmd, "step") == 0) {
            int steps = (n > 1 && arg > 0) ? arg : 1;
            for (int i = 0; i < steps; i++) {
                cpu_step(&cpu);
                plant_step(&plant, &cpu);
            }
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "run") == 0) {
            int steps = (n > 1 && arg > 0) ? arg : 100;
            for (int i = 0; i < steps; i++) {
                if (cpu.halted) break;
                cpu_step(&cpu);
                plant_step(&plant, &cpu);
            }
            print_dashboard(&cpu, &plant);
        } else if (strcmp(cmd, "status") == 0) {
            print_dashboard(&cpu, &plant);
        } else {
            printf("Unknown command. Type 'step', 'run', 'door open', 'door close', or 'quit'.\n");
        }
    }
}

// ==============================================================================
// PART 7: MAIN DISPATCHER
// ==============================================================================
int main(int argc, char **argv) {
    const char *hex_file = "programs/dishwasher_controller.hex";

    if (argc > 1) {
        if (strcmp(argv[1], "--test") == 0) {
            run_unit_tests();
            run_integration_tests(hex_file);
            return failed_count;
        } else if (strcmp(argv[1], "--batch") == 0) {
            run_batch_simulation(hex_file);
            return 0;
        }
    }

    run_interactive_cli(hex_file);
    return 0;
}
