// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         io_controller.v - 32-Port Memory/Port-Mapped Peripheral Controller
// ==============================================================================

module io_controller (
    input  wire       clk,
    input  wire       rst,
    input  wire       io_read,
    input  wire       io_write,
    input  wire [7:0] port_addr,
    input  wire [7:0] write_data,
    output reg  [7:0] read_data,

    // Sensor Inputs
    input  wire [7:0] sensor_sys_in,     // Port 0x00: Start, Door, Mode
    input  wire [7:0] sensor_water_lvl,  // Port 0x01: Water level
    input  wire [7:0] sensor_water_temp, // Port 0x02: Water temperature
    input  wire [7:0] sensor_turbidity,  // Port 0x03: Turbidity
    input  wire [7:0] sensor_aux,        // Port 0x04: Aux sensors

    // Actuator & Display Outputs
    output reg  [7:0] port_actuators,    // Port 0x10: Actuators (Inlet, Drain, Spray, Heater, Fan, Buzzer)
    output reg  [7:0] port_status_leds,  // Port 0x11: Status LEDs
    output reg  [7:0] port_progress      // Port 0x12: Progress indicator
);

    // Read Port Multiplexer
    always @(*) begin
        if (io_read) begin
            case (port_addr)
                8'h00: read_data = sensor_sys_in;
                8'h01: read_data = sensor_water_lvl;
                8'h02: read_data = sensor_water_temp;
                8'h03: read_data = sensor_turbidity;
                8'h04: read_data = sensor_aux;
                8'h10: read_data = port_actuators;
                8'h11: read_data = port_status_leds;
                8'h12: read_data = port_progress;
                default: read_data = 8'h00;
            endcase
        end else begin
            read_data = 8'h00;
        end
    end

    // Write Port Registers
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            port_actuators   <= 8'h00;
            port_status_leds <= 8'h00;
            port_progress    <= 8'h00;
        end else if (io_write) begin
            case (port_addr)
                8'h10: port_actuators   <= write_data;
                8'h11: port_status_leds <= write_data;
                8'h12: port_progress    <= write_data;
                default: begin end
            endcase
        end
    end

endmodule
