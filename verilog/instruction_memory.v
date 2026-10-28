// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         instruction_memory.v - 256 x 16-Bit Program ROM
// ==============================================================================

module instruction_memory (
    input  wire [7:0]  pc,
    output wire [15:0] instruction
);

    reg [15:0] rom [0:255];

    initial begin
        // Can be loaded with $readmemh("dishwasher_controller.hex", rom);
        // Default initialized to 0
        $readmemh("dishwasher_controller.hex", rom);
    end

    assign instruction = rom[pc];

endmodule
