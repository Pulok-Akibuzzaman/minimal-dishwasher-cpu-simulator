; ==============================================================================
; Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
; Course:       CSE360 Computer Architecture
; Institution:  East West University (EWU)
; Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
;               Pulok Akibuzzaman (ID: 2023-3-60-051)
;               Md. Jakaria Masud (ID: 2023-3-60-386)
; File:         dishwasher_controller.asm - Complete 216-Instruction Firmware
; ==============================================================================

; --- Port Map Constants ---
.EQU PORT_SYS_IN      0x00
.EQU PORT_WATER_LVL   0x01
.EQU PORT_WATER_TEMP  0x02
.EQU PORT_TURBIDITY   0x03
.EQU PORT_AUX_SENS    0x04

.EQU PORT_ACTUATORS   0x10
.EQU PORT_STATUS_LEDS 0x11
.EQU PORT_PROGRESS    0x12
.EQU PORT_DEBUG       0x13

; --- Actuator Bitmasks (Port 0x10) ---
.EQU ACT_OFF          0x00
.EQU ACT_INLET_VALVE  0x01
.EQU ACT_DRAIN_PUMP   0x02
.EQU ACT_SPRAY_PUMP   0x04
.EQU ACT_HEATER       0x08
.EQU ACT_DET_DISP     0x10
.EQU ACT_RINSE_DISP   0x20
.EQU ACT_DRY_FAN      0x40
.EQU ACT_BUZZER       0x80

; --- LED Bitmasks (Port 0x11) ---
.EQU LED_STANDBY      0x01
.EQU LED_PREWASH      0x02
.EQU LED_MAINWASH     0x04
.EQU LED_RINSE        0x08
.EQU LED_DRYING       0x10
.EQU LED_COMPLETE     0x20
.EQU LED_DOOR_WARN    0x40
.EQU LED_FAULT        0x80

; --- RAM Memory Variable Map ---
.EQU RAM_TARGET_TEMP  0x20
.EQU RAM_TARGET_LVL   0x21
.EQU RAM_WASH_TICKS   0x22
.EQU RAM_RINSE_COUNT  0x23
.EQU RAM_CURR_ACT     0x24
.EQU RAM_SAVED_LED    0x25

; ==============================================================================
; SYSTEM RESET & INITIALIZATION
; ==============================================================================
RESET:
    LDI R0, 0
    OUT [PORT_ACTUATORS], R0        ; Turn off all actuators
    STORE [RAM_CURR_ACT], R0
    LDI R0, 0
    OUT [PORT_PROGRESS], R0         ; Progress = 0 (Standby)
    LDI R0, LED_STANDBY
    OUT [PORT_STATUS_LEDS], R0      ; Light up Standby LED

; ==============================================================================
; STANDBY / USER SELECTION & START MONITORING
; ==============================================================================
STANDBY_LOOP:
    IN R1, [PORT_SYS_IN]            ; Read system input switches & sensors
    
    ; Check Door Closed (Bit 1: 0x02)
    MOV_TEST_DOOR:
    LDI R2, 0x02
    AND R2, R1, R2
    JZ DOOR_STANDBY_AJAR            ; If zero, door is open
    
    ; Check Start Button (Bit 0: 0x01)
    LDI R2, 0x01
    AND R2, R1, R2
    JNZ START_CYCLE                 ; Start button pressed!
    
    JMP STANDBY_LOOP

DOOR_STANDBY_AJAR:
    LDI R0, LED_DOOR_WARN
    OUT [PORT_STATUS_LEDS], R0
    WAIT #1
    JMP STANDBY_LOOP

; ==============================================================================
; CYCLE CONFIGURATION BASED ON USER MODE SELECTION
; ==============================================================================
START_CYCLE:
    IN R1, [PORT_SYS_IN]
    SHR R1, R1                      ; Shift mode bits [7:4] down
    SHR R1, R1
    SHR R1, R1
    SHR R1, R1
    ANDI R1, #0x0F

    CMPI R1, #0x01                  ; Mode 1 = Eco Wash
    JZ SETUP_ECO

    CMPI R1, #0x02                  ; Mode 2 = Heavy Wash
    JZ SETUP_HEAVY

