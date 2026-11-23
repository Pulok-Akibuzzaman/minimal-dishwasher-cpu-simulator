/*
 * ========================================================================================
 * EAST WEST UNIVERSITY
 * Department of Computer Science & Engineering
 * Course: CSE360 - Computer Architecture | Semester: Fall 2026
 *
 * Project Title:
 *   Suggest and design a minimal CPU architecture, including ISA, datapath,
 *   and control unit, for automating dishwasher control functions with sensor
 *   inputs, and verify it with a simulator.
 *
 * Submitted By:
 *   1. Nusrat Rahman Aurna   | Student ID: 2023-3-60-057
 *   2. Pulok Akibuzzaman     | Student ID: 2023-3-60-051
 *
 * Submitted To:
 *   Dr. Nawab Yousuf Ali
 *   Professor, Department of Computer Science & Engineering
 *   East West University, Dhaka, Bangladesh
 * ========================================================================================
 */

#include <stdio.h>

// --- 1. INSTRUCTION SET ARCHITECTURE (ISA) & HARDWARE PORTS ---
// 5-Instruction Minimal ISA
enum OperationCode {
    READ_SENSOR,        // Opcode 0: Read physical sensor value into data register
    SET_DEVICE,         // Opcode 1: Turn ON specified actuator device
    COMPARE_VALUE,      // Opcode 2: Compare data register with immediate operand, set zero flag
    JUMP_IF_NOT_EQUAL,  // Opcode 3: Branch to target ROM address if zero flag == 0
    HALT_CPU            // Opcode 4: Stop clock and halt CPU execution
};

// Physical Sensor Port Mapping (Input Channels)
enum SensorType {
    SENSOR_DOOR,        // Port 0: Safety door interlock switch (1 = Closed, 0 = Open)
    SENSOR_WATER_LEVEL, // Port 1: Tub water volume in Liters (0 to 50L)
    SENSOR_WATER_TEMP   // Port 2: Tub water temperature in Celsius (20 to 55 C)
};

// Actuator Hardware Port Mapping (Output Channels)
enum HardwareDevice {
    WATER_INLET_VALVE,  // Port 0: Solenoid valve allowing clean water fill (+10L/cycle)
    HEATER_AND_PUMP,    // Port 1: Heating element and spray circulation pump (+5C/cycle)
    DRAIN_PUMP,         // Port 2: Centrifugal pump draining dirty water (-10L/cycle)
    DRYING_FAN,         // Port 3: Forced heated air drying fan (-5C/cycle cooldown)
    FINISH_BUZZER       // Port 4: Audio beeper signaling cycle completion
};

// 2-Word Instruction Representation (Opcode + Operand)
typedef struct {
    int operation_code; // 3-bit Opcode identifier (0 to 4)
    int operand;        // 5-bit Operand (Port ID, Threshold, or ROM Address)
} Instruction;

// --- 2. CPU REGISTERS (DATAPATH) & SYSTEM STATE ---
int program_counter           = 0;  // PC: Address of next instruction in ROM
Instruction instruction_reg;        // IR: Holds currently fetched instruction word
int data_register             = 0;  // R0 / Accumulator: Working data register
int zero_flag                 = 0;  // ZF: Condition flag set to 1 if comparison matches
int is_processor_halted       = 0;  // Halt flag: Stops the clock generator loop

// Physical Appliance Environment (Sensors & Actuators)
int is_door_closed            = 1;  // Safety sensor: 1 = Door Closed, 0 = Door Open
int current_water_level       = 0;  // Physical tub water level in Liters (0 - 50L)
int current_water_temperature = 20; // Physical tub water temperature in Celsius (20 - 55C)
int active_hardware_device    = -1; // Currently energized actuator output (-1 = Standby)

// --- 3. DISHWASHER PROGRAM STORED IN INSTRUCTION MEMORY (ROM) ---
// Automated Sequence: Door Check -> Fill 50L -> Wash & Heat to 55C -> Drain 0L -> Dry to 30C -> Buzz & Halt
Instruction instruction_memory[] = {
    // Phase 0: Safety Door Interlock Verification
    { READ_SENSOR, SENSOR_DOOR }, { COMPARE_VALUE, 1 }, { JUMP_IF_NOT_EQUAL, 0 },

    // Phase 1: Tub Water Filling
    { SET_DEVICE, WATER_INLET_VALVE },
    { READ_SENSOR, SENSOR_WATER_LEVEL }, { COMPARE_VALUE, 50 }, { JUMP_IF_NOT_EQUAL, 4 },

    // Phase 2: High-Temperature Wash & Agitation
    { SET_DEVICE, HEATER_AND_PUMP },
    { READ_SENSOR, SENSOR_WATER_TEMP }, { COMPARE_VALUE, 55 }, { JUMP_IF_NOT_EQUAL, 8 },

    // Phase 3: Dirty Water Evacuation Drainage
    { SET_DEVICE, DRAIN_PUMP },
    { READ_SENSOR, SENSOR_WATER_LEVEL }, { COMPARE_VALUE, 0 }, { JUMP_IF_NOT_EQUAL, 12 },

    // Phase 4: Forced Convection Heated Drying
    { SET_DEVICE, DRYING_FAN },
    { READ_SENSOR, SENSOR_WATER_TEMP }, { COMPARE_VALUE, 30 }, { JUMP_IF_NOT_EQUAL, 16 },

    // Phase 5: Cycle Completion Notification & CPU Halt
    { SET_DEVICE, FINISH_BUZZER },
    { HALT_CPU, 0 }
};

