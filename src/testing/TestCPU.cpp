#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "TestCPU.h"
#include "../opcodes/opcodes.h"
#include "../data_types/data_types.h"
#include "../assembler/Assembler.h"

namespace
{
    constexpr s32 BRK_CYCLES = 7; // exec() stops after a BRK and counts its cycles too

    // Addressing modes the generic mode tests know how to set up
    enum class AddrMode { IMM, ZP, ZPX, ZPY, ABS, ABSX, ABSY, INDX, INDY };

    // One opcode to run in a given addressing mode, and the clock cycles it must take
    struct ModeCase
    {
        AddrMode mode;
        Byte opcode;
        s32 cycles;
    };

    const char *modeName(AddrMode mode)
    {
        switch (mode)
        {
            case AddrMode::IMM:  return "immediate";
            case AddrMode::ZP:   return "zero page";
            case AddrMode::ZPX:  return "zero page,X";
            case AddrMode::ZPY:  return "zero page,Y";
            case AddrMode::ABS:  return "absolute";
            case AddrMode::ABSX: return "absolute,X";
            case AddrMode::ABSY: return "absolute,Y";
            case AddrMode::INDX: return "(indirect,X)";
            case AddrMode::INDY: return "(indirect),Y";
        }
        return "?";
    }

    /**
     * Collects the failures of one test and prints what went wrong.
     */
    class Check
    {
    public:
        void that(bool condition, const std::string &message)
        {
            if (condition)
            {
                return;
            }
            if (!failed)
            {
                std::cout << "\n";
            }
            std::cout << "    " << message << "\n";
            failed = true;
        }

        /**
         * Checks the C, Z, V and N flags. The pattern has 4 characters, one for each flag in
         * that order: upper case = must be set, lower case = must be clear, '-' = don't care.
         * Example: "CzvN"
         */
        void flags(CPU &cpu, const char *pattern, const std::string &context)
        {
            static const int ids[4] = {CARRY_FLAG, ZERO_FLAG, OVERFLOW_FLAG, NEGATIVE_FLAG};
            static const char *names[4] = {"carry", "zero", "overflow", "negative"};
            for (int i = 0; i < 4; i++)
            {
                if (pattern[i] == '-')
                {
                    continue;
                }
                bool expected = pattern[i] >= 'A' && pattern[i] <= 'Z';
                that(cpu.getProcessorFlag(ids[i]) == expected,
                     context + ": " + names[i] + " flag should be " + (expected ? "set" : "clear"));
            }
        }

        // The zero and negative flags must describe `result`
        void zeroNegative(CPU &cpu, Byte result, const std::string &context)
        {
            that(cpu.getProcessorFlag(ZERO_FLAG) == (result == 0), context + ": zero flag does not match the result");
            that(cpu.getProcessorFlag(NEGATIVE_FLAG) == ((result & 0x80) != 0), context + ": negative flag does not match the result");
        }

        bool failed = false;
    };

    using Setup = std::function<void(CPU &)>;
    using Verify = std::function<void(Check &, CPU &, Memory &, Word, const std::string &)>;

    // Shorthand for the lambdas passed to run_modes(): `target` is the address the operand was read from / written to
#define VERIFY [&](Check &check, CPU &cpu, Memory &memory, Word target, const std::string &where)

    void begin(const char *name, Memory &memory, CPU &cpu)
    {
        std::cout << name << " TEST ";
        cpu.reset(memory);
    }

    bool finish(const Check &check, Memory &memory, CPU &cpu)
    {
        if (check.failed)
        {
            std::cout << RED << "ERROR" << RESET << std::endl;
            return 1;
        }
        std::cout << GREEN << "SUCCESS" << RESET << std::endl;
        cpu.reset(memory);
        return 0;
    }

    // Writes the program at `origin` and runs it. Returns the clock cycles used, BRK not included.
    s32 run(Memory &memory, CPU &cpu, const std::vector<Byte> &program, Word origin = 0x0200)
    {
        Word address = origin;
        for (Byte byte : program)
        {
            memory[address++] = byte;
        }
        cpu.setProgramCounter(origin);
        return cpu.exec(memory) - BRK_CYCLES;
    }

    /**
     * Writes "<opcode> <operand>" for the addressing mode at 0x0200 and puts `value` where the
     * operand points to, assuming X = 4 and Y = 5.
     * @returns Word - the address the instruction reads from / writes to
     */
    Word loadOperand(Memory &memory, AddrMode mode, Byte opcode, Byte value)
    {
        memory[0x0200] = opcode;
        switch (mode)
        {
            case AddrMode::IMM:
                memory[0x0201] = value;
                return 0x0201;
            case AddrMode::ZP:
                memory[0x0201] = 0x40;
                memory[0x40] = value;
                return 0x40;
            case AddrMode::ZPX:
                memory[0x0201] = 0x40;
                memory[0x44] = value;
                return 0x44;
            case AddrMode::ZPY:
                memory[0x0201] = 0x40;
                memory[0x45] = value;
                return 0x45;
            case AddrMode::ABS:
            case AddrMode::ABSX:
            case AddrMode::ABSY:
            {
                memory[0x0201] = 0x00;
                memory[0x0202] = 0x30;
                Word address = mode == AddrMode::ABS ? 0x3000 : (mode == AddrMode::ABSX ? 0x3004 : 0x3005);
                memory[address] = value;
                return address;
            }
            case AddrMode::INDX:
                memory[0x0201] = 0x20;
                memory[0x24] = 0x00; // 0x20 + X
                memory[0x25] = 0x31;
                memory[0x3100] = value;
                return 0x3100;
            case AddrMode::INDY:
                memory[0x0201] = 0x20;
                memory[0x20] = 0x00;
                memory[0x21] = 0x32;
                memory[0x3205] = value; // 0x3200 + Y
                return 0x3205;
        }
        return 0;
    }

    /**
     * Runs each case on a fresh CPU (X = 4, Y = 5), checks its clock cycles and calls verify.
     */
    void run_modes(Check &check, Memory &memory, CPU &cpu, const std::vector<ModeCase> &cases, Byte operand,
                   const Setup &setup, const Verify &verify)
    {
        for (const ModeCase &testCase : cases)
        {
            const std::string where = modeName(testCase.mode);
            cpu.reset(memory);
            cpu.setRX(0x04);
            cpu.setRY(0x05);
            setup(cpu);
            Word target = loadOperand(memory, testCase.mode, testCase.opcode, operand);
            s32 cycles = cpu.exec(memory) - BRK_CYCLES;
            check.that(cycles == testCase.cycles,
                       where + ": expected " + std::to_string(testCase.cycles) + " clock cycles, took " + std::to_string(cycles));
            verify(check, cpu, memory, target, where);
        }
    }

