// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         dishwasher_tb.v - Hardware Testbench & Timing Stimulus
// ==============================================================================

`timescale 1ns / 1ps

module dishwasher_tb;

    reg clk;
    reg rst;

    // Sensor inputs
    reg [7:0] sensor_sys_in;
    reg [7:0] sensor_water_lvl;
    reg [7:0] sensor_water_temp;
    reg [7:0] sensor_turbidity;
    reg [7:0] sensor_aux;

    // Outputs
    wire [7:0]  actuators_out;
    wire [7:0]  status_leds_out;
    wire [7:0]  progress_out;
    wire        halt_out;
    wire [7:0]  pc_out;
    wire [15:0] instruction_out;
    wire [7:0]  r0_out, r1_out, r2_out, r3_out;

    // DUT Instantiation
    cpu_top dut (
        .clk(clk),
        .rst(rst),
        .sensor_sys_in(sensor_sys_in),
        .sensor_water_lvl(sensor_water_lvl),
        .sensor_water_temp(sensor_water_temp),
        .sensor_turbidity(sensor_turbidity),
        .sensor_aux(sensor_aux),
        .actuators_out(actuators_out),
        .status_leds_out(status_leds_out),
        .progress_out(progress_out),
        .halt_out(halt_out),
        .pc_out(pc_out),
        .instruction_out(instruction_out),
        .r0_out(r0_out),
        .r1_out(r1_out),
        .r2_out(r2_out),
        .r3_out(r3_out)
    );

    // Clock generator: 100MHz (10ns period)
    always #5 clk = ~clk;

    // Physical plant simulation loop (water level and temp feedback)
    always @(posedge clk) begin
        if (!rst) begin
            // Inlet valve open: water fills
            if (actuators_out[0] && !actuators_out[1]) begin
                if (sensor_water_lvl < 8'h60) sensor_water_lvl <= sensor_water_lvl + 8'd2;
            end
            // Drain pump on: water drains
            else if (actuators_out[1] && !actuators_out[0]) begin
                if (sensor_water_lvl > 8'h00) sensor_water_lvl <= sensor_water_lvl - 8'd2;
            end

            // Heater on: water warms up
            if (actuators_out[3] && sensor_water_lvl >= 8'h10) begin
                if (sensor_water_temp < 8'd70) sensor_water_temp <= sensor_water_temp + 8'd1;
            end
        end
    end

    initial begin
        $display("==================================================");
        $display("   AquaCore-8 Dishwasher Verilog Testbench Start  ");
        $display("==================================================");

        clk = 0;
        rst = 1;
        sensor_sys_in     = 8'h02; // Door closed (bit 1=1), Start=0
        sensor_water_lvl  = 8'h00; // Empty
        sensor_water_temp = 8'd20; // 20°C
        sensor_turbidity  = 8'h00; // Clean
        sensor_aux        = 8'h00;

        #20;
        rst = 0;
        $display("[%0t ns] Reset de-asserted. CPU in Standby.", $time);

        // Press Start Button (Bit 0 = 1) in Normal Mode (bits 7:4 = 0000)
        #50;
        sensor_sys_in = 8'h03; // Start=1, Door Closed=1
        $display("[%0t ns] Start Button Pressed! Beginning cycle.", $time);

        #100;
        sensor_sys_in = 8'h02; // Release start button (latch start)

        // Wait for cycle execution or timeout
        #50000;

        $display("==================================================");
        $display("[%0t ns] Simulation Finished.", $time);
        $display("Final CPU State: PC=0x%02X, LEDs=0x%02X, Actuators=0x%02X, Halt=%b",
                 pc_out, status_leds_out, actuators_out, halt_out);
        $display("==================================================");
        $finish;
    end

endmodule
