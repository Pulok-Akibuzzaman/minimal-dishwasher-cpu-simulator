// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         data_memory.v - 256 x 8-Bit Scratchpad Data RAM
// ==============================================================================

module data_memory (
    input  wire       clk,
    input  wire       mem_read,
    input  wire       mem_write,
    input  wire [7:0] address,
    input  wire [7:0] write_data,
    output wire [7:0] read_data
);

    reg [7:0] ram [0:255];
    integer i;

    initial begin
        for (i = 0; i < 256; i = i + 1) begin
            ram[i] = 8'h00;
        end
    end

    always @(posedge clk) begin
        if (mem_write) begin
            ram[address] <= write_data;
        end
    end

    assign read_data = mem_read ? ram[address] : 8'h00;

endmodule
