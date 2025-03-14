# 6502 CPU EMULATION 
> [!NOTE]  
> This project is under development!

Simple C++ emulation of MOS Technology 6502.\
The 6502 is a 8 bit (little endian) CPU, with a capability of addressing maximum of 64KB of memory, using a 16bits address bus.

## Objective
In this project, the main objective is to create a simple emulator for 6502 processor, with the capabilty to read assembly and to perform the correct instructions operation.\
The project is still in "primitive" phase.

## Available assembly commands:

- LDA (Load to Accumulator): save value Accumulator.
  - Immediate, Zero Page and Zero Page X, mode available.\*
- LDX (Load to Register X ): save value Register X.
  - Immediate and Zero Page, mode available.\*
- LDY (Load to Register Y): save value Register Y.
  - Immediate and Zero Page, mode available.\*
- JSR (Jump to subroutine): jump to given memory address.
- STA (Store to accumulator): store to accumulator register, the value in given memory position.

More information here: https://web.archive.org/web/20190130160859/http://obelisk.me.uk/6502/addressing.html#ABS

#### Ways to load programs:
###### Using the function `loadTestProgram`
Add to ` void loadTestProgram(Memory &memory)` function in `main.cpp`

LDA, example:

```c++
memory[0x4242] = opcodes::LDA;
memory[0x4243] = 0x84;
```

JSR, example:

```c++
memory[0xfffC] = opcodes::JSR;
memory[0xfffD] = 0x42;
memory[0xfffe] = 0x42;
memory[0x4242] = opcodes::LDA;
memory[0x4243] = 0x84;
```

###### Using the assembler
By writing simple assembly code, for example: 
```assembly
LDA #50
LDX #10
```
The assembly code is written to ``assembly_code.txt``

## Online Documentation:

- CPU description: https://web.archive.org/web/20190130171422/http://www.obelisk.me.uk/6502/

Instructions completed: 16/145

I: Implemented \
N: Not implemented \
-: Don't exist

| Instruction | Name                        | Immediate Mode | Zero Page Mode | Zero Page X Mode | Zero Page Y Mode | Absolute Mode | Absolute X Mode | Absolute Y Mode | Indirect X Mode | Indirect Y Mode | Accumulator Mode | Relative Mode | Implied Mode | Indirect Mode |
| ----------- | --------------------------- | -------------- | -------------- | ---------------- | ---------------- | ------------- | --------------- | --------------- | --------------- | --------------- | ---------------- | ------------- | ------------ | ------------- |
| ADC         | Add with Carry              | N              | N              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| AND         | Logical AND                 | N              | N              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| ASL         | Arithmetic Shift Left       | -              | N              | N                | -                | N             | N               | -               | -               | -               | N                | -             | -            | -             |
| BCC         | Branch if Carry Clear       | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BCS         | Branch if Carry Set         | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BEQ         | Branch if Equal             | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BIT         | Bit Test                    | -              | N              | -                | -                | N             | -               | -               | -               | -               | -                | -             | -            | -             |
| BMI         | Branch if Minus             | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BNE         | Branch if Not Equal         | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BPL         | Branch if Positive          | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BRK         | Force Interrupt             | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | I            | -             |
| BVC         | Branch if Overflow Clear    | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| BVS         | Branch if Overflow Set      | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | N             | -            | -             |
| CLC         | Clear Carry Flag            | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| CLD         | Clear Decimal Mode          | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| CLI         | Clear Interrupt Disable     | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| CLV         | Clear Overflow Flag         | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| CMP         | Compare                     | N              | N              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| CPX         | Compare X Register          | N              | N              | -                | -                | N             | -               | -               | -               | -               | -                | -             | -            | -             |
| CPY         | Compare Y Register          | N              | N              | -                | -                | N             | -               | -               | -               | -               | -                | -             | -            | -             |
| DEC         | Decrement Memory            | -              | N              | N                | -                | N             | N               | -               | -               | -               | -                | -             | -            | -             |
| DEX         | Decrement X Register        | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| DEY         | Decrement Y Register        | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| EOR         | Exclusive OR                | N              | N              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| INC         | Increment Memory            | -              | N              | N                | -                | N             | N               | -               | -               | -               | -                | -             | -            | -             |
| INX         | Increment X Register        | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| INY         | Increment Y Register        | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| JMP         | Jump                        | -              | -              | -                | -                | N             | -               | -               | -               | -               | -                | -             | -            | N             |
| JSR         | Jump to Subroutine          | -              | -              | -                | -                | I             | -               | -               | -               | -               | -                | -             | -            | -             |
| LDA         | Load Accumulator            | I              | I              | I                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| LDX         | Load X Register             | I              | I              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| LDY         | Load y Register             | I              | I              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| LSR         | Logical Shift Right         | -              | N              | N                | -                | N             | N               | -               | -               | -               | N                | -             | -            | -             |
| NOP         | No Operation                | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| ORA         | Logical Inclusive OR        | N              | N              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| PHA         | Push Accumulator            | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| PHP         | Push Processor Status       | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| PLA         | Pull Accumulator            | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| PLP         | Pull Processor Status       | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| ROL         | Rotate Left                 | -              | N              | N                | -                | N             | N               | -               | -               | -               | N                | -             | -            | -             |
| ROR         | Rotate Right                | -              | N              | N                | -                | N             | N               | -               | -               | -               | N                | -             | -            | -             |
| RTI         | Return from Interrupt       | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| RTS         | Return from Subroutine      | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| SBC         | Subtract with Carry         | N              | N              | N                | -                | N             | N               | N               | N               | N               | -                | -             | -            | -             |
| SEC         | Set Carry Flag              | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| SED         | SED - Set Decimal Flag      | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| SEI         | Set Interrupt Disable       | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| STA         | Store Accumulator           | -              | I              | I                | -                | I             | I               | I               | N               | N               | -                | -             | -            | -             |
| STX         | Store X Register            | -              | I              | -                | N                | N             | -               | -               | -               | -               | -                | -             | -            | -             |
| STY         | Store Y Register            | -              | I              | N                | -                | N             | -               | -               | -               | -               | -                | -             | -            | -             |
| TAX         | Transfer Accumulator to X   | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| TAY         | Transfer Accumulator to Y   | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| TSX         | Transfer Stack Pointer to X | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| TXA         | Transfer X to Accumulator   | -              | -                 | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| TXS         | Transfer X to Stack Pointer | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |
| TYA         | Transfer X to Stack Pointer | -              | -              | -                | -                | -             | -               | -               | -               | -               | -                | -             | N            | -             |

## Docker
#### Build and enter container command:
```
docker-compose build emulator
```
```
docker-compose run emulator
```
It this moment, it should open the bash terminal inside the container.
Every change on the code, will be directly avaliable inside the container.
Meaning, it is possivel to execute the scripts that are modified, not needing to rebuild the docker image


#### To execute code after enter container:
Example:
```
root@1c1d29670293:/app# ls
```

#### To destroy image
```
docker-compose stop emulator
```

## Json to debug processor:
**NOTE: this project was build in ubuntu enviroment!**
Create a file task.json on .vscode folder
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