    // The 8 opcodes (and cycles) of the "classic" instructions: ADC, AND, CMP, EOR, LDA, ORA, SBC
    std::vector<ModeCase> aluModes(const Byte (&ops)[8])
    {
        return {
            {AddrMode::IMM, ops[0], 2},  {AddrMode::ZP, ops[1], 3},   {AddrMode::ZPX, ops[2], 4}, {AddrMode::ABS, ops[3], 4},
            {AddrMode::ABSX, ops[4], 4}, {AddrMode::ABSY, ops[5], 4}, {AddrMode::INDX, ops[6], 6}, {AddrMode::INDY, ops[7], 5}
        };
    }

#define ALU_OPS(name)                                                                                          \
    {opcodes::name, opcodes::name##_ZERO_PAGE, opcodes::name##_ZERO_PAGE_X, opcodes::name##_ABSOLUTE,          \
     opcodes::name##_ABSOLUTE_X, opcodes::name##_ABSOLUTE_Y, opcodes::name##_INDIRECT_X, opcodes::name##_INDIRECT_Y}

    const Byte ADC_OPS[8] = ALU_OPS(ADC);
    const Byte SBC_OPS[8] = ALU_OPS(SBC);
    const Byte AND_OPS[8] = ALU_OPS(AND);
    const Byte ORA_OPS[8] = ALU_OPS(ORA);
    const Byte EOR_OPS[8] = ALU_OPS(EOR);
    const Byte CMP_OPS[8] = ALU_OPS(CMP);
    const Byte LDA_OPS[8] = ALU_OPS(LDA);

    // ------------------------------------------------------------------ shared test bodies

    /**
     * AND / ORA / EOR: all the addressing modes plus a few values to check the flags.
     */
    bool logical_test(const char *name, Memory &memory, CPU &cpu, const Byte (&ops)[8], Byte (*reference)(Byte, Byte))
    {
        begin(name, memory, cpu);
        Check check;

        run_modes(check, memory, cpu, aluModes(ops), 0x7D, [](CPU &c) { c.setAcc(0xCA); },
                  VERIFY { check.that(cpu.getAcc() == reference(0xCA, 0x7D), where + ": wrong accumulator"); });

        const Byte values[][2] = {{0x0F, 0xF0}, {0xFF, 0x80}, {0xAA, 0x55}, {0xFF, 0xFF}, {0x00, 0x00}, {0x12, 0x34}};
        for (const auto &pair : values)
        {
            cpu.reset(memory);
            run(memory, cpu, {opcodes::LDA, pair[0], ops[0], pair[1]});
            Byte expected = reference(pair[0], pair[1]);
            check.that(cpu.getAcc() == expected, "wrong result for " + std::to_string(pair[0]) + " and " + std::to_string(pair[1]));
            check.zeroNegative(cpu, expected, "flags");
        }
        return finish(check, memory, cpu);
    }

    /**
     * CMP / CPX / CPY: loads the register, compares and checks the flags.
     */
    bool compare_test(const char *name, Memory &memory, CPU &cpu, const std::vector<ModeCase> &cases, Byte loadOpcode,
                      Byte compareImmediate, const Setup &setRegister)
    {
        begin(name, memory, cpu);
        Check check;

        run_modes(check, memory, cpu, cases, 0x40, setRegister,
                  VERIFY { check.flags(cpu, "CZ-n", where); });

        struct Pair { Byte reg; Byte value; const char *flags; };
        const Pair pairs[] = {
            {0x40, 0x40, "CZ-n"}, // equal
            {0x50, 0x40, "Cz-n"}, // greater
            {0x40, 0x50, "cz-N"}, // smaller, the result is negative
            {0x80, 0x01, "Cz-n"},
            {0x00, 0x80, "cz-N"},
            {0xFF, 0x00, "Cz-N"}
        };
        for (const Pair &pair : pairs)
        {
            cpu.reset(memory);
            run(memory, cpu, {loadOpcode, pair.reg, compareImmediate, pair.value});
            check.flags(cpu, pair.flags, std::string("compare ") + std::to_string(pair.reg) + " with " + std::to_string(pair.value));
        }
        return finish(check, memory, cpu);
    }

    /**
     * ASL / LSR / ROL / ROR / INC / DEC with one input value: the memory modes, and the
     * accumulator mode when accumulatorOpcode is not 0 (BRK is not a valid accumulator opcode).
     */
    void rmw_case(Check &check, Memory &memory, CPU &cpu, Byte accumulatorOpcode, const std::vector<ModeCase> &cases,
                  Byte input, bool carryIn, Byte expected, const char *flags)
    {
        run_modes(check, memory, cpu, cases, input,
                  [carryIn](CPU &c) { c.setProcessorFlags(CARRY_FLAG, carryIn); },
                  VERIFY {
                      check.that(memory[target] == expected, where + ": wrong value written back");
                      check.flags(cpu, flags, where);
                  });

        if (accumulatorOpcode != 0)
        {
            cpu.reset(memory);
            cpu.setAcc(input);
            cpu.setProcessorFlags(CARRY_FLAG, carryIn);
            s32 cycles = run(memory, cpu, {accumulatorOpcode});
            check.that(cpu.getAcc() == expected, "accumulator mode: wrong result");
            check.that(cycles == 2, "accumulator mode: should take 2 clock cycles");
            check.flags(cpu, flags, "accumulator mode");
        }
    }

    // ------------------------------------------------------------------ misc helpers

    Byte reference_and(Byte a, Byte b) { return a & b; }
    Byte reference_ora(Byte a, Byte b) { return a | b; }
    Byte reference_eor(Byte a, Byte b) { return a ^ b; }

    // ------------------------------------------------------------------ new tests

    bool LOAD_ALL_MODES_test(Memory &memory, CPU &cpu)
    {
        begin("LDA/LDX/LDY ALL MODES", memory, cpu);
        Check check;

        run_modes(check, memory, cpu, aluModes(LDA_OPS), 0x42, [](CPU &) {},
                  VERIFY { check.that(cpu.getAcc() == 0x42, where + ": LDA wrong accumulator"); });

        run_modes(check, memory, cpu,
                  {{AddrMode::IMM, opcodes::LDX, 2}, {AddrMode::ZP, opcodes::LDX_ZERO_PAGE, 3},
                   {AddrMode::ZPY, opcodes::LDX_ZERO_PAGE_Y, 4}, {AddrMode::ABS, opcodes::LDX_ABSOLUTE, 4},
                   {AddrMode::ABSY, opcodes::LDX_ABSOLUTE_Y, 4}},
                  0x42, [](CPU &) {},
                  VERIFY { check.that(cpu.getRX() == 0x42, where + ": LDX wrong register"); });

        run_modes(check, memory, cpu,
                  {{AddrMode::IMM, opcodes::LDY, 2}, {AddrMode::ZP, opcodes::LDY_ZERO_PAGE, 3},
                   {AddrMode::ZPX, opcodes::LDY_ZERO_PAGE_X, 4}, {AddrMode::ABS, opcodes::LDY_ABSOLUTE, 4},
                   {AddrMode::ABSX, opcodes::LDY_ABSOLUTE_X, 4}},
                  0x42, [](CPU &) {},
                  VERIFY { check.that(cpu.getRY() == 0x42, where + ": LDY wrong register"); });

        return finish(check, memory, cpu);
    }

    bool LOAD_FLAGS_test(Memory &memory, CPU &cpu)
    {
        begin("LOAD FLAGS", memory, cpu);
        Check check;

        struct Load { const char *name; Byte opcode; std::function<Byte(CPU &)> read; };
        const Load loads[] = {
            {"LDA", opcodes::LDA, [](CPU &c) { return c.getAcc(); }},
            {"LDX", opcodes::LDX, [](CPU &c) { return c.getRX(); }},
            {"LDY", opcodes::LDY, [](CPU &c) { return c.getRY(); }}
        };
        // The second load must also clear the flags the first one set
        const Byte sequences[][2] = {{0x00, 0x01}, {0x80, 0x01}, {0x01, 0x00}, {0x01, 0x80}, {0x00, 0x80}};

        for (const Load &load : loads)
        {
            for (const auto &sequence : sequences)
            {
                cpu.reset(memory);
                run(memory, cpu, {load.opcode, sequence[0], load.opcode, sequence[1]});
                check.that(load.read(cpu) == sequence[1], std::string(load.name) + ": wrong value");
                check.zeroNegative(cpu, sequence[1], load.name);
            }
        }
        return finish(check, memory, cpu);
    }

    bool STORE_ALL_MODES_test(Memory &memory, CPU &cpu)
    {
        begin("STA/STX/STY ALL MODES", memory, cpu);
        Check check;
        const Setup setRegisters = [](CPU &c) { c.setAcc(0x7B); c.setRX(0x04); c.setRY(0x05); };

        // The byte at the target starts as 0xEE, so we know it was overwritten
        run_modes(check, memory, cpu,
                  {{AddrMode::ZP, opcodes::STA_ZERO_PAGE, 3}, {AddrMode::ZPX, opcodes::STA_ZERO_PAGE_X, 4},
                   {AddrMode::ABS, opcodes::STA_ABSOLUTE, 4}, {AddrMode::ABSX, opcodes::STA_ABSOLUTE_X, 5},
                   {AddrMode::ABSY, opcodes::STA_ABSOLUTE_Y, 5}, {AddrMode::INDX, opcodes::STA_INDIRECT_X, 6},
                   {AddrMode::INDY, opcodes::STA_INDIRECT_Y, 6}},
                  0xEE, setRegisters,
                  VERIFY { check.that(memory[target] == 0x7B, where + ": STA did not store the accumulator"); });

        run_modes(check, memory, cpu,
                  {{AddrMode::ZP, opcodes::STX_ZERO_PAGE, 3}, {AddrMode::ZPY, opcodes::STX_ZERO_PAGE_Y, 4},
                   {AddrMode::ABS, opcodes::STX_ABSOLUTE, 4}},
                  0xEE, [](CPU &c) { c.setRX(0x04); c.setRY(0x05); },
                  VERIFY { check.that(memory[target] == 0x04, where + ": STX did not store X"); });

        run_modes(check, memory, cpu,
                  {{AddrMode::ZP, opcodes::STY_ZERO_PAGE, 3}, {AddrMode::ZPX, opcodes::STY_ZERO_PAGE_X, 4},
                   {AddrMode::ABS, opcodes::STY_ABSOLUTE, 4}},
                  0xEE, [](CPU &c) { c.setRX(0x04); c.setRY(0x05); },
                  VERIFY { check.that(memory[target] == 0x05, where + ": STY did not store Y"); });

        // Stores must not change any flag
        cpu.reset(memory);
        run(memory, cpu, {opcodes::LDA, 0x00, opcodes::STA_ZERO_PAGE, 0x10, opcodes::STX_ZERO_PAGE, 0x11});
        check.flags(cpu, "-Z-n", "stores keep the flags");
        return finish(check, memory, cpu);
    }

    bool ADDRESSING_EDGE_CASES_test(Memory &memory, CPU &cpu)
    {
        begin("ADDRESSING EDGE CASES", memory, cpu);
        Check check;
        s32 cycles;

        // Zero page indexing wraps around inside the zero page
        cpu.reset(memory);
        cpu.setRX(0x04);
        memory[0x03] = 0x5A;
        run(memory, cpu, {opcodes::LDA_ZERO_PAGE_X, 0xFF});
        check.that(cpu.getAcc() == 0x5A, "LDA $FF,X must wrap to $03");

        cpu.reset(memory);
        cpu.setRX(0x04);
        cpu.setAcc(0x6B);
        run(memory, cpu, {opcodes::STA_ZERO_PAGE_X, 0xFF});
        check.that(memory[0x03] == 0x6B, "STA $FF,X must wrap to $03");

        cpu.reset(memory);
        cpu.setRY(0x05);
        memory[0x04] = 0x5C;
        run(memory, cpu, {opcodes::LDX_ZERO_PAGE_Y, 0xFF});
        check.that(cpu.getRX() == 0x5C, "LDX $FF,Y must wrap to $04");

        // The pointers of the indirect modes wrap inside the zero page too
        cpu.reset(memory);
        cpu.setRX(0x04);
        memory[0xFF] = 0x00; // pointer = 0xFB + X = 0xFF
        memory[0x00] = 0x31; // high byte comes from $00, not from $0100
        memory[0x3100] = 0x7E;
        run(memory, cpu, {opcodes::LDA_INDIRECT_X, 0xFB});
        check.that(cpu.getAcc() == 0x7E, "LDA ($FB,X) pointer must wrap in the zero page");

        cpu.reset(memory);
        cpu.setRY(0x05);
        memory[0xFF] = 0x00;
        memory[0x00] = 0x31;
        memory[0x3105] = 0x7F;
        run(memory, cpu, {opcodes::LDA_INDIRECT_Y, 0xFF});
        check.that(cpu.getAcc() == 0x7F, "LDA ($FF),Y pointer must wrap in the zero page");

        // Indexed reads take 1 extra cycle when they cross a page
        cpu.reset(memory);
        cpu.setRX(0x04);
        memory[0x3103] = 0x11;
        cycles = run(memory, cpu, {opcodes::LDA_ABSOLUTE_X, 0xFF, 0x30});
        check.that(cpu.getAcc() == 0x11 && cycles == 5, "LDA $30FF,X crosses a page: 5 cycles");

        cpu.reset(memory);
        cpu.setRY(0x05);
        memory[0x3104] = 0x12;
        cycles = run(memory, cpu, {opcodes::LDA_ABSOLUTE_Y, 0xFF, 0x30});
        check.that(cpu.getAcc() == 0x12 && cycles == 5, "LDA $30FF,Y crosses a page: 5 cycles");

        cpu.reset(memory);
        cpu.setRY(0x05);
        memory[0x20] = 0xFF;
        memory[0x21] = 0x31; // pointer 0x31FF + Y = 0x3204
        memory[0x3204] = 0x13;
        cycles = run(memory, cpu, {opcodes::LDA_INDIRECT_Y, 0x20});
        check.that(cpu.getAcc() == 0x13 && cycles == 6, "LDA ($20),Y crosses a page: 6 cycles");

        // Stores always take the extra cycle, crossing a page or not
        cpu.reset(memory);
        cpu.setRX(0x04);
        cpu.setAcc(0x14);
        cycles = run(memory, cpu, {opcodes::STA_ABSOLUTE_X, 0xFF, 0x30});
        check.that(memory[0x3103] == 0x14 && cycles == 5, "STA $30FF,X crosses a page: 5 cycles");

        return finish(check, memory, cpu);
    }

    bool ADC_test(Memory &memory, CPU &cpu)
    {
        begin("ADC", memory, cpu);
        Check check;

        run_modes(check, memory, cpu, aluModes(ADC_OPS), 0x22, [](CPU &c) { c.setAcc(0x10); },
                  VERIFY { check.that(cpu.getAcc() == 0x32, where + ": wrong sum"); });

        struct Case { Byte a; Byte m; bool carryIn; bool decimal; Byte result; const char *flags; };
        const Case table[] = {
            {0x10, 0x22, false, false, 0x32, "czvn"},
            {0x10, 0x22, true,  false, 0x33, "czvn"},
            {0xFF, 0x01, false, false, 0x00, "CZvn"}, // carry out
            {0x50, 0x50, false, false, 0xA0, "czVN"}, // positive + positive = negative: overflow
            {0xD0, 0x90, false, false, 0x60, "CzVn"}, // negative + negative = positive: overflow
            {0x7F, 0x00, true,  false, 0x80, "czVN"},
            {0x80, 0x80, false, false, 0x00, "CZVn"},
            // Decimal mode
            {0x15, 0x27, false, true,  0x42, "c---"},
            {0x99, 0x01, false, true,  0x00, "C---"},
            {0x58, 0x46, true,  true,  0x05, "C---"},
            {0x09, 0x01, false, true,  0x10, "c---"}
        };
        for (const Case &c : table)
        {
            cpu.reset(memory);
            std::vector<Byte> program;
            if (c.decimal)
            {
                program.push_back(opcodes::SED);
            }
            program.insert(program.end(), {opcodes::LDA, c.a, c.carryIn ? opcodes::SEC : opcodes::CLC, opcodes::ADC, c.m});
            run(memory, cpu, program);

            const std::string context = std::string(c.decimal ? "decimal " : "") + "ADC " + std::to_string(c.a) + "+" + std::to_string(c.m);
            check.that(cpu.getAcc() == c.result, context + ": wrong result " + std::to_string(cpu.getAcc()));
            check.flags(cpu, c.flags, context);
        }
        return finish(check, memory, cpu);
    }

    bool SBC_test(Memory &memory, CPU &cpu)
    {
        begin("SBC", memory, cpu);
        Check check;

        run_modes(check, memory, cpu, aluModes(SBC_OPS), 0x10,
                  [](CPU &c) { c.setAcc(0x50); c.setProcessorFlags(CARRY_FLAG, true); },
                  VERIFY { check.that(cpu.getAcc() == 0x40, where + ": wrong difference"); });

        struct Case { Byte a; Byte m; bool carryIn; bool decimal; Byte result; const char *flags; };
        const Case table[] = {
            {0x50, 0x10, true,  false, 0x40, "Czvn"},
            {0x50, 0x10, false, false, 0x3F, "Czvn"}, // carry clear = borrow 1
            {0x00, 0x01, true,  false, 0xFF, "czvN"}, // borrow out
            {0x50, 0xB0, true,  false, 0xA0, "czVN"}, // positive - negative = negative: overflow
            {0x80, 0x01, true,  false, 0x7F, "CzVn"}, // negative - positive = positive: overflow
            {0x42, 0x42, true,  false, 0x00, "CZvn"},
            // Decimal mode
            {0x42, 0x13, true,  true,  0x29, "C---"},
            {0x00, 0x01, true,  true,  0x99, "c---"},
            {0x10, 0x01, true,  true,  0x09, "C---"}
        };
        for (const Case &c : table)
        {
            cpu.reset(memory);
            std::vector<Byte> program;
            if (c.decimal)
            {
                program.push_back(opcodes::SED);
            }
            program.insert(program.end(), {opcodes::LDA, c.a, c.carryIn ? opcodes::SEC : opcodes::CLC, opcodes::SBC, c.m});
            run(memory, cpu, program);

            const std::string context = std::string(c.decimal ? "decimal " : "") + "SBC " + std::to_string(c.a) + "-" + std::to_string(c.m);
            check.that(cpu.getAcc() == c.result, context + ": wrong result " + std::to_string(cpu.getAcc()));
            check.flags(cpu, c.flags, context);
        }
        return finish(check, memory, cpu);
    }

    bool AND_test(Memory &memory, CPU &cpu) { return logical_test("AND", memory, cpu, AND_OPS, reference_and); }
    bool ORA_test(Memory &memory, CPU &cpu) { return logical_test("ORA", memory, cpu, ORA_OPS, reference_ora); }
    bool EOR_test(Memory &memory, CPU &cpu) { return logical_test("EOR", memory, cpu, EOR_OPS, reference_eor); }

    bool BIT_test(Memory &memory, CPU &cpu)
    {
        begin("BIT", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::BIT_ZERO_PAGE, 3}, {AddrMode::ABS, opcodes::BIT_ABSOLUTE, 4}};

        struct Case { Byte a; Byte m; const char *flags; };
        const Case table[] = {
            {0x0F, 0xC0, "cZVN"}, // no common bit, bits 7 and 6 of the memory are copied
            {0xFF, 0x3F, "czvn"},
            {0x01, 0x41, "czVn"},
            {0xFF, 0x80, "czvN"}
        };
        for (const Case &c : table)
        {
            run_modes(check, memory, cpu, cases, c.m, [c](CPU &cpu2) { cpu2.setAcc(c.a); },
                      VERIFY {
                          check.flags(cpu, c.flags, where);
                          check.that(cpu.getAcc() == c.a, where + ": BIT must not change the accumulator");
                      });
        }
        return finish(check, memory, cpu);
    }