SETUP_NORMAL:
    LDI R0, 55                      ; 55°C
    STORE [RAM_TARGET_TEMP], R0
    LDI R0, 0x50                    ; Normal Water Level
    STORE [RAM_TARGET_LVL], R0
    LDI R0, 15                      ; Wash Ticks
    STORE [RAM_WASH_TICKS], R0
    LDI R0, 1                       ; 1 Rinse Cycle
    STORE [RAM_RINSE_COUNT], R0
    JMP EXECUTE_PREWASH

SETUP_ECO:
    LDI R0, 45                      ; 45°C (Eco lower temp)
    STORE [RAM_TARGET_TEMP], R0
    LDI R0, 0x48                    ; Lower water level for eco savings
    STORE [RAM_TARGET_LVL], R0
    LDI R0, 10                      ; Wash Ticks
    STORE [RAM_WASH_TICKS], R0
    LDI R0, 1                       ; 1 Rinse Cycle
    STORE [RAM_RINSE_COUNT], R0
    JMP EXECUTE_PREWASH

SETUP_HEAVY:
    LDI R0, 65                      ; 65°C (Sanitizing heavy temp)
    STORE [RAM_TARGET_TEMP], R0
    LDI R0, 0x60                    ; Deep fill level
    STORE [RAM_TARGET_LVL], R0
    LDI R0, 22                      ; Extended Wash Ticks
    STORE [RAM_WASH_TICKS], R0
    LDI R0, 2                       ; 2 Rinse Cycles
    STORE [RAM_RINSE_COUNT], R0
    JMP EXECUTE_PREWASH

; ==============================================================================
; PHASE 1: PRE-WASH
; ==============================================================================
EXECUTE_PREWASH:
    LDI R0, LED_PREWASH
    OUT [PORT_STATUS_LEDS], R0
    LDI R0, 1
    OUT [PORT_PROGRESS], R0

    CALL FILL_WATER                 ; Fill to configured target level

    ; Circulate water with wash spray pump
    LDI R0, ACT_SPRAY_PUMP
    CALL SET_ACTUATORS

    LDI R1, 6                       ; Pre-wash circulation duration
PREWASH_CIRC_LOOP:
    CALL SAFETY_CHECK
    WAIT #1
    SUBI R1, #1
    JNZ PREWASH_CIRC_LOOP

    CALL DRAIN_WATER                ; Drain soiled pre-wash water

; ==============================================================================
; PHASE 2: MAIN WASH (WITH SMART HEATING & TURBIDITY MONITORING)
; ==============================================================================
EXECUTE_MAINWASH:
    LDI R0, LED_MAINWASH
    OUT [PORT_STATUS_LEDS], R0
    LDI R0, 2
    OUT [PORT_PROGRESS], R0

    CALL FILL_WATER                 ; Fresh water fill

    ; Release Detergent Dispenser Solenoid
    LDI R0, ACT_DET_DISP
    CALL SET_ACTUATORS
    WAIT #2

    LOAD R1, [RAM_WASH_TICKS]       ; Load wash countdown
MAINWASH_LOOP:
    CALL SAFETY_CHECK

    ; Read Temperature Sensor and regulate Heating Element
    IN R2, [PORT_WATER_TEMP]
    LOAD R3, [RAM_TARGET_TEMP]
    CMP R2, R3
    JGE TEMP_REACHED

    ; Water is below target temp -> Heat and circulate simultaneously
    LDI R0, 0x0C                    ; ACT_HEATER (0x08) | ACT_SPRAY_PUMP (0x04)
    CALL SET_ACTUATORS
    JMP WASH_TICK_DONE

TEMP_REACHED:
    ; Temp satisfied -> Spray pump only, heater turns off to save power
    LDI R0, ACT_SPRAY_PUMP
    CALL SET_ACTUATORS

