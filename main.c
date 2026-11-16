// CSE360 Mini-Project: Minimal CPU Architecture (ISA, Datapath, Control Unit) for Dishwasher Control
#include <stdio.h>

// --- 1. INSTRUCTION SET ARCHITECTURE (ISA) & HARDWARE PORTS ---
enum OperationCode { READ_SENSOR, SET_DEVICE, COMPARE_VALUE, JUMP_IF_NOT_EQUAL, HALT_CPU };
enum SensorType    { SENSOR_DOOR, SENSOR_WATER_LEVEL, SENSOR_WATER_TEMP };
enum HardwareDevice{ WATER_INLET_VALVE, HEATER_AND_PUMP, DRAIN_PUMP, DRYING_FAN, FINISH_BUZZER };

typedef struct {
    int operation_code;
    int operand;
} Instruction;

// --- 2. CPU REGISTERS (DATAPATH) & SYSTEM STATE ---
int program_counter           = 0; // PC: points to next instruction
Instruction instruction_reg;       // IR: holds currently fetched instruction
int data_register             = 0; // R0: working accumulator register
int zero_flag                 = 0; // ZF: set to 1 if comparison matches
int is_processor_halted       = 0; // Halt flag: stops the CPU clock

// Sensor Inputs & Actuator Outputs
int is_door_closed            = 1;
int current_water_level       = 0;
int current_water_temperature = 20;
int active_hardware_device    = -1;

// --- 3. DISHWASHER PROGRAM STORED IN INSTRUCTION MEMORY (ROM) ---
Instruction instruction_memory[] = {
    { READ_SENSOR, SENSOR_DOOR }, { COMPARE_VALUE, 1 }, { JUMP_IF_NOT_EQUAL, 0 },         // 0-2: Check safety door
    { SET_DEVICE, WATER_INLET_VALVE },                                                    // 3: Open inlet valve
    { READ_SENSOR, SENSOR_WATER_LEVEL }, { COMPARE_VALUE, 50 }, { JUMP_IF_NOT_EQUAL, 4 }, // 4-6: Fill tub to 50 Liters
    { SET_DEVICE, HEATER_AND_PUMP },                                                      // 7: Heat water & wash dishes
    { READ_SENSOR, SENSOR_WATER_TEMP }, { COMPARE_VALUE, 55 }, { JUMP_IF_NOT_EQUAL, 8 },  // 8-10: Heat to 55 Celsius
    { SET_DEVICE, DRAIN_PUMP },                                                           // 11: Drain dirty water
    { READ_SENSOR, SENSOR_WATER_LEVEL }, { COMPARE_VALUE, 0 }, { JUMP_IF_NOT_EQUAL, 12 },  // 12-14: Drain until 0 Liters
    { SET_DEVICE, DRYING_FAN },                                                           // 15: Run hot air drying fan
    { READ_SENSOR, SENSOR_WATER_TEMP }, { COMPARE_VALUE, 30 }, { JUMP_IF_NOT_EQUAL, 16 }, // 16-18: Cool/Dry to 30 Celsius
    { SET_DEVICE, FINISH_BUZZER }, { HALT_CPU, 0 }                                        // 19-20: Sound buzzer & stop
};

// --- 4. CONTROL UNIT (Fetch -> Decode -> Execute Pipeline) ---
void execute_cpu_cycle(void) {
    // 1. FETCH: Read instruction from ROM at PC into IR, increment PC
    instruction_reg = instruction_memory[program_counter++];
    
    // 2. DECODE & 3. EXECUTE: Control Unit decodes opcode and triggers datapath
    switch (instruction_reg.operation_code) {
        case READ_SENSOR:
            if (instruction_reg.operand == SENSOR_DOOR)        data_register = is_door_closed;
            if (instruction_reg.operand == SENSOR_WATER_LEVEL) data_register = current_water_level;
            if (instruction_reg.operand == SENSOR_WATER_TEMP)  data_register = current_water_temperature;
            break;
        case SET_DEVICE:          active_hardware_device = instruction_reg.operand; break;
        case COMPARE_VALUE:       zero_flag = (data_register == instruction_reg.operand); break;
        case JUMP_IF_NOT_EQUAL:   if (!zero_flag) program_counter = instruction_reg.operand; break;
        case HALT_CPU:            is_processor_halted = 1; break;
    }
}

// --- 5. PHYSICAL DISHWASHER SIMULATION (Cyber-Physical Plant) ---
void simulate_dishwasher_hardware(void) {
    if (active_hardware_device == WATER_INLET_VALVE && current_water_level < 50) current_water_level += 10;
    if (active_hardware_device == HEATER_AND_PUMP   && current_water_temperature < 55) current_water_temperature += 5;
    if (active_hardware_device == DRAIN_PUMP        && current_water_level > 0) current_water_level -= 10;
    if (active_hardware_device == DRYING_FAN        && current_water_temperature > 30) current_water_temperature -= 5;
}

const char *device_names[] = {
    "Water Inlet Valve (Filling)", "Heater & Spray Pump (Washing)",
    "Drain Pump (Emptying)", "Drying Fan (Drying)", "Buzzer (Wash Completed)"
};

int main(void) {
    printf("========================================================================\n");
    printf("        AquaCore-8: Simple Dishwasher CPU Architecture Simulator        \n");
    printf("========================================================================\n");
    printf("Clock Cycle | PC | Water Level | Temperature | Active Hardware Device   \n");
    printf("------------+----+-------------+-------------+--------------------------\n");

    int clock_cycle = 0;
    while (!is_processor_halted && clock_cycle < 100) {
        int previous_device = active_hardware_device;
        execute_cpu_cycle();
        simulate_dishwasher_hardware();
        if (active_hardware_device != previous_device || is_processor_halted) {
            printf("    %3d     | %2d |     %2dL     |     %2dC     | %s\n",
                   clock_cycle, program_counter, current_water_level, current_water_temperature,
                   active_hardware_device >= 0 ? device_names[active_hardware_device] : "Idle (Standby)");
        }
        clock_cycle++;
    }
    printf("========================================================================\n");
    printf("SUCCESS: Dishwasher completed cycle in %d CPU clock cycles!\n", clock_cycle);
    return 0;
}