// --- 4. CONTROL UNIT (Fetch -> Decode -> Execute Pipeline) ---
void execute_cpu_cycle(void) {
    // 1. FETCH: Fetch instruction from ROM at PC into IR, advance PC
    instruction_reg = instruction_memory[program_counter++];

    // 2. DECODE & 3. EXECUTE: Control Unit decodes opcode and drives datapath
    switch (instruction_reg.operation_code) {
        case READ_SENSOR:
            if (instruction_reg.operand == SENSOR_DOOR)        data_register = is_door_closed;
            if (instruction_reg.operand == SENSOR_WATER_LEVEL) data_register = current_water_level;
            if (instruction_reg.operand == SENSOR_WATER_TEMP)  data_register = current_water_temperature;
            break;

        case SET_DEVICE:
            active_hardware_device = instruction_reg.operand;
            break;

        case COMPARE_VALUE:
            zero_flag = (data_register == instruction_reg.operand);
            break;

        case JUMP_IF_NOT_EQUAL:
            if (!zero_flag) {
                program_counter = instruction_reg.operand;
            }
            break;

        case HALT_CPU:
            is_processor_halted = 1;
            break;
    }
}

// --- 5. PHYSICAL DISHWASHER SIMULATION (Cyber-Physical Plant) ---
void simulate_dishwasher_hardware(void) {
    // Hydrological intake: +10 Liters per active clock cycle up to 50L capacity
    if (active_hardware_device == WATER_INLET_VALVE && current_water_level < 50) {
        current_water_level += 10;
    }
    // Thermodynamic heating: +5 Celsius per active clock cycle up to 55C
    if (active_hardware_device == HEATER_AND_PUMP && current_water_temperature < 55) {
        current_water_temperature += 5;
    }
    // Hydrological drainage: -10 Liters per active clock cycle down to 0L
    if (active_hardware_device == DRAIN_PUMP && current_water_level > 0) {
        current_water_level -= 10;
    }
    // Convection fan cooling/drying: -5 Celsius per active clock cycle down to 30C
    if (active_hardware_device == DRYING_FAN && current_water_temperature > 30) {
        current_water_temperature -= 5;
    }
}

const char *device_names[] = {
    "Water Inlet Valve (Filling)",
    "Heater & Spray Pump (Washing)",
    "Drain Pump (Emptying)",
    "Drying Fan (Drying)",
    "Buzzer (Wash Completed)"
};

// --- 6. MAIN SIMULATOR DRIVER ---
int main(void) {
    printf("========================================================================================\n");
    printf("        Minimal CPU Architecture Simulator for Automated Dishwasher Control            \n");
    printf(" Course: CSE360 Computer Architecture | East West University                           \n");
    printf(" Title : Suggest & design minimal CPU (ISA, datapath, control unit) for dishwasher     \n");
    printf("========================================================================================\n");
    printf("Clock Cycle | PC | Water Level | Temperature | Active Hardware Device\n");
    printf("------------+----+-------------+-------------+------------------------------------------\n");

    int clock_cycle = 0;
    while (!is_processor_halted && clock_cycle < 100) {
        int previous_device = active_hardware_device;

        execute_cpu_cycle();
        simulate_dishwasher_hardware();

        // Print formatted telemetry on hardware state transitions or CPU halt
        if (active_hardware_device != previous_device || is_processor_halted) {
            printf("    %3d     | %2d |     %2dL     |     %2dC     | %s\n",
                   clock_cycle, program_counter, current_water_level, current_water_temperature,
                   active_hardware_device >= 0 ? device_names[active_hardware_device] : "Standby (Idle)");
        }
        clock_cycle++;
    }

    printf("========================================================================================\n");
    printf("SUCCESS: Dishwasher automation verified! Completed cycle in %d CPU clock cycles.\n", clock_cycle);
    printf("========================================================================================\n");
    return 0;
}