WASH_TICK_DONE:
    ; Smart Turbidity Evaluation
    IN R2, [PORT_TURBIDITY]
    CMPI R2, #0x28                  ; Check if water is heavily soiled (> 40)
    JLE WASH_TIMER_DEC
    ; Highly soiled -> Add extra rinse dynamically if not already added
    LDI R3, 2
    STORE [RAM_RINSE_COUNT], R3

WASH_TIMER_DEC:
    WAIT #1
    SUBI R1, #1
    JNZ MAINWASH_LOOP

    ; Wash phase complete -> Drain wash water
    CALL DRAIN_WATER

; ==============================================================================
; PHASE 3: RINSE CYCLE (ADAPTIVE MULTI-RINSE)
; ==============================================================================
EXECUTE_RINSE:
    LDI R0, LED_RINSE
    OUT [PORT_STATUS_LEDS], R0
    LDI R0, 3
    OUT [PORT_PROGRESS], R0

RINSE_ITERATION:
    CALL FILL_WATER

    ; Dispense Rinse Aid Solenoid
    LDI R0, ACT_RINSE_DISP
    CALL SET_ACTUATORS
    WAIT #2

    ; Circulate Rinse Water
    LDI R0, ACT_SPRAY_PUMP
    CALL SET_ACTUATORS
    LDI R1, 8
RINSE_CIRC_LOOP:
    CALL SAFETY_CHECK
    WAIT #1
    SUBI R1, #1
    JNZ RINSE_CIRC_LOOP

    CALL DRAIN_WATER

    ; Check Turbidity after rinse
    IN R2, [PORT_TURBIDITY]
    CMPI R2, #0x12                  ; Clear threshold (approx 18)
    JLE RINSE_CHECK_COUNT
    ; Turbidity still detected -> ensure at least 1 more rinse
    LDI R3, 1
    STORE [RAM_RINSE_COUNT], R3

RINSE_CHECK_COUNT:
    LOAD R1, [RAM_RINSE_COUNT]
    SUBI R1, #1
    STORE [RAM_RINSE_COUNT], R1
    JNZ RINSE_ITERATION

; ==============================================================================
; PHASE 4: HEATED DRYING
; ==============================================================================
EXECUTE_DRYING:
    LDI R0, LED_DRYING
    OUT [PORT_STATUS_LEDS], R0
    LDI R0, 4
    OUT [PORT_PROGRESS], R0

    ; Make sure tub is drained dry
    LDI R0, ACT_OFF
    CALL SET_ACTUATORS

    ; Turn on Heater & Air Blower Fan
    LDI R0, 0x48                    ; ACT_HEATER (0x08) | ACT_DRY_FAN (0x40)
    CALL SET_ACTUATORS

    LDI R1, 10                      ; Drying duration
DRYING_LOOP:
    CALL SAFETY_CHECK
    WAIT #1
    SUBI R1, #1
    JNZ DRYING_LOOP

    ; Turn off all actuators
    LDI R0, ACT_OFF
    CALL SET_ACTUATORS

; ==============================================================================
; PHASE 5: CYCLE COMPLETE & USER NOTIFICATION
; ==============================================================================
CYCLE_COMPLETE:
    LDI R0, LED_COMPLETE
    OUT [PORT_STATUS_LEDS], R0
    LDI R0, 5
    OUT [PORT_PROGRESS], R0

    ; Sound Completion Chime Buzzer
    LDI R0, ACT_BUZZER
    OUT [PORT_ACTUATORS], R0
    WAIT #3
    LDI R0, ACT_OFF
    OUT [PORT_ACTUATORS], R0
    LDI R0, LED_COMPLETE
    OUT [PORT_STATUS_LEDS], R0

    HALT                            ; End of dishwasher wash cycle

; ==============================================================================
; SUBROUTINE: FILL WATER UNTIL TARGET LEVEL REACHED
; ==============================================================================
FILL_WATER:
    LDI R0, ACT_INLET_VALVE
    CALL SET_ACTUATORS
