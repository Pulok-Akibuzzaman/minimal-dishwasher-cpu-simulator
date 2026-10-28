// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         control_unit.v - Single-Cycle Hardwired Instruction Decoder
// ==============================================================================

module control_unit (
    input  wire [3:0] opcode,
    input  wire [2:0] funct,
    input  wire       is_shr,
    input  wire [3:0] cond,
    input  wire       flag_z,
    input  wire       flag_c,
    input  wire       flag_n,
    input  wire       flag_v,
    output reg        reg_write,
    output reg        alu_src,    // 0: Reg Rt, 1: Immediate
    output reg  [3:0] alu_op,
    output reg        mem_read,
    output reg        mem_write,
    output reg        io_read,
    output reg        io_write,
    output reg  [1:0] reg_dst_src, // 0: ALU, 1: Memory, 2: IO, 3: Imm
    output reg        branch_taken,
    output reg        jump,
    output reg        call,
    output reg        ret,
    output reg        halt
);

    reg cond_true;

    // Condition Code Evaluator
    always @(*) begin
        case (cond)
            4'h0: cond_true = flag_z;                    // JZ
            4'h1: cond_true = ~flag_z;                   // JNZ
            4'h2: cond_true = flag_c;                    // JC
            4'h3: cond_true = ~flag_c;                   // JNC
            4'h4: cond_true = flag_n;                    // JN
            4'h5: cond_true = ~flag_n;                   // JP
            4'h6: cond_true = flag_v;                    // JV
            4'h7: cond_true = (~flag_z) && (flag_n == flag_v); // JGT
            4'h8: cond_true = (flag_n != flag_v);        // JLT
            4'h9: cond_true = (flag_n == flag_v);        // JGE
            4'hA: cond_true = flag_z || (flag_n != flag_v); // JLE
            default: cond_true = 1'b0;
        endcase
    end

    // Instruction Decode & Control Line Assignment
    always @(*) begin
        reg_write    = 1'b0;
        alu_src      = 1'b0;
        alu_op       = 4'd0;
        mem_read     = 1'b0;
        mem_write    = 1'b0;
        io_read      = 1'b0;
        io_write     = 1'b0;
        reg_dst_src  = 2'd0; // ALU
        branch_taken = 1'b0;
        jump         = 1'b0;
        call         = 1'b0;
        ret          = 1'b0;
        halt         = 1'b0;

        case (opcode)
            4'h0: begin // R-Type
                reg_write   = (funct != 3'd5); // CMP does not write back
                alu_src     = 1'b0;
                reg_dst_src = 2'd0;
                case (funct)
                    3'd0: alu_op = 4'd0; // ADD
                    3'd1: alu_op = 4'd1; // SUB
                    3'd2: alu_op = 4'd2; // AND
                    3'd3: alu_op = 4'd3; // OR
                    3'd4: alu_op = 4'd4; // XOR
                    3'd5: alu_op = 4'd5; // CMP
                    3'd6: alu_op = 4'd6; // NOT
                    3'd7: alu_op = is_shr ? 4'd8 : 4'd7; // SHF
                    default: alu_op = 4'd0;
                endcase
            end

            4'h1: begin // ADDI
                reg_write   = 1'b1;
                alu_src     = 1'b1;
                alu_op      = 4'd0;
                reg_dst_src = 2'd0;
            end

            4'h2: begin // SUBI
                reg_write   = 1'b1;
                alu_src     = 1'b1;
                alu_op      = 4'd1;
                reg_dst_src = 2'd0;
            end

            4'h3: begin // CMPI
                reg_write   = 1'b0; // No writeback
                alu_src     = 1'b1;
                alu_op      = 4'd5;
            end

            4'h4: begin // ANDI
                reg_write   = 1'b1;
                alu_src     = 1'b1;
                alu_op      = 4'd2;
                reg_dst_src = 2'd0;
            end

            4'h5: begin // ORI
                reg_write   = 1'b1;
                alu_src     = 1'b1;
                alu_op      = 4'd3;
                reg_dst_src = 2'd0;
            end

            4'h6: begin // LDI
                reg_write   = 1'b1;
                reg_dst_src = 2'd3; // Direct Immediate
            end

            4'h7: begin // LOAD
                reg_write   = 1'b1;
                mem_read    = 1'b1;
                reg_dst_src = 2'd1; // From RAM
            end

            4'h8: begin // STORE
                mem_write   = 1'b1;
            end

            4'h9: begin // IN
                reg_write   = 1'b1;
                io_read     = 1'b1;
                reg_dst_src = 2'd2; // From IO
            end

            4'hA: begin // OUT
                io_write    = 1'b1;
            end

            4'hB: begin // BRANCH
                branch_taken = cond_true;
            end

            4'hC: begin // JMP
                jump = 1'b1;
            end

            4'hD: begin // CALL / RET
                if (cond[3]) begin // Bit 11 == 1 for RET
                    ret = 1'b1;
                end else begin
                    call = 1'b1;
                end
            end

            4'hE: begin // WAIT
                // NOP tick
            end

            4'hF: begin // SYS (NOP / HALT)
                if (cond == 4'h1) halt = 1'b1;
            end

            default: begin
            end
        endcase
    end

endmodule
