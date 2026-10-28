// ==============================================================================
// Project:      AquaCore-8 Microprocessor Architecture for Dishwasher Automation
// Course:       CSE360 Computer Architecture
// Institution:  East West University (EWU)
// Authors:      Nusrat Rahman Aurna (ID: 2023-3-60-057)
//               Pulok Akibuzzaman (ID: 2023-3-60-051)
//               Md. Jakaria Masud (ID: 2023-3-60-386)
// File:         cpu_top.v - Top-Level Synthesizable Processor Core
// ==============================================================================

module cpu_top (
    input  wire        clk,
    input  wire        rst,

    // External Sensor Inputs
    input  wire [7:0]  sensor_sys_in,
    input  wire [7:0]  sensor_water_lvl,
    input  wire [7:0]  sensor_water_temp,
    input  wire [7:0]  sensor_turbidity,
    input  wire [7:0]  sensor_aux,

    // External Actuator & Status Outputs
    output wire [7:0]  actuators_out,
    output wire [7:0]  status_leds_out,
    output wire [7:0]  progress_out,
    output wire        halt_out,

    // Internal Monitor Taps
    output wire [7:0]  pc_out,
    output wire [15:0] instruction_out,
    output wire [7:0]  r0_out,
    output wire [7:0]  r1_out,
    output wire [7:0]  r2_out,
    output wire [7:0]  r3_out
);

    // Instruction bus
    wire [7:0]  pc;
    wire [15:0] instruction;

    // Control signals
    wire        reg_write;
    wire        alu_src;
    wire [3:0]  alu_op;
    wire        mem_read;
    wire        mem_write;
    wire        io_read;
    wire        io_write;
    wire [1:0]  reg_dst_src;
    wire        branch_taken;
    wire        jump;
    wire        call;
    wire        ret;
    wire        halt;

    // ALU flags
    wire        flag_z;
    wire        flag_c;
    wire        flag_n;
    wire        flag_v;

    // Data buses
    wire [7:0]  mem_addr;
    wire [7:0]  mem_write_data;
    wire [7:0]  mem_read_data;

    wire [7:0]  io_addr;
    wire [7:0]  io_write_data;
    wire [7:0]  io_read_data;

    // Instruction memory instantiation
    instruction_memory imem_inst (
        .pc(pc),
        .instruction(instruction)
    );

    // Control Unit instantiation
    control_unit ctrl_inst (
        .opcode(instruction[15:12]),
        .funct(instruction[5:3]),
        .is_shr(instruction[0]),
        .cond(instruction[11:8]),
        .flag_z(flag_z),
        .flag_c(flag_c),
        .flag_n(flag_n),
        .flag_v(flag_v),
        .reg_write(reg_write),
        .alu_src(alu_src),
        .alu_op(alu_op),
        .mem_read(mem_read),
        .mem_write(mem_write),
        .io_read(io_read),
        .io_write(io_write),
        .reg_dst_src(reg_dst_src),
        .branch_taken(branch_taken),
        .jump(jump),
        .call(call),
        .ret(ret),
        .halt(halt)
    );

    // Datapath instantiation
    datapath dpath_inst (
        .clk(clk),
        .rst(rst),
        .reg_write(reg_write),
        .alu_src(alu_src),
        .alu_op(alu_op),
        .reg_dst_src(reg_dst_src),
        .branch_taken(branch_taken),
        .jump(jump),
        .call(call),
        .ret(ret),
        .rd(instruction[11:10]),
        .rs(instruction[9:8]),
        .rt(instruction[7:6]),
        .imm8(instruction[7:0]),
        .mem_read_data(mem_read_data),
        .io_read_data(io_read_data),
        .mem_addr(mem_addr),
        .mem_write_data(mem_write_data),
        .io_addr(io_addr),
        .io_write_data(io_write_data),
        .pc(pc),
        .next_pc(),
        .flag_z(flag_z),
        .flag_c(flag_c),
        .flag_n(flag_n),
        .flag_v(flag_v),
        .r0_out(r0_out),
        .r1_out(r1_out),
        .r2_out(r2_out),
        .r3_out(r3_out)
    );

    // Data memory instantiation
    data_memory dmem_inst (
        .clk(clk),
        .mem_read(mem_read),
        .mem_write(mem_write),
        .address(mem_addr),
        .write_data(mem_write_data),
        .read_data(mem_read_data)
    );

    // IO controller instantiation
    io_controller io_inst (
        .clk(clk),
        .rst(rst),
        .io_read(io_read),
        .io_write(io_write),
        .port_addr(io_addr),
        .write_data(io_write_data),
        .read_data(io_read_data),
        .sensor_sys_in(sensor_sys_in),
        .sensor_water_lvl(sensor_water_lvl),
        .sensor_water_temp(sensor_water_temp),
        .sensor_turbidity(sensor_turbidity),
        .sensor_aux(sensor_aux),
        .port_actuators(actuators_out),
        .port_status_leds(status_leds_out),
        .port_progress(progress_out)
    );

    assign halt_out        = halt;
    assign pc_out          = pc;
    assign instruction_out = instruction;

endmodule
