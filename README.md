# 6502 CPU EMULATION
Simple C++ emulation of MOS Technology 6502.\
The 6502 is a 8 bit (little endian) CPU, with a capability of addressing maximum of 64KB of memory, using a 16bits address bus.

<!--## Contributing
After making the changes, create a branch, make a pull request to branch `pre-main`. See the code, and merge it.
After merge it, make a PR from `pre-main` to `main`, and see the github actions

-->
## Avaliable assembly commands:
- LDA (Load to Accumulator): save value Accumulator.
  - Immediate, Zero Page and Zero Page X, mode avaliable.*
- LDX (Load to Register X ): save value Register X.
  - Immediate and Zero Page, mode avaliable.*
- LDY (Load to Register Y): save value Register Y.
  - Immediate and Zero Page, mode avaliable.*
- JSR (Jump to subroutine): jump to given memory address.

*More information here: https://web.archive.org/web/20190130160859/http://obelisk.me.uk/6502/addressing.html#ABS

#### Current way to load programs:
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

## Online Documentation:
- CPU description: https://web.archive.org/web/20190130171422/http://www.obelisk.me.uk/6502/

## Instructions list:

- [ ] ADC
- [ ] AND
- [ ] ASL
- [ ] BCC
- [ ] BCS
- [ ] BEQ
- [ ] BIT
- [ ] BMI
- [ ] BNE
- [ ] BPL
- [ ] BRK
- [ ] BVC
- [ ] BVS
- [ ] CLC
- [ ] CLD
- [ ] CLI
- [ ] CLV
- [ ] CMP
- [ ] CPX
- [ ] CPY
- [ ] DEC
- [ ] DEX
- [ ] DEY
- [ ] EOR
- [ ] INC
- [ ] INX
- [ ] INY
- [ ] JMP
- [X] JSR
- [ ] LDA
- [ ] LDX
- [ ] LDY
- [ ] LSR
- [ ] NOP
- [ ] ORA
- [ ] PHA
- [ ] PHP
- [ ] PLA
- [ ] PLP
- [ ] ROL
- [ ] ROR
- [ ] RTI
- [ ] RTS
- [ ] SBC
- [ ] SEC
- [ ] SED
- [ ] SEI
- [ ] STA
- [ ] STX
- [ ] STY
- [ ] TAX
- [ ] TAY
- [ ] TSX
- [ ] TXA
- [ ] TXS
- [ ] TYA
- [ ] ADC
- [ ] AND
- [ ] ASL
- [ ] BCC
- [ ] BCS
- [ ] BEQ
- [ ] BIT
- [ ] BMI
- [ ] BNE
- [ ] BPL
- [ ] BRK
- [ ] BVC
- [ ] BVS
- [ ] CLC
- [ ] CLD
- [ ] CLI
- [ ] CLV
- [ ] CMP
- [ ] CPX
- [ ] CPY
- [ ] DEC
- [ ] DEX
- [ ] DEY
- [ ] EOR
- [ ] INC
- [ ] INX
- [ ] INY
- [ ] JMP
- [ ] JSR
- [X] LDA
- [X] LDX
- [X] LDY
- [ ] LSR
- [ ] NOP
- [ ] ORA
- [ ] PHA
- [ ] PHP
- [ ] PLA
- [ ] PLP
- [ ] ROL
- [ ] ROR
- [ ] RTI
- [ ] RTS
- [ ] SBC
- [ ] SEC
- [ ] SED
- [ ] SEI
- [ ] STA
- [ ] STX
- [ ] STY
- [ ] TAX
- [ ] TAY
- [ ] TSX
- [ ] TXA
- [ ] TXS
- [ ] TYA