FILL_LOOP:
    CALL SAFETY_CHECK
    IN R1, [PORT_WATER_LVL]
    LOAD R2, [RAM_TARGET_LVL]
    CMP R1, R2
    JGE FILL_COMPLETE
    WAIT #1
    JMP FILL_LOOP

FILL_COMPLETE:
    LDI R0, ACT_OFF
    CALL SET_ACTUATORS
    RET

; ==============================================================================
; SUBROUTINE: DRAIN WATER UNTIL LEVEL DROPS TO EMPTY
; ==============================================================================
DRAIN_WATER:
    LDI R0, ACT_DRAIN_PUMP
    CALL SET_ACTUATORS
DRAIN_LOOP:
    CALL SAFETY_CHECK
    IN R1, [PORT_WATER_LVL]
    CMPI R1, #0x02                  ; Check if virtually empty (<= 2)
    JLE DRAIN_COMPLETE
    WAIT #1
    JMP DRAIN_LOOP

DRAIN_COMPLETE:
    LDI R0, ACT_OFF
    CALL SET_ACTUATORS
    RET

; ==============================================================================
; SUBROUTINE: SET ACTUATORS & CACHE TO RAM
; ==============================================================================
SET_ACTUATORS:
    OUT [PORT_ACTUATORS], R0
    STORE [RAM_CURR_ACT], R0
    RET

; ==============================================================================
; SUBROUTINE: SAFETY MONITOR (DOOR, OVERHEAT, LEAK DETECTION)
; ==============================================================================
SAFETY_CHECK:
    ; 1. Check Door Safety Interlock (Bit 1 of SYS_IN)
    IN R3, [PORT_SYS_IN]
    ANDI R3, #0x02
    JZ DOOR_OPEN_INTERLOCK

    ; 2. Check Overheat Emergency (Temp > 75°C)
    IN R3, [PORT_WATER_TEMP]
    CMPI R3, #75
    JGT OVERHEAT_FAULT

    ; 3. Check Water Leak Detection (Bit 3 of AUX_SENS)
    IN R3, [PORT_AUX_SENS]
    ANDI R3, #0x08
    JNZ LEAK_FAULT

    RET

DOOR_OPEN_INTERLOCK:
    ; Save active status LED before setting warning LED
    IN R3, [PORT_STATUS_LEDS]
    STORE [RAM_SAVED_LED], R3
    ; Immediately halt all physical motion and heating
    LDI R3, ACT_OFF
    OUT [PORT_ACTUATORS], R3
    LDI R3, LED_DOOR_WARN
    OUT [PORT_STATUS_LEDS], R3

DOOR_WAIT_LOOP:
    WAIT #1
    IN R3, [PORT_SYS_IN]
    ANDI R3, #0x02
    JZ DOOR_WAIT_LOOP               ; Stay paused while door is open

    ; Door re-closed! Restore LEDs and actuators from RAM cache
    LOAD R3, [RAM_SAVED_LED]
    OUT [PORT_STATUS_LEDS], R3
    LOAD R3, [RAM_CURR_ACT]
    OUT [PORT_ACTUATORS], R3
    RET

OVERHEAT_FAULT:
    ; Critical thermal trip!
    LDI R3, ACT_OFF
    OUT [PORT_ACTUATORS], R3
    LDI R3, 0x88                    ; ACT_HEATER off, buzzer on
    OUT [PORT_ACTUATORS], R3
    LDI R3, LED_FAULT
    OUT [PORT_STATUS_LEDS], R3
    LDI R3, 0xAA                    ; Error code 0xAA (Overheat)
    OUT [PORT_PROGRESS], R3
    HALT

LEAK_FAULT:
    ; Tub leak detected! Activate drain pump and raise fault
    LDI R3, ACT_DRAIN_PUMP
    OUT [PORT_ACTUATORS], R3
    LDI R3, LED_FAULT
    OUT [PORT_STATUS_LEDS], R3
    LDI R3, 0xEF                    ; Error code 0xEF (Leak)
    OUT [PORT_PROGRESS], R3
    HALT
