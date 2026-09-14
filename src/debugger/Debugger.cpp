
#include "Debugger.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Debugger::Debugger(CPU& cpu, Memory& memory)
    : cpu(cpu), memory(memory) {}

void Debugger::runConsole() {
    std::string input;

    std::cout << "6502 Debugger Console\n";
    std::cout << "Type 'help' for commands.\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::string cmd;
        std::vector<std::string> args;

        iss >> cmd;
        std::string arg;
        while (iss >> arg)
            args.push_back(arg);

        if (cmd == "exit") break;
        // if (cmd == "regs") { cmdRegs(); continue; }
        // if (cmd == "mem")  { cmdMem(args); continue; }
        // if (cmd == "step") { cmdStep(); continue; }
        // if (cmd == "run")  { cmdRun(); continue; }
        // if (cmd == "break") { cmdBreakpoint(args); continue; }

        std::cout << "Unknown command.\n";
    }
}

void Debugger::cmdRegs() {
    std::cout << std::hex << std::uppercase;

    std::cout << "PC=" << cpu.getProgramCounter()
              << " A=" << (int)cpu.getAcc()
              << " X=" << (int)cpu.getRX()
              << " Y=" << (int)cpu.getRY()
            //   << " SP=" << cpu.getStackPointer()
              << "\nFLAGS: "
            //   << "N=" << cpu.flag(NEGATIVE_FLAG)
            //   << " Z=" << cpu.flag(ZERO_FLAG)
            //   << " C=" << cpu.flag(CARRY_FLAG)
              << "\n";
}