    bool CMP_test(Memory &memory, CPU &cpu)
    {
        return compare_test("CMP", memory, cpu, aluModes(CMP_OPS), opcodes::LDA, opcodes::CMP,
                            [](CPU &c) { c.setAcc(0x40); });
    }

    bool CPX_test(Memory &memory, CPU &cpu)
    {
        return compare_test("CPX", memory, cpu,
                            {{AddrMode::IMM, opcodes::CPX, 2}, {AddrMode::ZP, opcodes::CPX_ZERO_PAGE, 3}, {AddrMode::ABS, opcodes::CPX_ABSOLUTE, 4}},
                            opcodes::LDX, opcodes::CPX, [](CPU &c) { c.setRX(0x40); });
    }

    bool CPY_test(Memory &memory, CPU &cpu)
    {
        return compare_test("CPY", memory, cpu,
                            {{AddrMode::IMM, opcodes::CPY, 2}, {AddrMode::ZP, opcodes::CPY_ZERO_PAGE, 3}, {AddrMode::ABS, opcodes::CPY_ABSOLUTE, 4}},
                            opcodes::LDY, opcodes::CPY, [](CPU &c) { c.setRY(0x40); });
    }

    bool ASL_test(Memory &memory, CPU &cpu)
    {
        begin("ASL", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::ASL_ZERO_PAGE, 5}, {AddrMode::ZPX, opcodes::ASL_ZERO_PAGE_X, 6},
                                             {AddrMode::ABS, opcodes::ASL_ABSOLUTE, 6}, {AddrMode::ABSX, opcodes::ASL_ABSOLUTE_X, 7}};
        rmw_case(check, memory, cpu, opcodes::ASL_ACCUMULATOR, cases, 0x41, false, 0x82, "cz-N");
        rmw_case(check, memory, cpu, opcodes::ASL_ACCUMULATOR, cases, 0x80, true,  0x00, "CZ-n"); // old carry is ignored
        rmw_case(check, memory, cpu, opcodes::ASL_ACCUMULATOR, cases, 0xC0, false, 0x80, "Cz-N");
        return finish(check, memory, cpu);
    }

    bool LSR_test(Memory &memory, CPU &cpu)
    {
        begin("LSR", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::LSR_ZERO_PAGE, 5}, {AddrMode::ZPX, opcodes::LSR_ZERO_PAGE_X, 6},
                                             {AddrMode::ABS, opcodes::LSR_ABSOLUTE, 6}, {AddrMode::ABSX, opcodes::LSR_ABSOLUTE_X, 7}};
        rmw_case(check, memory, cpu, opcodes::LSR_ACCUMULATOR, cases, 0x82, true,  0x41, "cz-n"); // bit 7 is always cleared
        rmw_case(check, memory, cpu, opcodes::LSR_ACCUMULATOR, cases, 0x01, false, 0x00, "CZ-n");
        rmw_case(check, memory, cpu, opcodes::LSR_ACCUMULATOR, cases, 0x03, false, 0x01, "Cz-n");
        return finish(check, memory, cpu);
    }

    bool ROL_test(Memory &memory, CPU &cpu)
    {
        begin("ROL", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::ROL_ZERO_PAGE, 5}, {AddrMode::ZPX, opcodes::ROL_ZERO_PAGE_X, 6},
                                             {AddrMode::ABS, opcodes::ROL_ABSOLUTE, 6}, {AddrMode::ABSX, opcodes::ROL_ABSOLUTE_X, 7}};
        rmw_case(check, memory, cpu, opcodes::ROL_ACCUMULATOR, cases, 0x40, true,  0x81, "cz-N");
        rmw_case(check, memory, cpu, opcodes::ROL_ACCUMULATOR, cases, 0x80, false, 0x00, "CZ-n");
        rmw_case(check, memory, cpu, opcodes::ROL_ACCUMULATOR, cases, 0x80, true,  0x01, "Cz-n");
        return finish(check, memory, cpu);
    }

    bool ROR_test(Memory &memory, CPU &cpu)
    {
        begin("ROR", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::ROR_ZERO_PAGE, 5}, {AddrMode::ZPX, opcodes::ROR_ZERO_PAGE_X, 6},
                                             {AddrMode::ABS, opcodes::ROR_ABSOLUTE, 6}, {AddrMode::ABSX, opcodes::ROR_ABSOLUTE_X, 7}};
        rmw_case(check, memory, cpu, opcodes::ROR_ACCUMULATOR, cases, 0x02, true,  0x81, "cz-N");
        rmw_case(check, memory, cpu, opcodes::ROR_ACCUMULATOR, cases, 0x01, false, 0x00, "CZ-n");
        rmw_case(check, memory, cpu, opcodes::ROR_ACCUMULATOR, cases, 0x01, true,  0x80, "Cz-N");
        return finish(check, memory, cpu);
    }

    bool INC_test(Memory &memory, CPU &cpu)
    {
        begin("INC", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::INC_ZERO_PAGE, 5}, {AddrMode::ZPX, opcodes::INC_ZERO_PAGE_X, 6},
                                             {AddrMode::ABS, opcodes::INC_ABSOLUTE, 6}, {AddrMode::ABSX, opcodes::INC_ABSOLUTE_X, 7}};
        // INC does not touch the carry
        rmw_case(check, memory, cpu, 0, cases, 0x41, false, 0x42, "cz-n");
        rmw_case(check, memory, cpu, 0, cases, 0xFF, false, 0x00, "cZ-n");
        rmw_case(check, memory, cpu, 0, cases, 0x7F, true,  0x80, "Cz-N");
        return finish(check, memory, cpu);
    }

    bool DEC_test(Memory &memory, CPU &cpu)
    {
        begin("DEC", memory, cpu);
        Check check;
        const std::vector<ModeCase> cases = {{AddrMode::ZP, opcodes::DEC_ZERO_PAGE, 5}, {AddrMode::ZPX, opcodes::DEC_ZERO_PAGE_X, 6},
                                             {AddrMode::ABS, opcodes::DEC_ABSOLUTE, 6}, {AddrMode::ABSX, opcodes::DEC_ABSOLUTE_X, 7}};
        rmw_case(check, memory, cpu, 0, cases, 0x42, false, 0x41, "cz-n");
        rmw_case(check, memory, cpu, 0, cases, 0x01, false, 0x00, "cZ-n");
        rmw_case(check, memory, cpu, 0, cases, 0x00, true,  0xFF, "Cz-N");
        return finish(check, memory, cpu);
    }

    bool INX_INY_DEX_DEY_test(Memory &memory, CPU &cpu)
    {
        begin("INX/INY/DEX/DEY", memory, cpu);
        Check check;

        struct Case
        {
            const char *name;
            Byte load;
            Byte opcode;
            std::function<Byte(CPU &)> read;
            Byte start;
            Byte expected;
        };
        auto getX = [](CPU &c) { return c.getRX(); };
        auto getY = [](CPU &c) { return c.getRY(); };
        const Case table[] = {
            {"INX", opcodes::LDX, opcodes::INX, getX, 0x10, 0x11}, {"INX", opcodes::LDX, opcodes::INX, getX, 0xFF, 0x00},
            {"INX", opcodes::LDX, opcodes::INX, getX, 0x7F, 0x80},
            {"INY", opcodes::LDY, opcodes::INY, getY, 0x10, 0x11}, {"INY", opcodes::LDY, opcodes::INY, getY, 0xFF, 0x00},
            {"INY", opcodes::LDY, opcodes::INY, getY, 0x7F, 0x80},
            {"DEX", opcodes::LDX, opcodes::DEX, getX, 0x11, 0x10}, {"DEX", opcodes::LDX, opcodes::DEX, getX, 0x01, 0x00},
            {"DEX", opcodes::LDX, opcodes::DEX, getX, 0x00, 0xFF},
            {"DEY", opcodes::LDY, opcodes::DEY, getY, 0x11, 0x10}, {"DEY", opcodes::LDY, opcodes::DEY, getY, 0x01, 0x00},
            {"DEY", opcodes::LDY, opcodes::DEY, getY, 0x00, 0xFF}
        };
        for (const Case &c : table)
        {
            cpu.reset(memory);
            s32 cycles = run(memory, cpu, {c.load, c.start, c.opcode});
            const std::string context = std::string(c.name) + " " + std::to_string(c.start);
            check.that(c.read(cpu) == c.expected, context + ": wrong result");
            check.that(cycles == 4, context + ": expected 4 clock cycles (2 for the load + 2)");
            check.zeroNegative(cpu, c.expected, context);
        }
        return finish(check, memory, cpu);
    }

    bool TRANSFER_test(Memory &memory, CPU &cpu)
    {
        begin("TAX/TAY/TXA/TYA/TSX/TXS", memory, cpu);
        Check check;

        struct Case
        {
            const char *name;
            Byte load;
            Byte opcode;
            std::function<Byte(CPU &)> destination;
        };
        const Case table[] = {
            {"TAX", opcodes::LDA, opcodes::TAX, [](CPU &c) { return c.getRX(); }},
            {"TAY", opcodes::LDA, opcodes::TAY, [](CPU &c) { return c.getRY(); }},
            {"TXA", opcodes::LDX, opcodes::TXA, [](CPU &c) { return c.getAcc(); }},
            {"TYA", opcodes::LDY, opcodes::TYA, [](CPU &c) { return c.getAcc(); }}
        };
        for (const Case &c : table)
        {
            for (Byte value : {0x00, 0x42, 0x80})
            {
                cpu.reset(memory);
                s32 cycles = run(memory, cpu, {c.load, value, c.opcode});
                const std::string context = std::string(c.name) + " " + std::to_string(value);
                check.that(c.destination(cpu) == value, context + ": wrong value copied");
                check.that(cycles == 4, context + ": expected 4 clock cycles");
                check.zeroNegative(cpu, value, context);
            }
        }

        // TSX copies the low byte of the stack pointer and sets Z and N
        cpu.reset(memory);
        cpu.setStackPointer(0x01A5);
        s32 cycles = run(memory, cpu, {opcodes::TSX});
        check.that(cpu.getRX() == 0xA5 && cycles == 2, "TSX: X should be 0xA5 and take 2 clock cycles");
        check.zeroNegative(cpu, 0xA5, "TSX");

        // TXS sets the stack pointer and leaves the flags alone: LDA #1 clears N, so N must stay clear
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::LDX, 0x80, opcodes::LDA, 0x01, opcodes::TXS});
        // 0x0180 - 3 clock cycles from BRK
        check.that(cpu.getStackPointer() == (0x017D) && cycles == 6, "TXS: stack pointer should be 0x0180 and take 6 cycles in total");
        check.flags(cpu, "-z-n", "TXS must not change the flags");

        return finish(check, memory, cpu);
    }

    bool STACK_test(Memory &memory, CPU &cpu)
    {
        begin("PHA/PHP/PLA/PLP", memory, cpu);
        Check check;
        s32 cycles;

        cycles = run(memory, cpu, {opcodes::LDA, 0x42, opcodes::PHA});
        check.that(memory[0x01FF] == 0x42 && cycles == 5, "PHA: should push the accumulator and take 3 clock cycles");

        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::LDA, 0x80, opcodes::PHA, opcodes::LDA, 0x00, opcodes::PLA});
        check.that(cpu.getAcc() == 0x80 && cycles == 11, "PLA: should pull the accumulator and take 4 clock cycles");
        check.flags(cpu, "-z-N", "PLA");

        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::SEC, opcodes::SED, opcodes::PHP});
        // C = 0x01, D = 0x08, bit 5 is always set and the pushed copy has the break flag set
        check.that(memory[0x01FF] == 0x39 && cycles == 7, "PHP: should push the status (0x39) and take 3 clock cycles");

        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::LDA, 0x43, opcodes::PHA, opcodes::PLP});
        check.that(cycles == 9, "PLP: should take 4 clock cycles");
        check.flags(cpu, "CZVn", "PLP restores the flags from the stack"); // 0x43 = C, Z and V

        // The stack pointer goes down on a push and back up on a pull
        cpu.reset(memory);
        run(memory, cpu, {opcodes::PHA, opcodes::PHA, opcodes::PLA});
        // 2 pushes and 1 pull leave the pointer 1 slot below the top (0x01FE), and BRK then pushes 3 bytes
        check.that(cpu.getStackPointer() == 0x01FB, "the stack pointer should go down on a push and up on a pull");
        return finish(check, memory, cpu);
    }

    bool FLAG_INSTRUCTIONS_test(Memory &memory, CPU &cpu)
    {
        begin("CLC/SEC/CLD/SED/CLI/SEI/CLV", memory, cpu);
        Check check;
        s32 cycles;

        cycles = run(memory, cpu, {opcodes::SEC});
        check.that(cpu.getProcessorFlag(CARRY_FLAG) && cycles == 2, "SEC: should set the carry in 2 clock cycles");
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::SEC, opcodes::CLC});
        check.that(!cpu.getProcessorFlag(CARRY_FLAG) && cycles == 4, "CLC: should clear the carry in 2 clock cycles");

        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::SED});
        check.that(cpu.getProcessorFlag(DECIMAL_MODE_FLAG) && cycles == 2, "SED: should set the decimal flag in 2 clock cycles");
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::SED, opcodes::CLD});
        check.that(!cpu.getProcessorFlag(DECIMAL_MODE_FLAG) && cycles == 4, "CLD: should clear the decimal flag in 2 clock cycles");

        // BRK sets the interrupt flag itself, so look at the status BRK pushed on the stack
        // (0x01FF and 0x01FE hold the return address)
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::SEI});
        check.that((memory[0x01FD] & 0x04) != 0 && cycles == 2, "SEI: should set the interrupt disable flag in 2 clock cycles");
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::SEI, opcodes::CLI});
        check.that((memory[0x01FD] & 0x04) == 0 && cycles == 4, "CLI: should clear the interrupt disable flag in 2 clock cycles");

        // CLV: 0x50 + 0x50 overflows
        cpu.reset(memory);
        run(memory, cpu, {opcodes::LDA, 0x50, opcodes::ADC, 0x50});
        check.flags(cpu, "---N", "setup of the CLV test");
        check.that(cpu.getProcessorFlag(OVERFLOW_FLAG), "the overflow flag should be set before CLV");
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::LDA, 0x50, opcodes::ADC, 0x50, opcodes::CLV});
        check.that(!cpu.getProcessorFlag(OVERFLOW_FLAG) && cycles == 6, "CLV: should clear the overflow flag in 2 clock cycles");

        return finish(check, memory, cpu);
    }

    bool NOP_test(Memory &memory, CPU &cpu)
    {
        begin("NOP", memory, cpu);
        Check check;

        cpu.setRX(0x11);
        s32 cycles = run(memory, cpu, {opcodes::LDA, 0x80, opcodes::NOP, opcodes::NOP});
        check.that(cycles == 6, "NOP: should take 2 clock cycles");
        check.that(cpu.getAcc() == 0x80 && cpu.getRX() == 0x11, "NOP: must not change the registers");
        check.flags(cpu, "cz-N", "NOP must not change the flags");
        return finish(check, memory, cpu);
    }

    bool BRANCH_test(Memory &memory, CPU &cpu)
    {
        begin("BRANCHES", memory, cpu);
        Check check;

        // Program: <setup> <branch +2> LDX #0x77 ; X is only 0x77 when the branch is not taken
        struct Case { const char *name; Byte opcode; std::vector<Byte> setup; s32 setupCycles; bool taken; };
        const std::vector<Byte> overflow = {opcodes::CLC, opcodes::LDA, 0x50, opcodes::ADC, 0x50}; // sets V (and N)
        const Case table[] = {
            {"BCC", opcodes::BCC, {},                          0, true},  {"BCC", opcodes::BCC, {opcodes::SEC},                    2, false},
            {"BCS", opcodes::BCS, {opcodes::SEC},              2, true},  {"BCS", opcodes::BCS, {},                                0, false},
            {"BEQ", opcodes::BEQ, {opcodes::LDA, 0x00},        2, true},  {"BEQ", opcodes::BEQ, {opcodes::LDA, 0x01},              2, false},
            {"BNE", opcodes::BNE, {opcodes::LDA, 0x01},        2, true},  {"BNE", opcodes::BNE, {opcodes::LDA, 0x00},              2, false},
            {"BMI", opcodes::BMI, {opcodes::LDA, 0x80},        2, true},  {"BMI", opcodes::BMI, {opcodes::LDA, 0x01},              2, false},
            {"BPL", opcodes::BPL, {opcodes::LDA, 0x01},        2, true},  {"BPL", opcodes::BPL, {opcodes::LDA, 0x80},              2, false},
            {"BVS", opcodes::BVS, overflow,                    6, true},  {"BVS", opcodes::BVS, {},                                0, false},
            {"BVC", opcodes::BVC, {},                          0, true},  {"BVC", opcodes::BVC, overflow,                          6, false}
        };
        for (const Case &c : table)
        {
            cpu.reset(memory);
            std::vector<Byte> program = c.setup;
            program.insert(program.end(), {c.opcode, 0x02, opcodes::LDX, 0x77});
            s32 cycles = run(memory, cpu, program);

            const std::string context = std::string(c.name) + (c.taken ? " (taken)" : " (not taken)");
            check.that((cpu.getRX() == 0) == c.taken, context + ": wrong branch decision");
            // taken: 3 cycles. not taken: 2 cycles + the LDX that is not skipped
            check.that(cycles == c.setupCycles + (c.taken ? 3 : 4), context + ": wrong clock cycles (" + std::to_string(cycles) + ")");
        }

        // Taking a branch to another page costs 1 extra cycle: BNE at 0x02FD, next instruction at 0x02FF, target 0x030F
        cpu.reset(memory);
        s32 cycles = run(memory, cpu, {opcodes::BNE, 0x10}, 0x02FD);
        check.that(cycles == 4, "a branch to another page should take 4 clock cycles");
        check.that(memory[0x01FF] == 0x03 && memory[0x01FE] == 0x11, "the branch should have landed on 0x030F (the BRK there returns to 0x0311)");

        // Backward branch: LDX #3 ; loop: DEX ; BNE loop
        cpu.reset(memory);
        cycles = run(memory, cpu, {opcodes::LDX, 0x03, opcodes::DEX, opcodes::BNE, 0xFD});
        check.that(cpu.getRX() == 0, "backward branch: the loop should run until X is 0");
        check.that(cycles == 2 + 3 * 2 + 2 * 3 + 2, "backward branch: wrong clock cycles (" + std::to_string(cycles) + ")");

        return finish(check, memory, cpu);
    }

    bool JMP_test(Memory &memory, CPU &cpu)
    {
        begin("JMP", memory, cpu);
        Check check;

        memory[0x3000] = opcodes::LDA;
        memory[0x3001] = 0x5A;
        s32 cycles = run(memory, cpu, {opcodes::JMP_ABSOLUTE, 0x00, 0x30});
        check.that(cpu.getAcc() == 0x5A && cycles == 3 + 2, "JMP absolute: should jump to 0x3000 in 3 clock cycles");

        cpu.reset(memory);
        memory[0x3000] = 0x00; // pointer -> 0x4000
        memory[0x3001] = 0x40;
        memory[0x4000] = opcodes::LDA;
        memory[0x4001] = 0x5B;
        cycles = run(memory, cpu, {opcodes::JMP_INDIRECT, 0x00, 0x30});
        check.that(cpu.getAcc() == 0x5B && cycles == 5 + 2, "JMP indirect: should jump to 0x4000 in 5 clock cycles");

        // The original 6502 bug: a pointer at $xxFF takes its high byte from $xx00
        cpu.reset(memory);
        memory[0x30FF] = 0x00;
        memory[0x3000] = 0x40; // high byte used by the real CPU
        memory[0x3100] = 0x50; // high byte a "correct" CPU would use
        memory[0x4000] = opcodes::LDA;
        memory[0x4001] = 0x5C;
        memory[0x5000] = opcodes::LDA;
        memory[0x5001] = 0x01;
        run(memory, cpu, {opcodes::JMP_INDIRECT, 0xFF, 0x30});
        check.that(cpu.getAcc() == 0x5C, "JMP ($30FF) should read the high byte from $3000, like the original 6502");

        return finish(check, memory, cpu);
    }

    bool RTS_test(Memory &memory, CPU &cpu)
    {
        begin("JSR/RTS", memory, cpu);
        Check check;

        // main: JSR 0x3000 ; LDX #0x11
        // 0x3000: JSR 0x3100 ; LDY #0x33 ; RTS
        // 0x3100: LDA #0x22 ; RTS
        memory[0x3000] = opcodes::JSR;
        memory[0x3001] = 0x00;
        memory[0x3002] = 0x31;
        memory[0x3003] = opcodes::LDY;
        memory[0x3004] = 0x33;
        memory[0x3005] = opcodes::RTS;
        memory[0x3100] = opcodes::LDA;
        memory[0x3101] = 0x22;
        memory[0x3102] = opcodes::RTS;
        s32 cycles = run(memory, cpu, {opcodes::JSR, 0x00, 0x30, opcodes::LDX, 0x11});

        check.that(cpu.getAcc() == 0x22 && cpu.getRY() == 0x33 && cpu.getRX() == 0x11, "nested JSR/RTS: every part of the program should have run");
        check.that(cycles == 6 + 6 + 2 + 6 + 2 + 6 + 2, "JSR/RTS: wrong clock cycles (" + std::to_string(cycles) + ")");
        check.that(cpu.getStackPointer() == 0x01FC, "JSR/RTS: the stack should be balanced (BRK pushed 3 bytes)");
        return finish(check, memory, cpu);
    }

    bool RTI_test(Memory &memory, CPU &cpu)
    {
        begin("RTI", memory, cpu);
        Check check;

        // Stack as an interrupt would leave it: status, return address low, return address high
        cpu.setStackPointer(0x01FC);
        memory[0x01FD] = 0x09; // C and D
        memory[0x01FE] = 0x00;
        memory[0x01FF] = 0x30; // return to 0x3000
        memory[0x3000] = opcodes::LDA;
        memory[0x3001] = 0x66;
        s32 cycles = run(memory, cpu, {opcodes::RTI});

        check.that(cpu.getAcc() == 0x66, "RTI: should return to 0x3000");
        check.that(cycles == 6 + 2, "RTI: should take 6 clock cycles");
        check.that(cpu.getProcessorFlag(CARRY_FLAG) && cpu.getProcessorFlag(DECIMAL_MODE_FLAG), "RTI: should restore the flags from the stack");
        check.that(cpu.getStackPointer() == 0x01FC, "RTI: the stack pointer should be back at the top (BRK pushed 3 bytes)");
        return finish(check, memory, cpu);
    }

    bool BRK_test(Memory &memory, CPU &cpu)
    {
        begin("BRK", memory, cpu);
        Check check;

        memory[0xFFFE] = 0x00; // IRQ vector -> 0x4000
        memory[0xFFFF] = 0x40;
        memory[0x0200] = opcodes::SEC;
        memory[0x0201] = opcodes::BRK;
        s32 cycles = cpu.exec(memory);

        check.that(cycles == 2 + 7, "BRK: should take 7 clock cycles");
        check.that(cpu.getProgramCounter() == 0x4000, "BRK: should load the program counter from $FFFE/F");
        check.that(memory[0x01FF] == 0x02 && memory[0x01FE] == 0x03, "BRK: should push the address of BRK + 2 (high byte first)");
        check.that(memory[0x01FD] == 0x31, "BRK: should push the status with the break flag set (0x31 = C + B + bit 5)");
        check.that(cpu.getProcessorFlag(INTERRUPT_DISABLE) && cpu.getProcessorFlag(BREAK_COMMAND), "BRK: should set the interrupt disable and break flags");
        check.that(cpu.getStackPointer() == 0x01FC, "BRK: should have pushed 3 bytes");
        return finish(check, memory, cpu);
    }

    bool OPCODE_TABLE_test(Memory &memory, CPU &cpu)
    {
        begin("OPCODE TABLE", memory, cpu);
        Check check;
        check.that(opcodes::opcode_map.size() == 151, "the official 6502 has 151 opcodes, the table has " + std::to_string(opcodes::opcode_map.size()));
        check.that(opcodes::opcode_map.at(opcodes::BRK) == 7 && opcodes::opcode_map.at(opcodes::LDA) == 2, "the table should hold the cycles of BRK and LDA");
        check.that(opcodes::opcode_map.find(0x02) == opcodes::opcode_map.end(), "unofficial opcodes are not in the table");
        return finish(check, memory, cpu);
    }
}

