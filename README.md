# 6502 CPU Emulator

A C++ emulator of the **MOS Technology 6502**, the 8 bit little-endian CPU that powered the Apple II, the Commodore 64 (a close relative), the NES (a close relative) and many others. It addresses up to 64 KB of memory through a 16 bit address bus.

> [!NOTE]
> The **whole official instruction set is implemented**: all 56 instructions, in every addressing mode (151 opcodes), with clock cycle counting.
> Unofficial ("illegal") opcodes and hardware interrupts are not emulated yet, see [Limitations](#limitations).

New to the 6502? The [6502 Reference](#6502-reference) at the end of this file has the registers, flags, addressing modes, an opcode matrix and every instruction with its opcodes, bytes and cycles.

## Objective

Build a simple, readable emulator of the 6502 that can read assembly and execute it with the correct behaviour, flags and timing. The code favours clarity over speed.

## Quick start

```c++
Memory memory;
CPU cpu;
cpu.reset(memory);            // clears memory and registers, PC = 0x0200

// LDX #3 ; LDA #0 ; CLC ; loop: ADC #5 ; DEX ; BNE loop
const Byte program[] = {0xA2, 0x03, 0xA9, 0x00, 0x18, 0x69, 0x05, 0xCA, 0xD0, 0xFB};
for (unsigned i = 0; i < sizeof program; i++)
    memory[0x0200 + i] = program[i];

s32 cycles = cpu.exec(memory); // runs until BRK (the zero filled memory after the program)

// cpu.getAcc() == 15, cpu.getRX() == 0, cycles == 33
```

Opcodes have names in `opcodes.h`, so programs can also be written like this:

```c++
memory[0x0200] = opcodes::LDA;          // LDA #$84
memory[0x0201] = 0x84;
memory[0x0202] = opcodes::STA_ZERO_PAGE; // STA $10
memory[0x0203] = 0x10;
```

JSR example (a subroutine at `0x4242`):

```c++
memory[0x0200] = opcodes::JSR;
memory[0x0201] = 0x42;   // low byte of the address
memory[0x0202] = 0x42;   // high byte of the address
memory[0x4242] = opcodes::LDA;
memory[0x4243] = 0x84;
memory[0x4244] = opcodes::RTS;
```

### Ways to load programs

###### Using the function `loadTestProgram`
Add your code to `void loadTestProgram(Memory &memory)` in `main.cpp`, like the examples above.

###### Using the assembler
Write simple assembly code, for example:
```assembly
LDA #50
LDX #10
```
The assembly code is written to `assembly_code.txt`.

> The assembler is a separate component from the CPU. The CPU understands every official opcode, but the assembler may not know every mnemonic and addressing mode yet.

## How it works

| Topic | Behaviour |
| ----- | --------- |
| **Memory** | 64 KB, `memory[address]` reads and writes a byte. `Memory::init()` fills it with zeros. |
| **Start address** | `reset()` sets the program counter to `0x0200`. |
| **Execution** | `exec()` runs instructions from the program counter **until a `BRK` (opcode `0x00`) has been executed**. Since memory is zero filled, a program ends by itself where its bytes end. An unknown opcode prints `COMMAND NOT FOUND` and stops. |
| **Return value** | `exec()` returns the clock cycles used by the program, `BRK` (7 cycles) included. |
| **Stack** | Lives in `0x0100`–`0x01FF` and grows downwards. `reset()` starts it at `0x01FF`. `StackPointer` holds the full address of the next free slot (`TSX`/`TXS` only see its low byte, like the real 8 bit register). |
| **Flags** | `C` carry, `Z` zero, `I` interrupt disable, `D` decimal mode, `B` break, `V` overflow, `N` negative. |
| **BRK** | Skips a padding byte, pushes the return address (`BRK` + 2) and the status (with `B` set), sets `I` and `B`, loads the PC from `$FFFE/F` and hands control back to the caller of `exec()`. |
| **RTI / PLP** | Restore all the flags from the value on the stack. |
| **Decimal mode** | `SED` makes `ADC` and `SBC` use BCD arithmetic, following the NMOS 6502. |
| **Zero page wrap** | `$FF,X` with `X = 4` reads `$03`, not `$0103`. The pointers of `(zp,X)` and `(zp),Y` wrap inside the zero page too. |
| **`JMP ($xxFF)` bug** | Reproduced like on the original chip: the high byte is taken from `$xx00`, not from `$xx00 + $0100`. |
| **Clock cycles** | Every opcode has its base cycles in `opcodes::opcode_map`. Indexed reads (`abs,X`, `abs,Y`, `(zp),Y`) cost +1 when they cross a page. Taken branches cost +1, and +1 more if they land on another page. |

## Instruction set

All 56 instructions are implemented. The [6502 Reference](#6502-reference) describes each one in detail, with opcodes, bytes and cycles.

| Instruction | Name                        | Addressing modes |
| ----------- | --------------------------- | ---------------- |
| ADC | Add with Carry              | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| AND | Logical AND                 | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| ASL | Arithmetic Shift Left       | Accumulator, Zero Page, Zero Page,X, Absolute, Absolute,X |
| BCC | Branch if Carry Clear       | Relative |
| BCS | Branch if Carry Set         | Relative |
| BEQ | Branch if Equal             | Relative |
| BIT | Bit Test                    | Zero Page, Absolute |
| BMI | Branch if Minus             | Relative |
| BNE | Branch if Not Equal         | Relative |
| BPL | Branch if Positive          | Relative |
| BRK | Force Interrupt             | Implied |
| BVC | Branch if Overflow Clear    | Relative |
| BVS | Branch if Overflow Set      | Relative |
| CLC | Clear Carry Flag            | Implied |
| CLD | Clear Decimal Mode          | Implied |
| CLI | Clear Interrupt Disable     | Implied |
| CLV | Clear Overflow Flag         | Implied |
| CMP | Compare                     | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| CPX | Compare X Register          | Immediate, Zero Page, Absolute |
| CPY | Compare Y Register          | Immediate, Zero Page, Absolute |
| DEC | Decrement Memory            | Zero Page, Zero Page,X, Absolute, Absolute,X |
| DEX | Decrement X Register        | Implied |
| DEY | Decrement Y Register        | Implied |
| EOR | Exclusive OR                | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| INC | Increment Memory            | Zero Page, Zero Page,X, Absolute, Absolute,X |
| INX | Increment X Register        | Implied |
| INY | Increment Y Register        | Implied |
| JMP | Jump                        | Absolute, Indirect |
| JSR | Jump to Subroutine          | Absolute |
| LDA | Load Accumulator            | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| LDX | Load X Register             | Immediate, Zero Page, Zero Page,Y, Absolute, Absolute,Y |
| LDY | Load Y Register             | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X |
| LSR | Logical Shift Right         | Accumulator, Zero Page, Zero Page,X, Absolute, Absolute,X |
| NOP | No Operation                | Implied |
| ORA | Logical Inclusive OR        | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| PHA | Push Accumulator            | Implied |
| PHP | Push Processor Status       | Implied |
| PLA | Pull Accumulator            | Implied |
| PLP | Pull Processor Status       | Implied |
| ROL | Rotate Left                 | Accumulator, Zero Page, Zero Page,X, Absolute, Absolute,X |
| ROR | Rotate Right                | Accumulator, Zero Page, Zero Page,X, Absolute, Absolute,X |
| RTI | Return from Interrupt       | Implied |
| RTS | Return from Subroutine      | Implied |
| SBC | Subtract with Carry         | Immediate, Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| SEC | Set Carry Flag              | Implied |
| SED | Set Decimal Flag            | Implied |
| SEI | Set Interrupt Disable       | Implied |
| STA | Store Accumulator           | Zero Page, Zero Page,X, Absolute, Absolute,X, Absolute,Y, (Indirect,X), (Indirect),Y |
| STX | Store X Register            | Zero Page, Zero Page,Y, Absolute |
| STY | Store Y Register            | Zero Page, Zero Page,X, Absolute |
| TAX | Transfer Accumulator to X   | Implied |
| TAY | Transfer Accumulator to Y   | Implied |
| TSX | Transfer Stack Pointer to X | Implied |
| TXA | Transfer X to Accumulator   | Implied |
| TXS | Transfer X to Stack Pointer | Implied |
| TYA | Transfer Y to Accumulator   | Implied |

### Opcode names

`opcodes.h` names every opcode `<INSTRUCTION>` for immediate/implied mode, or `<INSTRUCTION>_<MODE>` for the others:

| Suffix | Mode | Example |
| ------ | ---- | ------- |
| *(none)* | Immediate / Implied | `opcodes::LDA`, `opcodes::INX` |
| `_ACCUMULATOR` | Accumulator | `opcodes::ASL_ACCUMULATOR` |
| `_ZERO_PAGE`, `_ZERO_PAGE_X`, `_ZERO_PAGE_Y` | Zero Page, Zero Page,X / ,Y | `opcodes::LDA_ZERO_PAGE_X` |
| `_ABSOLUTE`, `_ABSOLUTE_X`, `_ABSOLUTE_Y` | Absolute, Absolute,X / ,Y | `opcodes::STA_ABSOLUTE_X` |
| `_INDIRECT_X`, `_INDIRECT_Y` | (Indirect,X), (Indirect),Y | `opcodes::ADC_INDIRECT_Y` |
| `JMP_INDIRECT` | Indirect | `opcodes::JMP_INDIRECT` |

The older `STA_ZERO_PAGE_ABSOLUTE*` and `STA_ZERO_PAGE_INDIRECT_*` names still work, they are aliases of `STA_ABSOLUTE*` and `STA_INDIRECT_*`.

## Tests

`is_cpu_with_errors(memory, cpu)` (in `testing/TestCPU.cpp`) runs every test and stops at the first one that fails:

```
TESTING CPU...
LDA TEST SUCCESS
...
ALL TESTS PASSED!
```

The tests cover:

- **Every official opcode** in every addressing mode (a generic helper loads the operand for each mode and runs the same check on all of them).
- **Clock cycles** of every instruction, including the page crossing and branch penalties.
- **Flags**: set *and* cleared correctly, carry/overflow of `ADC`/`SBC` (binary and decimal), `BIT`, compares, shifts and rotates.
- **Edge cases**: zero page wrap around, indirect pointer wrap around, the `JMP ($xxFF)` bug, nested `JSR`/`RTS`, `RTI`, `BRK` and the stack.
- The assembler (`assembly_code_unit_test.txt`).

## Limitations

- **Unofficial opcodes** (the 105 undocumented ones, like `LAX` or `SLO`) are not implemented. They print `COMMAND NOT FOUND` and stop `exec()`.
- **Interrupts**: only `BRK`/`RTI` exist. There is no `IRQ`/`NMI` input, and `reset()` does not read the reset vector at `$FFFC/D`, it starts at `0x0200`.
- `exec()` has no cycle budget or step mode: it runs until `BRK`. A program that loops forever never returns.
- `execDebug()` only turns on the CPU debug flag, no extra output yet.

## Docker

#### Build and enter the container:
```
docker-compose build emulator
```
```
docker-compose run emulator
```
At this point it should open a bash terminal inside the container.
Every change to the code is directly available inside the container, so modified scripts can be executed without rebuilding the Docker image.

#### Execute code after entering the container:
Example:
```
root@1c1d29670293:/app# ls
```

#### Stop the container:
```
docker-compose stop emulator
```

## VS Code: build and debug the emulator

**NOTE: this project was built in an Ubuntu environment!**

Create a file `tasks.json` in the `.vscode` folder:
```json
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++ build active file",
            "command": "/usr/bin/g++",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${workspaceFolder}/src/*.cpp",
                "${workspaceFolder}/src/assembler/*.cpp",
                "${workspaceFolder}/src/components/*.cpp",
                "${workspaceFolder}/src/cycles/*.cpp",
                "${workspaceFolder}/src/testing/*.cpp",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Task generated by Debugger."
        }
    ],
    "version": "2.0.0"
}
```

## 6502 Reference

Everything needed to work on this emulator, so there is no need to leave the repository. The facts (opcodes, bytes, cycles, flags) were checked against the Obelisk 6502 guide, and the explanations are written for this project. The original pages are listed in [Original sources](#original-sources).

- [Registers](#registers)
- [Status flags](#status-flags)
- [Memory map](#memory-map)
- [Addressing modes](#addressing-modes)
- [Stack](#stack)
- [Clock cycles](#clock-cycles)
- [Opcode matrix](#opcode-matrix)
- [Instruction reference](#instruction-reference)

### Registers

| Register | Size | Purpose | In this emulator |
| -------- | ---- | ------- | ---------------- |
| **PC** program counter | 16 bit | Address of the next instruction. Changed by jumps, branches, calls and returns. | `getProgramCounter()` / `setProgramCounter()` |
| **S** stack pointer | 8 bit | Low byte of the next free stack slot (the stack is `$0100`-`$01FF`). Goes down on push, up on pull. | `getStackPointer()` holds the full address (`0x01FF` after reset) |
| **A** accumulator | 8 bit | Target of arithmetic and logic. Can be loaded, stored, pushed and pulled. | `getAcc()` / `setAcc()` |
| **X** index | 8 bit | Counter / offset for indexed addressing. The only register that can read or write S (TSX, TXS). | `getRX()` / `setRX()` |
| **Y** index | 8 bit | Counter / offset for indexed addressing. No special functions. | `getRY()` / `setRY()` |
| **P** status | 8 bit | The [status flags](#status-flags). | `getStatusByte()`, `getProcessorFlag()` |

### Status flags

The status register `P` holds seven flags in this layout:

```
bit   7  6  5  4  3  2  1  0
      N  V  -  B  D  I  Z  C
```

| Flag | Bit | Name | Meaning |
| ---- | --- | ---- | ------- |
| **C** | 0 | Carry | Set when an addition carries out of bit 7, when a subtraction does **not** borrow, when a compare finds `register >= value`, and by shifts / rotates with the bit that fell out. |
| **Z** | 1 | Zero | Set when the last result was zero. |
| **I** | 2 | Interrupt disable | While set, maskable interrupts (IRQ) are ignored. |
| **D** | 3 | Decimal mode | While set, ADC and SBC work with BCD (two decimal digits per byte). |
| **B** | 4 | Break | Set by BRK. It only exists in the copy of P that BRK and PHP push on the stack. |
| - | 5 | Unused | Always reads as 1 in a pushed copy of P. |
| **V** | 6 | Overflow | Set when a signed addition or subtraction gave a wrong result (e.g. positive + positive = negative: 64 + 64). Also copied from bit 6 by BIT. |
| **N** | 7 | Negative | A copy of bit 7 of the last result. |

### Memory map

The 6502 is **little endian**: a 16 bit value is stored low byte first (address `$1234` is the bytes `$34 $12`).

| Range | Use |
| ----- | --- |
| `$0000`-`$00FF` | **Zero page.** Reachable with the short, faster zero page modes and used for the pointers of the indirect modes. |
| `$0100`-`$01FF` | **Stack.** Fixed, it cannot be moved. The CPU does not detect overflow, so too many pushes silently wrap around. |
| `$0200`-`$FFF9` | Free memory. **This emulator starts executing at `$0200`** after `reset()`. |
| `$FFFA`-`$FFFB` | NMI vector (not emulated yet). |
| `$FFFC`-`$FFFD` | Reset vector (a real 6502 starts here; this emulator starts at `$0200` instead). |
| `$FFFE`-`$FFFF` | IRQ / BRK vector. BRK loads the PC from here. |

Hardware is not special on the 6502: devices are simply mapped to memory addresses.

### Addressing modes

| Mode | Syntax | Operand | How the target is found |
| ---- | ------ | ------- | ----------------------- |
| Implied | `INX` | none | The instruction already says what it works on. |
| Accumulator | `ASL A` | none | Works on the accumulator. |
| Immediate | `LDA #$0A` | 1 byte | The operand **is** the value. |
| Zero Page | `LDA $80` | 1 byte | The operand is an address in `$0000`-`$00FF`. |
| Zero Page,X | `LDA $80,X` | 1 byte | Operand + X, **wrapping inside the zero page**: `$80,X` with `X = $FF` reads `$007F`, not `$017F`. |
| Zero Page,Y | `LDX $80,Y` | 1 byte | Same as Zero Page,X using Y. Only LDX and STX have it. |
| Relative | `BNE label` | 1 byte | Signed offset (-128..+127) added to the address of the next instruction. Branches only. |
| Absolute | `JMP $1234` | 2 bytes | The operand is the full 16 bit address. |
| Absolute,X | `STA $2000,X` | 2 bytes | Address + X. |
| Absolute,Y | `STA $2000,Y` | 2 bytes | Address + Y. |
| Indirect | `JMP ($0120)` | 2 bytes | The operand points to the two bytes that hold the real target address. Only JMP has it. |
| (Indirect,X) | `LDA ($40,X)` | 1 byte | **Indexed indirect.** Zero page operand + X (wrapping) gives the location of a 16 bit pointer, which is the target. |
| (Indirect),Y | `LDA ($40),Y` | 1 byte | **Indirect indexed.** The zero page operand locates a 16 bit pointer, and Y is added to that pointer to give the target. This is the most used indirect mode. |

### Stack

- **Push:** the byte is written at `$0100 + S`, then S is decremented. **Pull:** S is incremented, then the byte is read.
- 16 bit values are pushed **high byte first**, so the low byte ends up at the lower address.
- `JSR` pushes the return address **minus one**, and `RTS` adds the one back after pulling it.
- `BRK` pushes the PC (address of the BRK + 2) and then the status. `RTI` pulls the status first and then the PC, with nothing added.

### Clock cycles

Cycle counts are the base cycles shown in each instruction table, plus:

- **+1** for `Absolute,X`, `Absolute,Y` and `(Indirect),Y` when the instruction **reads** and adding the index crosses a page boundary (`+` in the tables). Stores and read-modify-write instructions (INC, DEC, shifts) always pay their full, fixed cost.
- **Branches:** 2 cycles if not taken, **+1** if taken, and **+1 more** if the target is on another page than the next instruction.

### Opcode matrix

Find an opcode by its hex digits (row = high digit, column = low digit). Empty cells are unofficial opcodes, which this emulator does not implement.

<details>
<summary>Show the matrix</summary>

| | _0 | _1 | _2 | _3 | _4 | _5 | _6 | _7 | _8 | _9 | _A | _B | _C | _D | _E | _F |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| **0_** | BRK | ORA (zp,X) |  |  |  | ORA zp | ASL zp |  | PHP | ORA # | ASL A |  |  | ORA abs | ASL abs |  |
| **1_** | BPL rel | ORA (zp),Y |  |  |  | ORA zp,X | ASL zp,X |  | CLC | ORA abs,Y |  |  |  | ORA abs,X | ASL abs,X |  |
| **2_** | JSR abs | AND (zp,X) |  |  | BIT zp | AND zp | ROL zp |  | PLP | AND # | ROL A |  | BIT abs | AND abs | ROL abs |  |
| **3_** | BMI rel | AND (zp),Y |  |  |  | AND zp,X | ROL zp,X |  | SEC | AND abs,Y |  |  |  | AND abs,X | ROL abs,X |  |
| **4_** | RTI | EOR (zp,X) |  |  |  | EOR zp | LSR zp |  | PHA | EOR # | LSR A |  | JMP abs | EOR abs | LSR abs |  |
| **5_** | BVC rel | EOR (zp),Y |  |  |  | EOR zp,X | LSR zp,X |  | CLI | EOR abs,Y |  |  |  | EOR abs,X | LSR abs,X |  |
| **6_** | RTS | ADC (zp,X) |  |  |  | ADC zp | ROR zp |  | PLA | ADC # | ROR A |  | JMP (abs) | ADC abs | ROR abs |  |
| **7_** | BVS rel | ADC (zp),Y |  |  |  | ADC zp,X | ROR zp,X |  | SEI | ADC abs,Y |  |  |  | ADC abs,X | ROR abs,X |  |
| **8_** |  | STA (zp,X) |  |  | STY zp | STA zp | STX zp |  | DEY |  | TXA |  | STY abs | STA abs | STX abs |  |
| **9_** | BCC rel | STA (zp),Y |  |  | STY zp,X | STA zp,X | STX zp,Y |  | TYA | STA abs,Y | TXS |  |  | STA abs,X |  |  |
| **A_** | LDY # | LDA (zp,X) | LDX # |  | LDY zp | LDA zp | LDX zp |  | TAY | LDA # | TAX |  | LDY abs | LDA abs | LDX abs |  |
| **B_** | BCS rel | LDA (zp),Y |  |  | LDY zp,X | LDA zp,X | LDX zp,Y |  | CLV | LDA abs,Y | TSX |  | LDY abs,X | LDA abs,X | LDX abs,Y |  |
| **C_** | CPY # | CMP (zp,X) |  |  | CPY zp | CMP zp | DEC zp |  | INY | CMP # | DEX |  | CPY abs | CMP abs | DEC abs |  |
| **D_** | BNE rel | CMP (zp),Y |  |  |  | CMP zp,X | DEC zp,X |  | CLD | CMP abs,Y |  |  |  | CMP abs,X | DEC abs,X |  |
| **E_** | CPX # | SBC (zp,X) |  |  | CPX zp | SBC zp | INC zp |  | INX | SBC # | NOP |  | CPX abs | SBC abs | INC abs |  |
| **F_** | BEQ rel | SBC (zp),Y |  |  |  | SBC zp,X | INC zp,X |  | SED | SBC abs,Y |  |  |  | SBC abs,X | INC abs,X |  |

Mode legend: `#` immediate, `zp` zero page, `zp,X` / `zp,Y` zero page indexed, `abs` absolute, `abs,X` / `abs,Y` absolute indexed, `(abs)` indirect, `(zp,X)` indexed indirect, `(zp),Y` indirect indexed, `A` accumulator, `rel` relative, no suffix = implied.

</details>

### Instruction reference

Each table lists every opcode of the instruction. `Cycles` with a `+` means one extra cycle when a page boundary is crossed. The **Flags** line lists the flags the instruction can change (`C` carry, `Z` zero, `I` interrupt disable, `D` decimal, `B` break, `V` overflow, `N` negative).

<details>
<summary><b>Load and store</b> (LDA, LDX, LDY, STA, STX, STY)</summary>

Move one byte between memory and a register. Loads set Z and N; stores change no flag.

#### LDA - Load Accumulator

`A = M`

Copies a byte from memory (or the immediate value) into the accumulator.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `LDA #$nn` | `$A9` | 2 | 2 |
| Zero Page | `LDA $nn` | `$A5` | 2 | 3 |
| Zero Page,X | `LDA $nn,X` | `$B5` | 2 | 4 |
| Absolute | `LDA $nnnn` | `$AD` | 3 | 4 |
| Absolute,X | `LDA $nnnn,X` | `$BD` | 3 | 4+ |
| Absolute,Y | `LDA $nnnn,Y` | `$B9` | 3 | 4+ |
| (Indirect,X) | `LDA ($nn,X)` | `$A1` | 2 | 6 |
| (Indirect),Y | `LDA ($nn),Y` | `$B1` | 2 | 5+ |

#### LDX - Load X Register

`X = M`

Copies a byte from memory (or the immediate value) into the X register.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `LDX #$nn` | `$A2` | 2 | 2 |
| Zero Page | `LDX $nn` | `$A6` | 2 | 3 |
| Zero Page,Y | `LDX $nn,Y` | `$B6` | 2 | 4 |
| Absolute | `LDX $nnnn` | `$AE` | 3 | 4 |
| Absolute,Y | `LDX $nnnn,Y` | `$BE` | 3 | 4+ |

#### LDY - Load Y Register

`Y = M`

Copies a byte from memory (or the immediate value) into the Y register.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `LDY #$nn` | `$A0` | 2 | 2 |
| Zero Page | `LDY $nn` | `$A4` | 2 | 3 |
| Zero Page,X | `LDY $nn,X` | `$B4` | 2 | 4 |
| Absolute | `LDY $nnnn` | `$AC` | 3 | 4 |
| Absolute,X | `LDY $nnnn,X` | `$BC` | 3 | 4+ |

#### STA - Store Accumulator

`M = A`

Writes the accumulator to memory. No flag is changed.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Zero Page | `STA $nn` | `$85` | 2 | 3 |
| Zero Page,X | `STA $nn,X` | `$95` | 2 | 4 |
| Absolute | `STA $nnnn` | `$8D` | 3 | 4 |
| Absolute,X | `STA $nnnn,X` | `$9D` | 3 | 5 |
| Absolute,Y | `STA $nnnn,Y` | `$99` | 3 | 5 |
| (Indirect,X) | `STA ($nn,X)` | `$81` | 2 | 6 |
| (Indirect),Y | `STA ($nn),Y` | `$91` | 2 | 6 |

#### STX - Store X Register

`M = X`

Writes the X register to memory. No flag is changed.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Zero Page | `STX $nn` | `$86` | 2 | 3 |
| Zero Page,Y | `STX $nn,Y` | `$96` | 2 | 4 |
| Absolute | `STX $nnnn` | `$8E` | 3 | 4 |

#### STY - Store Y Register

`M = Y`

Writes the Y register to memory. No flag is changed.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Zero Page | `STY $nn` | `$84` | 2 | 3 |
| Zero Page,X | `STY $nn,X` | `$94` | 2 | 4 |
| Absolute | `STY $nnnn` | `$8C` | 3 | 4 |

</details>

<details>
<summary><b>Register transfers</b> (TAX, TAY, TXA, TYA, TSX, TXS)</summary>

Copy a value between two registers. All of them set Z and N except TXS.

#### TAX - Transfer Accumulator to X

`X = A`

Copies the accumulator into X.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `TAX` | `$AA` | 1 | 2 |

#### TAY - Transfer Accumulator to Y

`Y = A`

Copies the accumulator into Y.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `TAY` | `$A8` | 1 | 2 |

#### TXA - Transfer X to Accumulator

`A = X`

Copies X into the accumulator.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `TXA` | `$8A` | 1 | 2 |

#### TYA - Transfer Y to Accumulator

`A = Y`

Copies Y into the accumulator.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `TYA` | `$98` | 1 | 2 |

#### TSX - Transfer Stack Pointer to X

`X = S`

Copies the low byte of the stack pointer into X.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `TSX` | `$BA` | 1 | 2 |

#### TXS - Transfer X to Stack Pointer

`S = X`

Copies X into the stack pointer (the stack is always in page `$01`, so only 8 bits are stored). This is the only transfer that does not touch the flags.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `TXS` | `$9A` | 1 | 2 |

</details>

<details>
<summary><b>Stack</b> (PHA, PHP, PLA, PLP)</summary>

Push to / pull from the stack (page `$01`). Pulling a byte into A sets Z and N.

#### PHA - Push Accumulator

`push A`

Pushes the accumulator on the stack.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `PHA` | `$48` | 1 | 3 |

#### PHP - Push Processor Status

`push P`

Pushes the status flags on the stack. The pushed copy always has the break bit (bit 4) and the unused bit (bit 5) set.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `PHP` | `$08` | 1 | 3 |

#### PLA - Pull Accumulator

`A = pull`

Pulls a byte from the stack into the accumulator.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `PLA` | `$68` | 1 | 4 |

#### PLP - Pull Processor Status

`P = pull`

Pulls a byte from the stack into the status flags, replacing all of them.

**Flags:** all (loaded from the stack)

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `PLP` | `$28` | 1 | 4 |

</details>

<details>
<summary><b>Logical</b> (AND, EOR, ORA, BIT)</summary>

Bit operations on the accumulator.

#### AND - Logical AND

`A = A & M`

Bitwise AND of the accumulator with the operand.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `AND #$nn` | `$29` | 2 | 2 |
| Zero Page | `AND $nn` | `$25` | 2 | 3 |
| Zero Page,X | `AND $nn,X` | `$35` | 2 | 4 |
| Absolute | `AND $nnnn` | `$2D` | 3 | 4 |
| Absolute,X | `AND $nnnn,X` | `$3D` | 3 | 4+ |
| Absolute,Y | `AND $nnnn,Y` | `$39` | 3 | 4+ |
| (Indirect,X) | `AND ($nn,X)` | `$21` | 2 | 6 |
| (Indirect),Y | `AND ($nn),Y` | `$31` | 2 | 5+ |

#### EOR - Exclusive OR

`A = A ^ M`

Bitwise exclusive OR of the accumulator with the operand.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `EOR #$nn` | `$49` | 2 | 2 |
| Zero Page | `EOR $nn` | `$45` | 2 | 3 |
| Zero Page,X | `EOR $nn,X` | `$55` | 2 | 4 |
| Absolute | `EOR $nnnn` | `$4D` | 3 | 4 |
| Absolute,X | `EOR $nnnn,X` | `$5D` | 3 | 4+ |
| Absolute,Y | `EOR $nnnn,Y` | `$59` | 3 | 4+ |
| (Indirect,X) | `EOR ($nn,X)` | `$41` | 2 | 6 |
| (Indirect),Y | `EOR ($nn),Y` | `$51` | 2 | 5+ |

#### ORA - Logical Inclusive OR

`A = A | M`

Bitwise inclusive OR of the accumulator with the operand.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `ORA #$nn` | `$09` | 2 | 2 |
| Zero Page | `ORA $nn` | `$05` | 2 | 3 |
| Zero Page,X | `ORA $nn,X` | `$15` | 2 | 4 |
| Absolute | `ORA $nnnn` | `$0D` | 3 | 4 |
| Absolute,X | `ORA $nnnn,X` | `$1D` | 3 | 4+ |
| Absolute,Y | `ORA $nnnn,Y` | `$19` | 3 | 4+ |
| (Indirect,X) | `ORA ($nn,X)` | `$01` | 2 | 6 |
| (Indirect),Y | `ORA ($nn),Y` | `$11` | 2 | 5+ |

#### BIT - Bit Test

`A & M, N = M7, V = M6`

Tests memory against the accumulator without changing either. Z is set when `A & M` is zero, while N and V are copied straight from bits 7 and 6 of the memory byte.

**Flags:** Z V N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Zero Page | `BIT $nn` | `$24` | 2 | 3 |
| Absolute | `BIT $nnnn` | `$2C` | 3 | 4 |

</details>

<details>
<summary><b>Arithmetic</b> (ADC, SBC, CMP, CPX, CPY)</summary>

Addition, subtraction and comparison.

#### ADC - Add with Carry

`A = A + M + C`

Adds the operand and the carry to the accumulator. The carry is set when the sum does not fit in 8 bits, so several bytes can be added by chaining ADCs (use CLC before the first one). V is set when the result is wrong as a signed number, e.g. 64 + 64. With the D flag set, the operands are treated as BCD digits.

**Flags:** C Z V N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `ADC #$nn` | `$69` | 2 | 2 |
| Zero Page | `ADC $nn` | `$65` | 2 | 3 |
| Zero Page,X | `ADC $nn,X` | `$75` | 2 | 4 |
| Absolute | `ADC $nnnn` | `$6D` | 3 | 4 |
| Absolute,X | `ADC $nnnn,X` | `$7D` | 3 | 4+ |
| Absolute,Y | `ADC $nnnn,Y` | `$79` | 3 | 4+ |
| (Indirect,X) | `ADC ($nn,X)` | `$61` | 2 | 6 |
| (Indirect),Y | `ADC ($nn),Y` | `$71` | 2 | 5+ |

#### SBC - Subtract with Carry

`A = A - M - (1 - C)`

Subtracts the operand and the inverted carry (the "borrow") from the accumulator. C ends up **set when no borrow was needed**, so use SEC before the first byte of a subtraction. V works like in ADC. With the D flag set, BCD arithmetic is used.

**Flags:** C Z V N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `SBC #$nn` | `$E9` | 2 | 2 |
| Zero Page | `SBC $nn` | `$E5` | 2 | 3 |
| Zero Page,X | `SBC $nn,X` | `$F5` | 2 | 4 |
| Absolute | `SBC $nnnn` | `$ED` | 3 | 4 |
| Absolute,X | `SBC $nnnn,X` | `$FD` | 3 | 4+ |
| Absolute,Y | `SBC $nnnn,Y` | `$F9` | 3 | 4+ |
| (Indirect,X) | `SBC ($nn,X)` | `$E1` | 2 | 6 |
| (Indirect),Y | `SBC ($nn),Y` | `$F1` | 2 | 5+ |

#### CMP - Compare Accumulator

`A - M`

Compares the accumulator with the operand by subtracting, but only the flags are kept: C is set if `A >= M`, Z if they are equal, N from bit 7 of the difference.

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `CMP #$nn` | `$C9` | 2 | 2 |
| Zero Page | `CMP $nn` | `$C5` | 2 | 3 |
| Zero Page,X | `CMP $nn,X` | `$D5` | 2 | 4 |
| Absolute | `CMP $nnnn` | `$CD` | 3 | 4 |
| Absolute,X | `CMP $nnnn,X` | `$DD` | 3 | 4+ |
| Absolute,Y | `CMP $nnnn,Y` | `$D9` | 3 | 4+ |
| (Indirect,X) | `CMP ($nn,X)` | `$C1` | 2 | 6 |
| (Indirect),Y | `CMP ($nn),Y` | `$D1` | 2 | 5+ |

#### CPX - Compare X Register

`X - M`

Same as CMP, using the X register.

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `CPX #$nn` | `$E0` | 2 | 2 |
| Zero Page | `CPX $nn` | `$E4` | 2 | 3 |
| Absolute | `CPX $nnnn` | `$EC` | 3 | 4 |

#### CPY - Compare Y Register

`Y - M`

Same as CMP, using the Y register.

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Immediate | `CPY #$nn` | `$C0` | 2 | 2 |
| Zero Page | `CPY $nn` | `$C4` | 2 | 3 |
| Absolute | `CPY $nnnn` | `$CC` | 3 | 4 |

</details>

<details>
<summary><b>Increments and decrements</b> (INC, INX, INY, DEC, DEX, DEY)</summary>

Add or subtract one from memory or from X / Y. They set Z and N and leave the carry alone.

#### INC - Increment Memory

`M = M + 1`

Adds one to a byte in memory. Wraps from `$FF` to `$00`. The carry is not touched.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Zero Page | `INC $nn` | `$E6` | 2 | 5 |
| Zero Page,X | `INC $nn,X` | `$F6` | 2 | 6 |
| Absolute | `INC $nnnn` | `$EE` | 3 | 6 |
| Absolute,X | `INC $nnnn,X` | `$FE` | 3 | 7 |

#### INX - Increment X Register

`X = X + 1`

Adds one to X.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `INX` | `$E8` | 1 | 2 |

#### INY - Increment Y Register

`Y = Y + 1`

Adds one to Y.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `INY` | `$C8` | 1 | 2 |

#### DEC - Decrement Memory

`M = M - 1`

Subtracts one from a byte in memory. Wraps from `$00` to `$FF`. The carry is not touched.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Zero Page | `DEC $nn` | `$C6` | 2 | 5 |
| Zero Page,X | `DEC $nn,X` | `$D6` | 2 | 6 |
| Absolute | `DEC $nnnn` | `$CE` | 3 | 6 |
| Absolute,X | `DEC $nnnn,X` | `$DE` | 3 | 7 |

#### DEX - Decrement X Register

`X = X - 1`

Subtracts one from X.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `DEX` | `$CA` | 1 | 2 |

#### DEY - Decrement Y Register

`Y = Y - 1`

Subtracts one from Y.

**Flags:** Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `DEY` | `$88` | 1 | 2 |

</details>

<details>
<summary><b>Shifts and rotates</b> (ASL, LSR, ROL, ROR)</summary>

Move the bits of the accumulator or of a memory byte one place. The bit that falls out ends up in the carry.

#### ASL - Arithmetic Shift Left

`A or M = M * 2`

Shifts every bit one place to the left. Bit 7 moves into the carry and bit 0 becomes 0. It doubles the value (unsigned).

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Accumulator | `ASL A` | `$0A` | 1 | 2 |
| Zero Page | `ASL $nn` | `$06` | 2 | 5 |
| Zero Page,X | `ASL $nn,X` | `$16` | 2 | 6 |
| Absolute | `ASL $nnnn` | `$0E` | 3 | 6 |
| Absolute,X | `ASL $nnnn,X` | `$1E` | 3 | 7 |

#### LSR - Logical Shift Right

`A or M = M / 2`

Shifts every bit one place to the right. Bit 0 moves into the carry and bit 7 becomes 0. It halves the value (unsigned).

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Accumulator | `LSR A` | `$4A` | 1 | 2 |
| Zero Page | `LSR $nn` | `$46` | 2 | 5 |
| Zero Page,X | `LSR $nn,X` | `$56` | 2 | 6 |
| Absolute | `LSR $nnnn` | `$4E` | 3 | 6 |
| Absolute,X | `LSR $nnnn,X` | `$5E` | 3 | 7 |

#### ROL - Rotate Left

`A or M = (M << 1) | C`

Rotates the bits one place to the left **through the carry**: the old carry enters at bit 0 and bit 7 becomes the new carry.

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Accumulator | `ROL A` | `$2A` | 1 | 2 |
| Zero Page | `ROL $nn` | `$26` | 2 | 5 |
| Zero Page,X | `ROL $nn,X` | `$36` | 2 | 6 |
| Absolute | `ROL $nnnn` | `$2E` | 3 | 6 |
| Absolute,X | `ROL $nnnn,X` | `$3E` | 3 | 7 |

#### ROR - Rotate Right

`A or M = (M >> 1) | (C << 7)`

Rotates the bits one place to the right **through the carry**: the old carry enters at bit 7 and bit 0 becomes the new carry.

**Flags:** C Z N

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Accumulator | `ROR A` | `$6A` | 1 | 2 |
| Zero Page | `ROR $nn` | `$66` | 2 | 5 |
| Zero Page,X | `ROR $nn,X` | `$76` | 2 | 6 |
| Absolute | `ROR $nnnn` | `$6E` | 3 | 6 |
| Absolute,X | `ROR $nnnn,X` | `$7E` | 3 | 7 |

</details>

<details>
<summary><b>Jumps and calls</b> (JMP, JSR, RTS)</summary>

Change the program counter.

#### JMP - Jump

`PC = address`

Continues execution at the given address. The indirect form reads the target from the two bytes at the given address. On the original chip, if that pointer sits at the end of a page (`$xxFF`), the high byte is read from `$xx00` instead of the next page. This emulator reproduces that bug.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Absolute | `JMP $nnnn` | `$4C` | 3 | 3 |
| Indirect | `JMP ($nnnn)` | `$6C` | 3 | 5 |

#### JSR - Jump to Subroutine

`push PC-1; PC = address`

Calls a subroutine: pushes the address of the **last byte of the JSR instruction** (the return address minus one), high byte first, then jumps.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Absolute | `JSR $nnnn` | `$20` | 3 | 6 |

#### RTS - Return from Subroutine

`PC = pull + 1`

Returns from a subroutine: pulls the return address from the stack and adds one to it, which is the instruction after the JSR.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `RTS` | `$60` | 1 | 6 |

</details>

<details>
<summary><b>Branches</b> (BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS)</summary>

Relative jumps: the operand is a **signed byte** (-128 to +127) added to the address of the *next* instruction, when the condition is true. Cycles are listed as `not taken / taken / taken to another page`.

#### BCC - Branch if Carry Clear

`branch if C = 0`

Branches if the carry flag is clear.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BCC label` | `$90` | 2 | 2 / 3 / 4 |

#### BCS - Branch if Carry Set

`branch if C = 1`

Branches if the carry flag is set.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BCS label` | `$B0` | 2 | 2 / 3 / 4 |

#### BEQ - Branch if Equal

`branch if Z = 1`

Branches if the zero flag is set (the last result was zero / the compared values were equal).

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BEQ label` | `$F0` | 2 | 2 / 3 / 4 |

#### BMI - Branch if Minus

`branch if N = 1`

Branches if the negative flag is set.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BMI label` | `$30` | 2 | 2 / 3 / 4 |

#### BNE - Branch if Not Equal

`branch if Z = 0`

Branches if the zero flag is clear.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BNE label` | `$D0` | 2 | 2 / 3 / 4 |

#### BPL - Branch if Positive

`branch if N = 0`

Branches if the negative flag is clear.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BPL label` | `$10` | 2 | 2 / 3 / 4 |

#### BVC - Branch if Overflow Clear

`branch if V = 0`

Branches if the overflow flag is clear.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BVC label` | `$50` | 2 | 2 / 3 / 4 |

#### BVS - Branch if Overflow Set

`branch if V = 1`

Branches if the overflow flag is set.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Relative | `BVS label` | `$70` | 2 | 2 / 3 / 4 |

</details>

<details>
<summary><b>Status flag changes</b> (CLC, CLD, CLI, CLV, SEC, SED, SEI)</summary>

Set or clear a single flag. Two cycles each.

#### CLC - Clear Carry Flag

`C = 0`

Clears the carry flag.

**Flags:** C

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `CLC` | `$18` | 1 | 2 |

#### CLD - Clear Decimal Mode

`D = 0`

Clears the decimal flag: ADC and SBC go back to binary arithmetic. The chip does not clear this flag by itself at power on or on interrupts, so programs should run CLD first.

**Flags:** D

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `CLD` | `$D8` | 1 | 2 |

#### CLI - Clear Interrupt Disable

`I = 0`

Clears the interrupt disable flag, allowing IRQs to be serviced.

**Flags:** I

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `CLI` | `$58` | 1 | 2 |

#### CLV - Clear Overflow Flag

`V = 0`

Clears the overflow flag. There is no instruction to set it.

**Flags:** V

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `CLV` | `$B8` | 1 | 2 |

#### SEC - Set Carry Flag

`C = 1`

Sets the carry flag.

**Flags:** C

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `SEC` | `$38` | 1 | 2 |

#### SED - Set Decimal Flag

`D = 1`

Sets the decimal flag: ADC and SBC use BCD arithmetic.

**Flags:** D

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `SED` | `$F8` | 1 | 2 |

#### SEI - Set Interrupt Disable

`I = 1`

Sets the interrupt disable flag, so IRQs are ignored.

**Flags:** I

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `SEI` | `$78` | 1 | 2 |

</details>

<details>
<summary><b>System</b> (BRK, NOP, RTI)</summary>

Interrupt and miscellaneous instructions.

#### BRK - Force Interrupt

`software interrupt`

Pushes the return address (**BRK address + 2**, the byte after BRK is a padding byte that is skipped), then the status flags with the break bit set, sets the I flag and loads the PC from the IRQ/BRK vector at `$FFFE/$FFFF`. In this emulator `exec()` returns right after a BRK.

**Flags:** B, and I (set by the interrupt sequence)

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `BRK` | `$00` | 1 | 7 |

#### NOP - No Operation

Does nothing for two cycles.

**Flags:** none

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `NOP` | `$EA` | 1 | 2 |

#### RTI - Return from Interrupt

`P = pull; PC = pull`

Returns from an interrupt: pulls the status flags first, then the program counter. Unlike RTS, nothing is added to the address.

**Flags:** all (loaded from the stack)

| Mode | Syntax | Opcode | Bytes | Cycles |
| ---- | ------ | ------ | ----- | ------ |
| Implied | `RTI` | `$40` | 1 | 6 |

</details>

### Original sources

The opcode, cycle and flag data above was verified against the Obelisk 6502 guide (archived copy). Go there for the original text:

- CPU description: https://web.archive.org/web/20190130171422/http://www.obelisk.me.uk/6502/
- Registers: https://web.archive.org/web/20190203164248/http://obelisk.me.uk/6502/registers.html
- Addressing modes: https://web.archive.org/web/20190130160859/http://obelisk.me.uk/6502/addressing.html
- Instruction set: https://web.archive.org/web/20190206131148/http://www.obelisk.me.uk/6502/instructions.html
- Instruction reference (opcodes, flags, cycles): https://web.archive.org/web/20190213222926/http://www.obelisk.me.uk/6502/reference.html
