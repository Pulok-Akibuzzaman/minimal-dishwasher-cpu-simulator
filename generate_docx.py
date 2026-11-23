import docx
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

def create_report():
    doc = docx.Document()

    # Base Normal Style
    normal = doc.styles['Normal']
    normal.font.name = 'Calibri'
    normal.font.size = Pt(11)
    normal.font.color.rgb = RGBColor(0x22, 0x22, 0x22)
    normal.paragraph_format.line_spacing = 1.15
    normal.paragraph_format.space_after = Pt(4)

    # 1-inch margins
    for sec in doc.sections:
        sec.top_margin = Inches(1.0)
        sec.bottom_margin = Inches(1.0)
        sec.left_margin = Inches(1.0)
        sec.right_margin = Inches(1.0)

    # Helpers
    def set_cell_background(cell, fill_hex):
        shd = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
        cell._tc.get_or_add_tcPr().append(shd)

    def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
        tcPr = cell._tc.get_or_add_tcPr()
        tcMar = OxmlElement('w:tcMar')
        for m, val in [('w:top', top), ('w:bottom', bottom), ('w:left', left), ('w:right', right)]:
            node = OxmlElement(m)
            node.set(qn('w:w'), str(val))
            node.set(qn('w:type'), 'dxa')
            tcMar.append(node)
        tcPr.append(tcMar)

    def add_h1(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(14)
        p.paragraph_format.space_after = Pt(6)
        p.paragraph_format.keep_with_next = True
        run = p.add_run(text)
        run.bold = True
        run.font.name = 'Calibri'
        run.font.size = Pt(16)
        run.font.color.rgb = RGBColor(0x0F, 0x29, 0x4A)
        return p

    def add_h2(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(10)
        p.paragraph_format.space_after = Pt(4)
        p.paragraph_format.keep_with_next = True
        run = p.add_run(text)
        run.bold = True
        run.font.name = 'Calibri'
        run.font.size = Pt(13)
        run.font.color.rgb = RGBColor(0x1E, 0x40, 0xAF)
        return p

    def add_h3(text):
        p = doc.add_paragraph()
        p.paragraph_format.space_before = Pt(8)
        p.paragraph_format.space_after = Pt(2)
        p.paragraph_format.keep_with_next = True
        run = p.add_run(text)
        run.bold = True
        run.font.name = 'Calibri'
        run.font.size = Pt(11.5)
        run.font.color.rgb = RGBColor(0x33, 0x41, 0x55)
        return p

    def add_code_block(text):
        tbl = doc.add_table(rows=1, cols=1)
        tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
        cell = tbl.cell(0, 0)
        cell.width = Inches(6.5)
        set_cell_background(cell, 'F8FAFC')
        set_cell_margins(cell, top=100, bottom=100, left=150, right=150)
        p = cell.paragraphs[0]
        p.paragraph_format.line_spacing = 1.05
        p.paragraph_format.space_after = Pt(0)
        run = p.add_run(text)
        run.font.name = 'Consolas'
        run.font.size = Pt(8.5)
        run.font.color.rgb = RGBColor(0x0F, 0x17, 0x2A)
        p_after = doc.add_paragraph()
        p_after.paragraph_format.space_before = Pt(0)
        p_after.paragraph_format.space_after = Pt(4)

    def style_table(tbl, col_widths, header_bg='0F294A', zebra_bg='F8FAFC'):
        tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
        for r_idx, row in enumerate(tbl.rows):
            is_header = (r_idx == 0)
            for c_idx, cell in enumerate(row.cells):
                if c_idx < len(col_widths):
                    cell.width = col_widths[c_idx]
                set_cell_margins(cell, top=80, bottom=80, left=120, right=120)
                if is_header:
                    set_cell_background(cell, header_bg)
                    for p in cell.paragraphs:
                        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
                        for r in p.runs:
                            r.bold = True
                            r.font.name = 'Calibri'
                            r.font.size = Pt(9.5)
                            r.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)
                else:
                    if r_idx % 2 == 1:
                        set_cell_background(cell, 'FFFFFF')
                    else:
                        set_cell_background(cell, zebra_bg)
                    for p in cell.paragraphs:
                        for r in p.runs:
                            r.font.name = 'Calibri'
                            r.font.size = Pt(9.5)
        p_after = doc.add_paragraph()
        p_after.paragraph_format.space_after = Pt(4)

    # ==================== COVER PAGE ====================
    p_uni = doc.add_paragraph()
    p_uni.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_uni.paragraph_format.space_before = Pt(20)
    p_uni.paragraph_format.space_after = Pt(2)
    r_uni = p_uni.add_run("EAST WEST UNIVERSITY")
    r_uni.bold = True
    r_uni.font.size = Pt(22)
    r_uni.font.color.rgb = RGBColor(0x0B, 0x25, 0x45)

    p_dept = doc.add_paragraph()
    p_dept.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_dept.paragraph_format.space_after = Pt(4)
    r_dept = p_dept.add_run("DEPARTMENT OF COMPUTER SCIENCE & ENGINEERING")
    r_dept.bold = True
    r_dept.font.size = Pt(13)
    r_dept.font.color.rgb = RGBColor(0x33, 0x41, 0x55)

    p_course = doc.add_paragraph()
    p_course.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_course.paragraph_format.space_after = Pt(12)
    r_course = p_course.add_run("COURSE: CSE360 – COMPUTER ARCHITECTURE  |  SEMESTER: FALL 2026")
    r_course.font.size = Pt(11)
    r_course.font.color.rgb = RGBColor(0x47, 0x55, 0x69)

    p_line = doc.add_paragraph()
    p_line.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_line.paragraph_format.space_after = Pt(16)
    r_line = p_line.add_run("____________________________________________________________________")
    r_line.font.color.rgb = RGBColor(0x94, 0xA3, 0xB8)

    p_rep = doc.add_paragraph()
    p_rep.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_rep.paragraph_format.space_after = Pt(8)
    r_rep = p_rep.add_run("MINI-PROJECT REPORT")
    r_rep.bold = True
    r_rep.font.size = Pt(14)
    r_rep.font.color.rgb = RGBColor(0x1E, 0x40, 0xAF)

    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_title.paragraph_format.space_after = Pt(24)
    r_title = p_title.add_run("Suggest and design a minimal CPU architecture, including ISA, datapath, and control unit, for automating dishwasher control functions with sensor inputs, and verify it with a simulator.")
    r_title.bold = True
    r_title.font.size = Pt(15)
    r_title.font.color.rgb = RGBColor(0x0F, 0x17, 0x2A)

    # Metadata Submission Box Table
    meta_table = doc.add_table(rows=1, cols=2)
    meta_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    cell_by = meta_table.cell(0, 0)
    cell_to = meta_table.cell(0, 1)
    cell_by.width = Inches(3.25)
    cell_to.width = Inches(3.25)
    set_cell_background(cell_by, 'F8FAFC')
    set_cell_background(cell_to, 'F8FAFC')
    set_cell_margins(cell_by, top=140, bottom=140, left=180, right=180)
    set_cell_margins(cell_to, top=140, bottom=140, left=180, right=180)

    p_by = cell_by.paragraphs[0]
    p_by.paragraph_format.line_spacing = 1.2
    r_by_h = p_by.add_run("SUBMITTED BY:\n")
    r_by_h.bold = True
    r_by_h.font.color.rgb = RGBColor(0x0F, 0x29, 0x4A)
    p_by.add_run("• Nusrat Rahman Aurna\n  Student ID: 2023-3-60-057\n  2023-3-60-057@std.ewubd.edu\n\n• Pulok Akibuzzaman\n  Student ID: 2023-3-60-051\n  2023-3-60-051@std.ewubd.edu")

    p_to = cell_to.paragraphs[0]
    p_to.paragraph_format.line_spacing = 1.2
    r_to_h = p_to.add_run("SUBMITTED TO:\n")
    r_to_h.bold = True
    r_to_h.font.color.rgb = RGBColor(0x0F, 0x29, 0x4A)
    p_to.add_run("Dr. Nawab Yousuf Ali\nProfessor\nDepartment of Computer Science & Engineering\nEast West University\nDhaka, Bangladesh")

    p_date = doc.add_paragraph()
    p_date.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_date.paragraph_format.space_before = Pt(30)
    p_date.paragraph_format.space_after = Pt(2)
    r_date = p_date.add_run("Date of Submission: November 2026   |   Semester: Fall 2026")
    r_date.font.size = Pt(10)
    r_date.font.color.rgb = RGBColor(0x64, 0x74, 0x8B)

    doc.add_page_break()

    # ==================== TABLE OF CONTENTS ====================
    add_h1("Table of Contents")
    toc_data = [
        ("SL.", "Table of Contents / Section Title", "Page No."),
        ("1", "Objective", "02"),
        ("2", "Theory", "02"),
        ("2.1", "The Application-Specific Instruction-set Processor (ASIP) Approach", "02"),
        ("2.2", "Architectural Paradigms: Modified Harvard vs. Von Neumann", "03"),
        ("2.3", "CPU Component Organization & Structural Subsystems", "03"),
        ("2.4", "The Three-Phase Instruction Cycle (Fetch-Decode-Execute Micro-Operations)", "04"),
        ("2.5", "Table of Instructions & Architectural Semantics", "05"),
        ("2.6", "Cyber-Physical Plant Dynamics (Hydrological & Thermodynamic Equations)", "05"),
        ("3", "Design", "06"),
        ("3.1", "Datapath and Control Unit Block Diagram", "06"),
        ("3.2", "Instruction Set Architecture (ISA) Binary Encoding & Specification", "07"),
        ("3.3", "Hardware I/O Port Mapping (Sensors & Actuators)", "08"),
        ("3.4", "Dishwasher Control Finite State Machine (FSM) & Flowchart", "09"),
        ("3.5", "Instruction Memory (ROM) Firmware Listing", "10"),
        ("4", "Implementation", "11"),
        ("4.1", "System Requirements, Toolchain, and Environment", "11"),
        ("4.2", "Hierarchical Modular Architecture", "11"),
        ("4.3", "Complete Verified Source Code (main.c)", "12"),
        ("5", "Debugging and Test-Run", "15"),
        ("5.1", "Verification Methodology & Safety Assertions", "15"),
        ("5.2", "Test Scenarios & Fault Tolerance Verification", "15"),
        ("5.3", "Complete Step-by-Step Clock Cycle Trace Table", "16"),
        ("5.4", "Expected Execution vs. Observed Output Verification", "17"),
        ("6", "Results Analysis", "18"),
        ("6.1", "Computational Complexity (Time and Space)", "18"),
        ("6.2", "Robustness, Determinism, and Fail-Safe Verification", "18"),
        ("6.3", "Edge Cases & Error Recovery", "19"),
        ("7", "Conclusion and Future Improvements", "20"),
        ("7.1", "Summary of Project Achievements", "20"),
        ("7.2", "Architectural Limitations", "20"),
        ("7.3", "Future Enhancements", "20"),
        ("8", "Bibliography", "21")
    ]
    t_toc = doc.add_table(rows=len(toc_data), cols=3)
    for r_i, row in enumerate(toc_data):
        for c_i, val in enumerate(row):
            t_toc.cell(r_i, c_i).text = val
    style_table(t_toc, [Inches(0.8), Inches(4.7), Inches(1.0)])

    # ==================== 1. OBJECTIVE ====================
    add_h1("1. Objective")
    doc.add_paragraph(
        "The overarching objective of this mini-project is to formally suggest and design a minimal CPU architecture, "
        "including ISA, datapath, and control unit, for automating dishwasher control functions with sensor inputs, and verify it with a simulator, "
        "strictly fulfilling the academic guidelines of CSE360: Computer Architecture at East West University."
    )
    doc.add_paragraph("Specifically, the project aims to fulfill the following technical goals:")
    doc.add_paragraph("1. Architectural ISA Formulation: Define an ultra-lean 5-instruction ISA (READ_SENSOR, SET_DEVICE, COMPARE_VALUE, JUMP_IF_NOT_EQUAL, HALT_CPU) specialized for embedded closed-loop automation, avoiding redundant silicon area, gate complexity, and energy dissipation.")
    doc.add_paragraph("2. Datapath & Register Synthesis: Construct an accumulator-style datapath comprising a Program Counter (PC), Instruction Register (IR), single accumulator Data Register (R0), and an Arithmetic/Logic Zero Flag (ZF).")
    doc.add_paragraph("3. Synchronous Control Unit Implementation: Realize a deterministic Fetch -> Decode -> Execute execution pipeline delivering single-cycle micro-operations with zero pipelining hazards or memory stalls.")
    doc.add_paragraph("4. Closed-Loop Appliance Automation: Formulate firmware coordinating all operating stages: door safety interlocking, solenoid tub filling (50L), heated wash agitation (55°C), dirty water evacuation (0L), forced hot air drying (30°C), and audio completion signaling.")
    doc.add_paragraph("5. Cyber-Physical Feedback Modeling: Integrate physical tub water volume accumulation and thermodynamic heating equations directly with the processor I/O bus.")
    doc.add_paragraph("6. Verification via Software Simulator in C: Implement and verify the entire design in an ultra-readable, self-documenting ANSI C simulator under 110 lines of code, confirming complete execution in exactly 45 clock cycles.")

    # ==================== 2. THEORY ====================
    add_h1("2. Theory")
    add_h2("2.1 The Application-Specific Instruction-set Processor (ASIP) Approach")
    doc.add_paragraph(
        "Standard commercial microprocessors (e.g., ARM Cortex-M or x86) implement expansive instruction sets with multi-register files, "
        "floating-point pipelines, and complex addressing modes. In dedicated household appliances, over 90% of these silicon features remain idle, "
        "introducing unnecessary manufacturing cost, static leakage currents, and software vulnerabilities. "
        "By tailoring the Instruction Set Architecture (ISA) explicitly to dishwasher automation, the ASIP approach achieves minimal gate count, "
        "deterministic single-cycle execution, and high electrical efficiency."
    )

    add_h2("2.2 Architectural Paradigms: Modified Harvard vs. Von Neumann")
    doc.add_paragraph(
        "In a conventional Von Neumann machine, code and data share a single physical bus and memory address space. During memory-intensive cycles, "
        "instruction fetches and operand reads contend for the bus (the classical Von Neumann bottleneck). "
        "AquaCore-8 utilizes a Modified Harvard architecture: instructions reside in fixed Read-Only Memory (ROM), while sensor inputs and actuator "
        "latches operate over separate dedicated I/O channels. This enables non-blocking single-cycle instruction execution with perfectly predictable timing."
    )

    add_h2("2.3 CPU Component Organization & Structural Subsystems")
    doc.add_paragraph(
        "The CPU architecture integrates six functional blocks: Control Unit, Arithmetic/Comparator Unit (ALU), Internal Registers (PC, IR, R0, ZF), "
        "Program Memory (ROM), Sensor Input Ports, and Actuator Output Ports as illustrated below:"
    )
    add_code_block(
"""+-------------------------------------------------------------------------+
|                                  CPU                                    |
|                                                                         |
|  +--------------------+         +------------------------------------+  |
|  |    Control Unit    |<------->|    Arithmetic / Comparator Unit    |  |
|  |     (F-D-E)        |         |               (ALU)                |  |
|  +--------------------+         +------------------------------------+  |
|            ^                                       ^                    |
|            |                                       |                    |
|            v                                       v                    |
|  +-------------------------------------------------------------------+  |
|  |                        Internal Registers                         |  |
|  |   • Program Counter (PC)          • Instruction Register (IR)     |  |
|  |   • Data Register / ACC (R0)      • Zero Flag (ZF)                |  |
|  +-------------------------------------------------------------------+  |
+------------------------------------+------------------------------------+
                   ^                 |                  ^
                   | Instruction Bus | Address Bus      | Control / Data Bus
                   v                 v                  v
        +---------------------+   +--------------------+--------------------+
        |  Program Memory     |   |    Input Ports     |    Output Ports    |
        |      (ROM)          |   |   (Plant Sensors)  |  (Plant Actuators) |
        +---------------------+   +--------------------+--------------------+"""
    )

    add_h2("2.4 The Three-Phase Instruction Cycle (Fetch-Decode-Execute Micro-Operations)")
    doc.add_paragraph("Each clock cycle t executes the following micro-operations sequentially:")
    doc.add_paragraph("1. Fetch Phase: IR <- ROM[PC]; PC <- PC + 1")
    doc.add_paragraph("2. Decode Phase: Opcode <- IR.operation_code; Operand <- IR.operand")
    doc.add_paragraph("3. Execute Phase:")
    doc.add_paragraph("   • READ_SENSOR: R0 <- Sensors[Operand]")
    doc.add_paragraph("   • SET_DEVICE: active_hardware_device <- Operand")
    doc.add_paragraph("   • COMPARE_VALUE: ZF <- (R0 == Operand)")
    doc.add_paragraph("   • JUMP_IF_NOT_EQUAL: if (!ZF) PC <- Operand")
    doc.add_paragraph("   • HALT_CPU: is_processor_halted <- 1")

    add_h2("2.5 Table of Instructions & Architectural Semantics")
    sem_data = [
        ("Instruction Mnemonic", "Opcode ID", "Operand Type", "Formal RTL Semantics", "Architectural Effect"),
        ("READ_SENSOR", "0", "Sensor Port ID (0..2)", "R0 <- Sensors[operand]", "Transfers sensor channel reading into accumulator R0."),
        ("SET_DEVICE", "1", "Device Port ID (0..4)", "active_device <- operand", "Latches output port to energize targeted actuator."),
        ("COMPARE_VALUE", "2", "8-bit Immediate Constant", "ZF <- (R0 == operand)", "Evaluates accumulator; sets Zero Flag to 1 on match, 0 otherwise."),
        ("JUMP_IF_NOT_EQUAL", "3", "ROM Target Address", "if (!ZF) PC <- operand", "Conditional branch: loops to target address while condition is unmet."),
        ("HALT_CPU", "4", "0 (Ignored)", "is_halted <- 1", "De-asserts CPU clock run line; halts processor execution.")
    ]
    t_sem = doc.add_table(rows=len(sem_data), cols=5)
    for r_i, row in enumerate(sem_data):
        for c_i, val in enumerate(row):
            t_sem.cell(r_i, c_i).text = val
    style_table(t_sem, [Inches(1.5), Inches(0.8), Inches(1.3), Inches(1.5), Inches(1.4)])

    add_h2("2.6 Cyber-Physical Plant Dynamics (Hydrological & Thermodynamic Equations)")
    doc.add_paragraph(
        "To verify closed-loop behavior, the software simulator incorporates discrete physical plant accumulation equations:\n"
        "• Hydrological Tub Water Balance: L(t+1) = L(t) + 10 * u_valve(t) - 10 * u_drain(t), bounded in [0, 50] Liters.\n"
        "• Thermodynamic Water Temperature: T(t+1) = T(t) + 5 * u_heater(t) - 5 * u_fan(t), starting at 20°C and bounded in [20, 55] °C."
    )

    # ==================== 3. DESIGN ====================
    add_h1("3. Design")
    add_h2("3.1 Datapath and Control Unit Block Diagram")
    doc.add_paragraph("The detailed datapath organization, internal registers, and control routing network are modeled as follows:")
    add_code_block(
"""                  +-------------------------------------------------------+
                  |                 Program Counter (PC)                  |
                  +-------------------------------------------------------+
                                              |
                                              v (Address Bus)
                  +-------------------------------------------------------+
                  |               Instruction Memory (ROM)                |
                  +-------------------------------------------------------+
                                              |
                                              v (Instruction Word)
                  +-------------------------------------------------------+
                  |               Instruction Register (IR)               |
                  |     [ 3-bit Opcode (7:5)  |  5-bit Operand (4:0) ]    |
                  +-------------------------------------------------------+
                            |                                  |
               Opcode [7:5] |                     Operand [4:0]|
                            v                                  v
                  +--------------------+             +--------------------+
                  |    Control Unit    |             |    Multiplexer /   |
                  |      Decoder       |             |   Routing Network  |
                  +--------------------+             +--------------------+
                     |      |       |                   |       |      |
             Control |      |       +-------------------+       |      |
             Signals v      |       |                           |      v
                  +-----+   |       v [Read Sensor Port]        |  +-------------------+
                  | R0  |<--+   +-------------------+           |  | Actuator Latches  |
                  +-----+       |   Sensor Inputs   |           |  | • Water Valve (0) |
                     |          | • Door Switch (0) |           |  | • Heater/Pump (1) |
                     v          | • Water Level (1) |           |  | • Drain Pump  (2) |
                  +-----+       | • Temperature (2) |           |  | • Drying Fan  (3) |
                  | ALU |<------+-------------------+           |  | • Buzzer      (4) |
                  | CMP |<--------------------------------------+  +-------------------+
                  +-----+            Immediate Operand Constant
                     |
                     v
                +---------+
                |Zero Flag|---------------------------------------+
                |  (ZF)   |                                       |
                +---------+                                       v
                     |                                   (Branch Feedback)
                     +----------------------------------> To PC Multiplexer"""
    )

    add_h2("3.2 Instruction Set Architecture (ISA) Binary Encoding & Specification")
    doc.add_paragraph("The CPU uses an 8-bit instruction word: Bits [7:5] represent the 3-bit Opcode, and Bits [4:0] represent the 5-bit Operand.")
    isa_data = [
        ("Opcode Mnemonic", "Opcode [7:5]", "Operand [4:0]", "Hex Code", "Micro-Operation / Action"),
        ("READ_SENSOR", "000", "Port ID (00000..00010)", "0x00 - 0x02", "R0 <- SensorPort[operand]"),
        ("SET_DEVICE", "001", "Device ID (00000..00100)", "0x20 - 0x24", "active_device <- operand"),
        ("COMPARE_VALUE", "010", "Immediate Value (0..31)", "0x40 - 0x5F", "ZF <- (R0 == operand)"),
        ("JUMP_IF_NOT_EQUAL", "011", "ROM Target Address (0..31)", "0x60 - 0x7F", "if (!ZF) PC <- operand"),
        ("HALT_CPU", "100", "00000 (Unused)", "0x80", "is_halted <- 1")
    ]
    t_isa = doc.add_table(rows=len(isa_data), cols=5)
    for r_i, row in enumerate(isa_data):
        for c_i, val in enumerate(row):
            t_isa.cell(r_i, c_i).text = val
    style_table(t_isa, [Inches(1.5), Inches(1.1), Inches(1.5), Inches(0.9), Inches(1.5)])

    add_h2("3.3 Hardware I/O Port Mapping (Sensors & Actuators)")
    doc.add_paragraph("Sensor Input Port Map:")
    s_data = [
        ("Port Channel", "Symbolic Identifier", "Port ID", "Physical Transducer", "Operational Range & Units"),
        ("PORT_IN_0", "SENSOR_DOOR", "0", "Safety door interlock microswitch", "1 = Closed, 0 = Open"),
        ("PORT_IN_1", "SENSOR_WATER_LEVEL", "1", "Hydrostatic level sensor", "0 to 50 Liters (10L increments)"),
        ("PORT_IN_2", "SENSOR_WATER_TEMP", "2", "NTC thermistor probe", "20°C to 55°C (5°C increments)")
    ]
    t_s = doc.add_table(rows=len(s_data), cols=5)
    for r_i, row in enumerate(s_data):
        for c_i, val in enumerate(row):
            t_s.cell(r_i, c_i).text = val
    style_table(t_s, [Inches(1.1), Inches(1.5), Inches(0.7), Inches(1.8), Inches(1.4)])

    doc.add_paragraph("Actuator Output Port Map:")
    a_data = [
        ("Port Channel", "Symbolic Identifier", "Port ID", "Electrical Subsystem Driven", "Physical Impact per Active Tick"),
        ("PORT_OUT_0", "WATER_INLET_VALVE", "0", "24V DC solenoid intake valve", "+10 Liters/cycle tub volume"),
        ("PORT_OUT_1", "HEATER_AND_PUMP", "1", "2.2kW heating coil & spray wash pump", "+5 °C/cycle water temperature"),
        ("PORT_OUT_2", "DRAIN_PUMP", "2", "Centrifugal drainage evacuation pump", "-10 Liters/cycle tub volume"),
        ("PORT_OUT_3", "DRYING_FAN", "3", "Forced hot-air convection blower fan", "-5 °C/cycle cooldown towards 30°C"),
        ("PORT_OUT_4", "FINISH_BUZZER", "4", "Piezoelectric completion audio beeper", "Acoustic signal to user (cycle done)")
    ]
    t_a = doc.add_table(rows=len(a_data), cols=5)
    for r_i, row in enumerate(a_data):
        for c_i, val in enumerate(row):
            t_a.cell(r_i, c_i).text = val
    style_table(t_a, [Inches(1.1), Inches(1.5), Inches(0.7), Inches(1.8), Inches(1.4)])

    add_h2("3.4 Dishwasher Control Finite State Machine (FSM) & Flowchart")
    add_code_block(
"""                 [ START / POWER-ON RESET ]
                              |
                              v
                +----------------------------+
                |    STATE 0: SAFETY CHECK   |
                |   Read Door Sensor (Port 0)|
                +----------------------------+
                              |
                     Is Door Closed (== 1)?
                     /                      \\
               NO   /                        \\  YES
                   v                          v
            [ Hold / Poll ]     +----------------------------+
                   ^            |    STATE 1: FILL TUB       |
                   |            | Turn ON Water Inlet Valve  |
                   +-- Loops -- | Read Water Level (Port 1)  |
                                +----------------------------+
                                              |
                                     Is Level == 50 Liters?
                                     /                      \\
                               NO   /                        \\  YES
                                   v                          v
                            [ Continue Fill ]   +----------------------------+
                                                |    STATE 2: WASH & HEAT    |
                                                | Turn ON Heater & Wash Pump |
                                                | Read Water Temp (Port 2)   |
                                                +----------------------------+
                                                              |
                                                      Is Temp == 55 Celsius?
                                                      /                      \\
                                                NO   /                        \\  YES
                                                    v                          v
                                             [ Continue Heat ]  +----------------------------+
                                                                |    STATE 3: DRAIN TUB      |
                                                                | Turn ON Drain Pump         |
                                                                | Read Water Level (Port 1)  |
                                                                +----------------------------+
                                                                              |
                                                                     Is Level == 0 Liters?
                                                                     /                     \\
                                                               NO   /                       \\  YES
                                                                   v                         v
                                                            [ Continue Drain ] +----------------------------+
                                                                               |    STATE 4: DRY & COOL     |
                                                                               | Turn ON Forced Drying Fan  |
                                                                               | Read Water Temp (Port 2)   |
                                                                               +----------------------------+
                                                                                             |
                                                                                     Is Temp == 30 Celsius?
                                                                                     /                      \\
                                                                               NO   /                        \\  YES
                                                                                   v                          v
                                                                            [ Continue Dry ]   +----------------------------+
                                                                                               |    STATE 5: HALT & BUZZ    |
                                                                                               | Turn ON Finish Buzzer      |
                                                                                               | Execute HALT_CPU (Opcode 4)|
                                                                                               +----------------------------+
                                                                                                             |
                                                                                                             v
                                                                                                       [ CPU HALTED ]"""
    )

    add_h2("3.5 Instruction Memory (ROM) Firmware Listing")
    rom_data = [
        ("ROM Address", "Assembly Code", "Opcode ID", "Operand", "Functional Purpose"),
        ("0x00", "READ_SENSOR SENSOR_DOOR", "0", "0", "Fetch door contact switch status into R0"),
        ("0x01", "COMPARE_VALUE 1", "2", "1", "Test if door is closed (R0 == 1); update ZF"),
        ("0x02", "JUMP_IF_NOT_EQUAL 0", "3", "0", "Safety Interlock: hold at 0x00 while door open"),
        ("0x03", "SET_DEVICE WATER_INLET_VALVE", "1", "0", "Open solenoid valve to start water filling"),
        ("0x04", "READ_SENSOR SENSOR_WATER_LEVEL", "0", "1", "Read water volume in Liters into R0"),
        ("0x05", "COMPARE_VALUE 50", "2", "50", "Compare tub volume against 50L capacity"),
        ("0x06", "JUMP_IF_NOT_EQUAL 4", "3", "4", "Loop to 0x04 until tub reaches 50 Liters"),
        ("0x07", "SET_DEVICE HEATER_AND_PUMP", "1", "1", "Energize heating element and wash pump"),
        ("0x08", "READ_SENSOR SENSOR_WATER_TEMP", "0", "2", "Read water temperature in Celsius into R0"),
        ("0x09", "COMPARE_VALUE 55", "2", "55", "Compare water temperature against 55°C"),
        ("0x0A", "JUMP_IF_NOT_EQUAL 8", "3", "8", "Loop to 0x08 until water reaches 55°C"),
        ("0x0B", "SET_DEVICE DRAIN_PUMP", "1", "2", "Energize centrifugal drain pump"),
        ("0x0C", "READ_SENSOR SENSOR_WATER_LEVEL", "0", "1", "Read water level into R0"),
        ("0x0D", "COMPARE_VALUE 0", "2", "0", "Compare volume against 0L (empty tub)"),
        ("0x0E", "JUMP_IF_NOT_EQUAL 12", "3", "12", "Loop to 0x0C until water is fully drained"),
        ("0x0F", "SET_DEVICE DRYING_FAN", "1", "3", "Energize forced convection hot-air fan"),
        ("0x10", "READ_SENSOR SENSOR_WATER_TEMP", "0", "2", "Read temperature sensor into R0"),
        ("0x11", "COMPARE_VALUE 30", "2", "30", "Compare temperature against 30°C cooldown"),
        ("0x12", "JUMP_IF_NOT_EQUAL 16", "3", "16", "Loop to 0x10 until dishes cool to 30°C"),
        ("0x13", "SET_DEVICE FINISH_BUZZER", "1", "4", "Sound audio buzzer signaling completion"),
        ("0x14", "HALT_CPU 0", "4", "0", "Halt processor clock and terminate execution")
    ]
    t_rom = doc.add_table(rows=len(rom_data), cols=5)
    for r_i, row in enumerate(rom_data):
        for c_i, val in enumerate(row):
            t_rom.cell(r_i, c_i).text = val
    style_table(t_rom, [Inches(0.9), Inches(2.2), Inches(0.7), Inches(0.7), Inches(2.0)])

    # ==================== 4. IMPLEMENTATION ====================
    add_h1("4. Implementation")
    add_h2("4.1 System Requirements, Toolchain, and Environment")
    doc.add_paragraph("• Language: ANSI C (C99 / C11 Standard).")
    doc.add_paragraph("• Compilers: GCC (MinGW / Linux GCC), Clang/LLVM, MSVC.")
    doc.add_paragraph("• Operating Systems: Windows 10/11, Ubuntu Linux 22.04 LTS, macOS.")
    doc.add_paragraph("• Build Command: gcc -Wall -Wextra main.c -o aquacore8_sim.exe")

    add_h2("4.2 Hierarchical Modular Architecture")
    doc.add_paragraph("The simulator is structured into 6 logical modules:")
    doc.add_paragraph("1. Module 1: ISA & Port Enumeration (Enums for opcodes, sensors, actuators, Instruction struct).")
    doc.add_paragraph("2. Module 2: Datapath Registers & Processor State (program_counter, instruction_reg, data_register, zero_flag, is_processor_halted).")
    doc.add_paragraph("3. Module 3: Instruction Memory ROM (instruction_memory[] array).")
    doc.add_paragraph("4. Module 4: Control Unit Execution Engine (execute_cpu_cycle() implementing F-D-E).")
    doc.add_paragraph("5. Module 5: Cyber-Physical Plant Model (simulate_dishwasher_hardware() updating volume and temperature).")
    doc.add_paragraph("6. Module 6: Main Execution Driver & Telemetry Logger (main() executing synchronous clock ticks and logging state transitions).")

    add_h2("4.3 Complete Verified Source Code (main.c)")
    with open(r'f:\EWU\CSE360\Mini-Project\main.c', 'r', encoding='utf-8') as f:
        src_code = f.read()
    add_code_block(src_code)

    # ==================== 5. DEBUGGING AND TEST-RUN ====================
    add_h1("5. Debugging and Test-Run")
    add_h2("5.1 Verification Methodology & Safety Assertions")
    doc.add_paragraph(
        "Verification was performed using cycle-accurate state assertions ensuring that every control signal adheres strictly "
        "to safety bounds. When the door is opened (is_door_closed = 0), the Program Counter stays trapped in the safety polling loop (0x00 - 0x02) "
        "and never energizes the water valve. Furthermore, all physical parameters (water volume 0-50L, temperature 20-55°C) are strictly bounded."
    )

    add_h2("5.2 Test Scenarios & Dynamic Execution Modes")
    doc.add_paragraph("The simulator provides three execution modes to thoroughly verify all architectural states and safety constraints:")
    doc.add_paragraph("• Mode 1 (Standard Automated Run): Executes the verified academic baseline with default factory conditions (is_door_closed = 1, Temp = 20°C, Water Level = 0L), completing in exactly 45 clock cycles.")
    doc.add_paragraph("• Mode 2 (Custom Environment & Sensor Input): Allows dynamic configuration of starting water temperature (15°C - 40°C), initial water volume (0 - 40L), and enables step-by-step clock execution where pressing Enter advances one clock cycle while displaying current PC, IR, R0, and ZF register values.")
    doc.add_paragraph("• Mode 3 (Live Safety Interlock Verification): Starts with the safety door open (is_door_closed = 0). The CPU remains trapped in a safe polling loop (PC: 0 -> 2) with zero actuator power until the user presses Enter to close the door, at which point the CPU transitions to State 1 and completes the cycle without error.")

    add_h2("5.3 Complete Step-by-Step Clock Cycle Trace Table")
    trace_data = [
        ("Clock Cycle", "PC Value", "Water Level", "Water Temp", "Active Hardware Device", "Operational Phase & Action"),
        ("0 - 2", "0 - 2", "0L", "20°C", "Standby (Idle)", "Phase 0: Door closed confirmed (ZF = 1)"),
        ("3", "4", "10L", "20°C", "Water Inlet Valve (Filling)", "Phase 1: Solenoid valve energized, water begins filling"),
        ("4 - 12", "4 - 6", "20L - 50L", "20°C", "Water Inlet Valve (Filling)", "Phase 1: Tub fills to 50L (5 filling ticks)"),
        ("13", "8", "50L", "25°C", "Heater & Spray Pump (Washing)", "Phase 2: Tub full (50L); heating element energized"),
        ("14 - 22", "8 - 10", "50L", "30°C - 55°C", "Heater & Spray Pump (Washing)", "Phase 2: Water heated to 55°C (7 heating ticks)"),
        ("23", "12", "40L", "55°C", "Drain Pump (Emptying)", "Phase 3: Wash completed; drain pump energized"),
        ("24 - 32", "12 - 14", "30L - 0L", "55°C", "Drain Pump (Emptying)", "Phase 3: Dirty water fully drained (5 drainage ticks)"),
        ("33", "16", "0L", "50°C", "Drying Fan (Drying)", "Phase 4: Tub empty (0L); forced convection fan energized"),
        ("34 - 42", "16 - 18", "0L", "45°C - 30°C", "Drying Fan (Drying)", "Phase 4: Dishes dried and cooled to 30°C (5 drying ticks)"),
        ("43", "20", "0L", "30°C", "Buzzer (Wash Completed)", "Phase 5: Target temperature reached; audio buzzer sounds"),
        ("44", "21", "0L", "30°C", "Buzzer (Wash Completed)", "Phase 5: HALT_CPU executes; clock stopped permanently")
    ]
    t_tr = doc.add_table(rows=len(trace_data), cols=6)
    for r_i, row in enumerate(trace_data):
        for c_i, val in enumerate(row):
            t_tr.cell(r_i, c_i).text = val
    style_table(t_tr, [Inches(0.9), Inches(0.7), Inches(0.9), Inches(0.9), Inches(1.7), Inches(1.4)])

    add_h2("5.4 Expected Execution vs. Observed Output Verification")
    add_code_block(
"""========================================================================================
        Minimal CPU Architecture Simulator for Automated Dishwasher Control            
 Course: CSE360 Computer Architecture | East West University                           
 Title : Suggest & design minimal CPU (ISA, datapath, control unit) for dishwasher     
========================================================================================
Clock Cycle | PC | Water Level | Temperature | Active Hardware Device
------------+----+-------------+-------------+------------------------------------------
      3     |  4 |     10L     |     20C     | Water Inlet Valve (Filling)
     13     |  8 |     50L     |     25C     | Heater & Spray Pump (Washing)
     23     | 12 |     40L     |     55C     | Drain Pump (Emptying)
     33     | 16 |      0L     |     50C     | Drying Fan (Drying)
     43     | 20 |      0L     |     30C     | Buzzer (Wash Completed)
     44     | 21 |      0L     |     30C     | Buzzer (Wash Completed)
========================================================================================
SUCCESS: Dishwasher automation verified! Completed cycle in 45 CPU clock cycles.
========================================================================================"""
    )
    doc.add_paragraph("Outcome: The simulator output perfectly validates the design with 100% precision. The execution concludes at exactly cycle 45.")

    # ==================== 6. RESULTS ANALYSIS ====================
    add_h1("6. Results Analysis")
    add_h2("6.1 Computational Complexity (Time and Space)")
    doc.add_paragraph("• Time Complexity: O(N) where N = 45 CPU clock cycles for the complete automated sequence.")
    doc.add_paragraph("• Space Complexity: O(1) auxiliary working memory requiring only 4 scalar registers (PC, IR, R0, ZF) totaling under 8 bytes of state memory.")

    add_h2("6.2 Robustness, Determinism, and Fail-Safe Verification")
    doc.add_paragraph("• Fail-Safe Dry-Fire Protection: The heating element is physically gated behind the water volume loop; it is impossible for the heater to activate in a dry tub.")
    doc.add_paragraph("• Clock Determinism: Instructions execute in constant single-cycle time with zero branch misprediction penalties.")

    add_h2("6.3 Edge Cases & Error Recovery")
    edge_data = [
        ("Edge Case Scenario", "Architectural Behavior", "System Safeguard"),
        ("Door Unlatched at Start", "R0 = 0 != 1 -> ZF = 0 -> PC <- 0", "Continuous safe polling; zero actuators powered"),
        ("Water Supply Interruption", "Tub volume holds < 50L -> ZF = 0", "Fill loop holds safely; heater never activates"),
        ("Heater Failure", "Temperature cannot reach 55°C", "Drain pump never triggers; system pauses safely"),
        ("Watchdog Cycle Boundary", "Clock reaches 100 ticks", "Hard termination boundary prevents infinite runaways")
    ]
    t_ed = doc.add_table(rows=len(edge_data), cols=3)
    for r_i, row in enumerate(edge_data):
        for c_i, val in enumerate(row):
            t_ed.cell(r_i, c_i).text = val
    style_table(t_ed, [Inches(1.8), Inches(2.2), Inches(2.5)])

    # ==================== 7. CONCLUSION AND FUTURE IMPROVEMENTS ====================
    add_h1("7. Conclusion and Future Improvements")
    add_h2("7.1 Summary of Project Achievements")
    doc.add_paragraph(
        "This project successfully suggests, designs, and verifies AquaCore-8—a minimal CPU architecture (ISA, datapath, and control unit) "
        "specialized for automating dishwasher control functions with sensor inputs. The design proves that a minimal 5-instruction processor "
        "can orchestrate full closed-loop appliance automation in 45 clock cycles with complete fail-safe robustness."
    )

    add_h2("7.2 Architectural Limitations")
    doc.add_paragraph("• Single accumulator register necessitates sequential evaluation of physical sensors.")
    doc.add_paragraph("• Polling-based sensor evaluation requires busy loops rather than asynchronous hardware interrupts.")

    add_h2("7.3 Future Enhancements")
    doc.add_paragraph("1. Asynchronous Hardware Interrupt Controller (IRQ): For instant sub-cycle response to emergency door openings.")
    doc.add_paragraph("2. Verilog HDL RTL Synthesis: Synthesize the processor for physical deployment on an FPGA (e.g., Xilinx Spartan-7).")
    doc.add_paragraph("3. Optical Turbidity Sensing: Adaptively regulate wash duration based on dish soil density.")
    doc.add_paragraph("4. PWM Motor Speed Modulation: Modulate wash spray pump pressure between delicate glassware and heavy cookware.")

    # ==================== 8. BIBLIOGRAPHY ====================
    add_h1("8. Bibliography")
    bib_items = [
        "Patterson, D. A., & Hennessy, J. L. (2017). Computer Organization and Design: The Hardware/Software Interface (ARM Edition). Morgan Kaufmann / Elsevier.",
        "Stallings, W. (2016). Computer Organization and Architecture: Designing for Performance (10th Edition). Pearson Education.",
        "Tanenbaum, A. S., & Austin, T. (2013). Structured Computer Organization (6th Edition). Pearson Education.",
        "Mano, M. M. (1993). Computer System Architecture (3rd Edition). Prentice-Hall, Inc.",
        "Nisan, N., & Schocken, S. (2021). The Elements of Computing Systems: Building a Modern Computer from First Principles (Nand2Tetris) (2nd Edition). The MIT Press.",
        "Kernighan, B. W., & Ritchie, D. M. (1988). The C Programming Language (2nd Edition). Prentice Hall Software Series.",
        "IEEE Standards Association. (2006). IEEE Standard for Verilog Hardware Description Language (IEEE Std 1364-2005). IEEE."
    ]
    for b in bib_items:
        p_b = doc.add_paragraph()
        p_b.paragraph_format.left_indent = Inches(0.4)
        p_b.paragraph_format.first_line_indent = Inches(-0.4)
        r_b = p_b.add_run(b)
        r_b.font.size = Pt(10)

    # Save Document
    doc.save(r'f:\EWU\CSE360\Mini-Project\Report_CSE360.docx')
    print('SUCCESS: Report_CSE360.docx generated successfully!')

if __name__ == '__main__':
    create_report()
