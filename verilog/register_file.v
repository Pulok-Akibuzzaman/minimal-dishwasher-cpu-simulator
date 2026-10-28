// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         register_file.v - 4 x 8-Bit Register Bank (R0, R1, R2, R3)
// ==============================================================================

module register_file (
    input  wire       clk,
    input  wire       rst,
    input  wire       reg_write,
    input  wire [1:0] read_reg1, // Rs
    input  wire [1:0] read_reg2, // Rt
    input  wire [1:0] write_reg, // Rd
    input  wire [7:0] write_data,
    output wire [7:0] read_data1,
    output wire [7:0] read_data2,
    output wire [7:0] r0_out,
    output wire [7:0] r1_out,
    output wire [7:0] r2_out,
    output wire [7:0] r3_out
);

    reg [7:0] registers [0:3];
    integer i;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            for (i = 0; i < 4; i = i + 1) begin
                registers[i] <= 8'h00;
            end
        end else if (reg_write) begin
            registers[write_reg] <= write_data;
        end
    end

    // Asynchronous Read
    assign read_data1 = registers[read_reg1];
    assign read_data2 = registers[read_reg2];

    // Debug taps
    assign r0_out = registers[0];
    assign r1_out = registers[1];
    assign r2_out = registers[2];
    assign r3_out = registers[3];

endmodule
