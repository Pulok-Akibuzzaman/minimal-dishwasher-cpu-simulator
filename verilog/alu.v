// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         alu.v - 8-Bit Arithmetic Logic Unit (ALU) with Z, C, N, V Flags
// ==============================================================================

module alu (
    input  wire [7:0] a,
    input  wire [7:0] b,
    input  wire [3:0] alu_op,   // 0: ADD, 1: SUB, 2: AND, 3: OR, 4: XOR, 5: CMP, 6: NOT, 7: SHL, 8: SHR
    output reg  [7:0] result,
    output wire       flag_z,   // Zero Flag
    output reg        flag_c,   // Carry / Borrow Flag
    output wire       flag_n,   // Negative Flag
    output reg        flag_v    // Overflow Flag
);

    reg [8:0] sum_ext;
    reg [8:0] diff_ext;

    always @(*) begin
        sum_ext  = {1'b0, a} + {1'b0, b};
        diff_ext = {1'b0, a} - {1'b0, b};
        flag_c   = 1'b0;
        flag_v   = 1'b0;
        result   = 8'h00;

        case (alu_op)
            4'd0: begin // ADD
                result = sum_ext[7:0];
                flag_c = sum_ext[8];
                flag_v = ((a[7] == b[7]) && (result[7] != a[7]));
            end
            4'd1, 4'd5: begin // SUB / CMP
                result = diff_ext[7:0];
                flag_c = (a < b); // Borrow
                flag_v = ((a[7] != b[7]) && (result[7] != a[7]));
            end
            4'd2: begin // AND
                result = a & b;
            end
            4'd3: begin // OR
                result = a | b;
            end
            4'd4: begin // XOR
                result = a ^ b;
            end
            4'd6: begin // NOT
                result = ~a;
            end
            4'd7: begin // SHL
                flag_c = a[7];
                result = {a[6:0], 1'b0};
            end
            4'd8: begin // SHR
                flag_c = a[0];
                result = {1'b0, a[7:1]};
            end
            default: begin
                result = 8'h00;
            end
        endcase
    end

    assign flag_z = (result == 8'h00);
    assign flag_n = result[7];

endmodule
