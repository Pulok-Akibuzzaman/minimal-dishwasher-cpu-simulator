/**
 * ==============================================================================
 * Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
 * Course:       CSE360 Computer Architecture
 * Institution:  East West University (EWU)
 * Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
 *               Pulok Akibuzzaman (ID: 2023-3-60-051)
 *               Md. Jakaria Masud (ID: 2023-3-60-386)
 * File:         dishwasher_plant.c - Fluid Dynamics, Heating & Turbidity Physics
 * ==============================================================================
 */

#include "dishwasher_plant.h"
#include <math.h>

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
