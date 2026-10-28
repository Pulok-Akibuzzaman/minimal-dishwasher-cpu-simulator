// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         datapath.v - PC, Return Stack, Register File, ALU & MUX Multiplexers
// ==============================================================================

module datapath (
    input  wire        clk,
    input  wire        rst,
    // Control signals from Control Unit
    input  wire        reg_write,
    input  wire        alu_src,
    input  wire [3:0]  alu_op,
    input  wire [1:0]  reg_dst_src, // 0: ALU, 1: Mem, 2: IO, 3: Imm
    input  wire        branch_taken,
    input  wire        jump,
    input  wire        call,
    input  wire        ret,
    // Instruction fields
    input  wire [1:0]  rd,
    input  wire [1:0]  rs,
    input  wire [1:0]  rt,
    input  wire [7:0]  imm8,
    // Memory and IO buses
    input  wire [7:0]  mem_read_data,
    input  wire [7:0]  io_read_data,
    output wire [7:0]  mem_addr,
    output wire [7:0]  mem_write_data,
    output wire [7:0]  io_addr,
    output wire [7:0]  io_write_data,
    output reg  [7:0]  pc,
    output wire [7:0]  next_pc,
    // ALU flags to Control Unit
    output wire        flag_z,
    output wire        flag_c,
    output wire        flag_n,
    output wire        flag_v,
    // Register outputs for monitoring
    output wire [7:0]  r0_out,
    output wire [7:0]  r1_out,
    output wire [7:0]  r2_out,
    output wire [7:0]  r3_out
);

    // Internal buses
    wire [7:0] r_data1;
    wire [7:0] r_data2;
    wire [7:0] alu_b_operand;
    wire [7:0] alu_result;
    reg  [7:0] writeback_data;

    // 8-level Return Stack for CALL / RET
    reg  [7:0] call_stack [0:7];
    reg  [2:0] sp;

    // Program Counter Logic
    assign next_pc = pc + 8'd1;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            pc <= 8'h00;
            sp <= 3'd0;
        end else begin
            if (ret) begin
                if (sp > 0) begin
                    pc <= call_stack[sp - 1];
                    sp <= sp - 1;
                end
            end else if (call) begin
                if (sp < 7) begin
                    call_stack[sp] <= next_pc;
                    sp <= sp + 1;
                end
                pc <= imm8;
            end else if (jump || branch_taken) begin
                pc <= imm8;
            end else begin
                pc <= next_pc;
            end
        end
    end

    // ALU Operand Mux
    assign alu_b_operand = alu_src ? imm8 : r_data2;

    // Writeback Mux
    always @(*) begin
        case (reg_dst_src)
            2'd0: writeback_data = alu_result;
            2'd1: writeback_data = mem_read_data;
            2'd2: writeback_data = io_read_data;
            2'd3: writeback_data = imm8;
            default: writeback_data = alu_result;
        endcase
    end

    // Register File instantiation
    register_file reg_file_inst (
        .clk(clk),
        .rst(rst),
        .reg_write(reg_write),
        .read_reg1(rs),
        .read_reg2(rt),
        .write_reg(rd),
        .write_data(writeback_data),
        .read_data1(r_data1),
        .read_data2(r_data2),
        .r0_out(r0_out),
        .r1_out(r1_out),
        .r2_out(r2_out),
        .r3_out(r3_out)
    );

    // ALU instantiation
    alu alu_inst (
        .a(r_data1),
        .b(alu_b_operand),
        .alu_op(alu_op),
        .result(alu_result),
        .flag_z(flag_z),
        .flag_c(flag_c),
        .flag_n(flag_n),
        .flag_v(flag_v)
    );

    // External memory & IO connections
    assign mem_addr       = imm8;
    assign mem_write_data = r_data1;
    assign io_addr        = imm8;
    assign io_write_data  = r_data1;

endmodule