/**
 * Run's tests for every operation avaliable in this emulator. Verifies CPU operations
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool is_cpu_with_errors(Memory &memory, CPU &cpu)
{
    std::cout << "TESTING CPU...\n";

    typedef bool (*Test)(Memory &, CPU &);
    const Test tests[] = {
        // The original tests
        LDA_test,
        LDA_ZERO_PAGE_test,
        LDA_ZERO_PAGE_X_test,
        LDX_test,
        LDX_ZERO_PAGE_test,
        LDY_test,
        LDY_ZERO_PAGE_test,
        JSR_test,
        STA_ZERO_PAGE_test,
        STA_ZERO_PAGE_X_test,
        STA_ZERO_PAGE_ABSOLUTE_test,
        STA_ZERO_PAGE_ABSOLUTE_X_test,
        STA_ZERO_PAGE_ABSOLUTE_Y_test,
        STX_ZERO_PAGE_test,
        STY_ZERO_PAGE_test,
        // Every addressing mode, flags and clock cycles of the load / store instructions
        LOAD_ALL_MODES_test,
        LOAD_FLAGS_test,
        STORE_ALL_MODES_test,
        ADDRESSING_EDGE_CASES_test,
        // The rest of the instruction set
        ADC_test,
        SBC_test,
        AND_test,
        ORA_test,
        EOR_test,
        BIT_test,
        CMP_test,
        CPX_test,
        CPY_test,
        ASL_test,
        LSR_test,
        ROL_test,
        ROR_test,
        INC_test,
        DEC_test,
        INX_INY_DEX_DEY_test,
        TRANSFER_test,
        STACK_test,
        FLAG_INSTRUCTIONS_test,
        NOP_test,
        BRANCH_test,
        JMP_test,
        RTS_test,
        RTI_test,
        BRK_test,
        OPCODE_TABLE_test,
        assembler_test
    };

    for (Test test : tests)
    {
        if (test(memory, cpu))
        {
            return 1;
        }
    }

    std::cout << "\nALL TESTS PASSED!" << std::endl;
    return 0;
}

/**
 * Run's tests for LDA operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDA_test(Memory &memory, CPU &cpu)
{
    begin("LDA", memory, cpu);
    Check check;

    s32 cycles = run(memory, cpu, {opcodes::LDA, 0x42});

    check.that(cpu.getAcc() == 0x42, "accumulator should be 0x42");
    check.that(cycles == 2, "should take 2 clock cycles");
    check.zeroNegative(cpu, 0x42, "LDA");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for LDA ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDA_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    begin("LDA ZERO PAGE", memory, cpu);
    Check check;

    memory[0xff] = 0x42;
    s32 cycles = run(memory, cpu, {opcodes::LDA_ZERO_PAGE, 0xff});

    check.that(cpu.getAcc() == 0x42, "accumulator should be 0x42");
    check.that(cycles == 3, "should take 3 clock cycles");
    check.zeroNegative(cpu, 0x42, "LDA");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for LDA ZERO PAGE X operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDA_ZERO_PAGE_X_test(Memory &memory, CPU &cpu)
{
    begin("LDA ZERO PAGE X", memory, cpu);
    Check check;

    cpu.setRX(0x11);
    memory[0x31] = 0x22;
    s32 cycles = run(memory, cpu, {opcodes::LDA_ZERO_PAGE_X, 0x20});

    check.that(cpu.getAcc() == 0x22, "accumulator should be 0x22");
    check.that(cycles == 4, "should take 4 clock cycles");
    check.zeroNegative(cpu, 0x22, "LDA");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for LDX operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDX_test(Memory &memory, CPU &cpu)
{
    begin("LDX", memory, cpu);
    Check check;

    s32 cycles = run(memory, cpu, {opcodes::LDX, 0x42});

    check.that(cpu.getRX() == 0x42, "X should be 0x42");
    check.that(cycles == 2, "should take 2 clock cycles");
    check.zeroNegative(cpu, 0x42, "LDX");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for LDX ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDX_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    begin("LDX ZERO PAGE", memory, cpu);
    Check check;

    memory[0xff] = 0x42;
    s32 cycles = run(memory, cpu, {opcodes::LDX_ZERO_PAGE, 0xff});

    check.that(cpu.getRX() == 0x42, "X should be 0x42");
    check.that(cycles == 3, "should take 3 clock cycles");
    check.zeroNegative(cpu, 0x42, "LDX");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for LDY operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDY_test(Memory &memory, CPU &cpu)
{
    begin("LDY", memory, cpu);
    Check check;

    s32 cycles = run(memory, cpu, {opcodes::LDY, 0x42});

    check.that(cpu.getRY() == 0x42, "Y should be 0x42");
    check.that(cycles == 2, "should take 2 clock cycles");
    check.zeroNegative(cpu, 0x42, "LDY");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for LDY ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDY_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    begin("LDY ZERO PAGE", memory, cpu);
    Check check;

    memory[0xff] = 0x42;
    s32 cycles = run(memory, cpu, {opcodes::LDY_ZERO_PAGE, 0xff});

    check.that(cpu.getRY() == 0x42, "Y should be 0x42");
    check.that(cycles == 3, "should take 3 clock cycles");
    check.zeroNegative(cpu, 0x42, "LDY");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for JSR operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool JSR_test(Memory &memory, CPU &cpu)
{
    begin("JSR", memory, cpu);
    Check check;

    memory[0x4241] = opcodes::LDA;
    memory[0x4242] = 0x30;
    s32 cycles = run(memory, cpu, {opcodes::JSR, 0x41, 0x42});

    check.that(cpu.getAcc() == 0x30, "the subroutine at 0x4241 should have run");
    check.that(cycles == 6 + 2, "JSR should take 6 clock cycles");
    // The address of the last byte of the JSR (0x0202) is pushed, high byte first
    check.that(memory[0x01FF] == 0x02 && memory[0x01FE] == 0x02, "JSR should push the return address minus one (0x0202)");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STA ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    begin("STA ZERO PAGE", memory, cpu);
    Check check;

    cpu.setAcc(0x23);
    s32 cycles = run(memory, cpu, {opcodes::STA_ZERO_PAGE, 0xfb});

    check.that(memory[0xfb] == cpu.getAcc(), "0xfb should hold the accumulator");
    check.that(cycles == 3, "should take 3 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STA ZERO PAGE X operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_X_test(Memory &memory, CPU &cpu)
{
    begin("STA ZERO PAGE X", memory, cpu);
    Check check;

    cpu.setAcc(0x31);
    cpu.setRX(0x8a);
    s32 cycles = run(memory, cpu, {opcodes::STA_ZERO_PAGE_X, 0xfa});

    // 0xfa + 0x8a = 0x184, which wraps around inside the zero page to 0x84
    check.that(memory[0x84] == cpu.getAcc(), "0x84 should hold the accumulator (zero page wraps around)");
    check.that(cycles == 4, "should take 4 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STA ZERO PAGE ABSOLUTE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_ABSOLUTE_test(Memory &memory, CPU &cpu)
{
    begin("STA ZERO PAGE ABSOLUTE", memory, cpu);
    Check check;

    cpu.setAcc(0x33);
    s32 cycles = run(memory, cpu, {opcodes::STA_ZERO_PAGE_ABSOLUTE, 0xff, 0xfA});

    check.that(memory[0xfaff] == cpu.getAcc(), "0xfaff should hold the accumulator");
    check.that(cycles == 4, "should take 4 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STA ZERO PAGE ABSOLUTE X operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_ABSOLUTE_X_test(Memory &memory, CPU &cpu)
{
    begin("STA ZERO PAGE ABSOLUTE X", memory, cpu);
    Check check;

    cpu.setAcc(0x33);
    cpu.setRX(0x8a);
    s32 cycles = run(memory, cpu, {opcodes::STA_ZERO_PAGE_ABSOLUTE_X, 0xff, 0xfA});

    check.that(memory[0xfaff + 0x8a] == cpu.getAcc(), "0xfb89 should hold the accumulator");
    check.that(cycles == 5, "should take 5 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STA ZERO PAGE ABSOLUTE Y operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_ABSOLUTE_Y_test(Memory &memory, CPU &cpu)
{
    begin("STA ZERO PAGE ABSOLUTE Y", memory, cpu);
    Check check;

    cpu.setAcc(0x33);
    cpu.setRY(0x8a);
    s32 cycles = run(memory, cpu, {opcodes::STA_ZERO_PAGE_ABSOLUTE_Y, 0xff, 0xfA});

    check.that(memory[0xfaff + 0x8a] == cpu.getAcc(), "0xfb89 should hold the accumulator");
    check.that(cycles == 5, "should take 5 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STX ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STX_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    begin("STX ZERO PAGE", memory, cpu);
    Check check;

    cpu.setRX(0x23);
    s32 cycles = run(memory, cpu, {opcodes::STX_ZERO_PAGE, 0xfb});

    check.that(memory[0xfb] == cpu.getRX(), "0xfb should hold X");
    check.that(cycles == 3, "should take 3 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for STY ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STY_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    begin("STY ZERO PAGE", memory, cpu);
    Check check;

    cpu.setRY(0x23);
    s32 cycles = run(memory, cpu, {opcodes::STY_ZERO_PAGE, 0xfb});

    check.that(memory[0xfb] == cpu.getRY(), "0xfb should hold Y");
    check.that(cycles == 3, "should take 3 clock cycles");
    return finish(check, memory, cpu);
}

/**
 * Run's tests for the assembler
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool assembler_test(Memory &memory, CPU &cpu)
{
    std::cout << "ASSEMBLER TEST ";
    compileAssemblyProgram(cpu, memory, "assembly_code_unit_test.txt");

    cpu.exec(memory);

    if (cpu.getAcc() != 0x32 || cpu.getRX() != 0x0A)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}