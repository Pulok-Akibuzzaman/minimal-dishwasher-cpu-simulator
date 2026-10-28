# AquaCore-8: Instruction Set Architecture (ISA) Specification
## CSE360: Computer Architecture Mini-Project Reference

---

### Architectural Highlights
- **Word Width**: 8-bit Data Bus / 16-bit Fixed Instruction Word
- **Address Spaces**:
  - Instruction Memory (ROM): 256 x 16-bit (Word-addressed by PC)
  - Data Memory (RAM): 256 x 8-bit (Byte-addressed)
  - I/O Port Space: 32 Ports (`0x00` - `0x1F`)
- **Registers**:
  - `R0` (00), `R1` (01), `R2` (10), `R3` (11)
  - `PC` (Program Counter, 8-bit)
  - `SP` (Stack Pointer, 16-level deep hardware return stack)
  - `FLAGS`: Zero (Z), Carry (C), Negative (N), Overflow (V)

---

### Complete Instruction Summary Table

| Mnemonic | Format | Opcode [15:12] | Field [11:10] | Field [9:8] | Field [7:6] | Field [5:3] | Field [2:0] | Description |
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---|
| `ADD Rd, Rs, Rt` | R-Type | `0000` | Rd | Rs | Rt | `000` | `000` | Rd = Rs + Rt (flags Z, C, N, V) |
| `SUB Rd, Rs, Rt` | R-Type | `0000` | Rd | Rs | Rt | `001` | `000` | Rd = Rs - Rt (flags Z, C, N, V) |
| `AND Rd, Rs, Rt` | R-Type | `0000` | Rd | Rs | Rt | `010` | `000` | Rd = Rs & Rt (flags Z, N) |
| `OR  Rd, Rs, Rt` | R-Type | `0000` | Rd | Rs | Rt | `011` | `000` | Rd = Rs \| Rt (flags Z, N) |
| `XOR Rd, Rs, Rt` | R-Type | `0000` | Rd | Rs | Rt | `100` | `000` | Rd = Rs ^ Rt (flags Z, N) |
| `CMP Rs, Rt`     | R-Type | `0000` | 00 | Rs | Rt | `101` | `000` | Compare Rs - Rt (flags Z, C, N, V) |
| `NOT Rd, Rs`     | R-Type | `0000` | Rd | Rs | 00 | `110` | `000` | Rd = ~Rs (flags Z, N) |
| `SHL Rd, Rs`     | R-Type | `0000` | Rd | Rs | 00 | `111` | `000` | Shift Left: Rd = Rs << 1 (flag C=MSB) |
| `SHR Rd, Rs`     | R-Type | `0000` | Rd | Rs | 00 | `111` | `001` | Shift Right: Rd = Rs >> 1 (flag C=LSB) |
| `ADDI Rd, #imm8` | I-Type | `0001` | Rd | 00 | Immediate [7:0] | - | - | Rd = Rd + imm8 |
| `SUBI Rd, #imm8` | I-Type | `0010` | Rd | 00 | Immediate [7:0] | - | - | Rd = Rd - imm8 |
| `CMPI Rd, #imm8` | I-Type | `0011` | Rd | 00 | Immediate [7:0] | - | - | Compare Rd - imm8 |
| `ANDI Rd, #imm8` | I-Type | `0100` | Rd | 00 | Immediate [7:0] | - | - | Rd = Rd & imm8 |
| `ORI  Rd, #imm8` | I-Type | `0101` | Rd | 00 | Immediate [7:0] | - | - | Rd = Rd \| imm8 |
| `LDI  Rd, #imm8` | I-Type | `0110` | Rd | 00 | Immediate [7:0] | - | - | Rd = imm8 |
| `LOAD Rd, [addr]`| I-Type | `0111` | Rd | 00 | RAM Address [7:0] | - | - | Rd = RAM[addr8] |
| `STORE [addr],Rs`| I-Type | `1000` | Rs | 00 | RAM Address [7:0] | - | - | RAM[addr8] = Rs |
| `IN   Rd, [port]`| I-Type | `1001` | Rd | 00 | Port Address [7:0]| - | - | Rd = PORT[port8] |
| `OUT  [port],Rs` | I-Type | `1010` | Rs | 00 | Port Address [7:0]| - | - | PORT[port8] = Rs |
| `JZ  target8`    | Branch | `1011` | `0000` (Cond) | - | Target [7:0] | - | - | Jump if Z == 1 |
| `JNZ target8`    | Branch | `1011` | `0001` (Cond) | - | Target [7:0] | - | - | Jump if Z == 0 |
| `JC  target8`    | Branch | `1011` | `0010` (Cond) | - | Target [7:0] | - | - | Jump if C == 1 |
| `JNC target8`    | Branch | `1011` | `0011` (Cond) | - | Target [7:0] | - | - | Jump if C == 0 |
| `JN  target8`    | Branch | `1011` | `0100` (Cond) | - | Target [7:0] | - | - | Jump if N == 1 |
| `JP  target8`    | Branch | `1011` | `0101` (Cond) | - | Target [7:0] | - | - | Jump if N == 0 |
| `JV  target8`    | Branch | `1011` | `0110` (Cond) | - | Target [7:0] | - | - | Jump if V == 1 |
| `JGT target8`    | Branch | `1011` | `0111` (Cond) | - | Target [7:0] | - | - | Jump if Signed Greater Than |
| `JLT target8`    | Branch | `1011` | `1000` (Cond) | - | Target [7:0] | - | - | Jump if Signed Less Than |
| `JGE target8`    | Branch | `1011` | `1001` (Cond) | - | Target [7:0] | - | - | Jump if Signed Greater/Equal |
| `JLE target8`    | Branch | `1011` | `1010` (Cond) | - | Target [7:0] | - | - | Jump if Signed Less/Equal |
| `JMP target8`    | Jump   | `1100` | 00 | 00 | Target [7:0] | - | - | PC = target8 |
| `CALL target8`   | Subr   | `1101` | `00` (Bit 11=0)| - | Target [7:0] | - | - | Stack[SP++] = PC+1; PC = target8 |
| `RET`            | Subr   | `1101` | `10` (Bit 11=1)| - | 00000000 | - | - | PC = Stack[--SP] |
| `WAIT #ticks`    | Delay  | `1110` | 00 | 00 | Ticks [7:0] | - | - | Synchronous hardware delay tick |
| `NOP`            | Sys    | `1111` | `0000` | - | 00000000 | - | - | No Operation |
| `HALT`           | Sys    | `1111` | `0001` | - | 00000000 | - | - | Halts processor clock |
