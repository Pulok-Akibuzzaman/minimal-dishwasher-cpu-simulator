/**
 * ==============================================================================
 * Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
 * Course:       CSE360 Computer Architecture
 * Institution:  East West University (EWU)
 * Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
 *               Pulok Akibuzzaman (ID: 2023-3-60-051)
 *               Md. Jakaria Masud (ID: 2023-3-60-386)
 * File:         dishwasher_plant.h - Cyber-Physical Fluid & Thermal Model Header
 * ==============================================================================
 */

#ifndef DISHWASHER_PLANT_H
#define DISHWASHER_PLANT_H

#include "aquacore8.h"

#define WATER_LEVEL_EMPTY 0x00
#define WATER_LEVEL_LOW   0x10
#define WATER_LEVEL_FULL  0x50
#define WATER_LEVEL_HEAVY 0x60
#define WATER_LEVEL_MAX   0x80

typedef struct {
    bool   door_closed;
    bool   start_button_pressed;
    int    selected_mode;       // 0=Normal, 1=Eco, 2=Heavy
    int    water_level;         // 0 to 128
    double water_temp;          // Temperature in Celsius
    int    turbidity;           // 0 (clean) to 100 (filthy)
    int    dish_soil_remaining; // 0 to 100%
    bool   detergent_released;
    bool   rinse_aid_released;
    bool   leak_detected;
    bool   dry_burn_alarm;
} DishwasherPlant;

void plant_init(DishwasherPlant *p, AquaCore8 *cpu);
void plant_step(DishwasherPlant *p, AquaCore8 *cpu);
void plant_sync_to_io(DishwasherPlant *p, AquaCore8 *cpu);

#endif // DISHWASHER_PLANT_H
