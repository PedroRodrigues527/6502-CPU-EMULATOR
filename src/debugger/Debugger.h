#include "../components/Cpu.h"
#include "../components/Memory.h"
#include "../opcodes/opcodes.h"

#include <vector>
#include <unordered_set>

class Debugger {
public:
    Debugger(CPU& cpu, Memory& memory);

    void runConsole();
private:
    CPU& cpu;
    Memory& memory;

    void cmdRegs();
    void cmdMem(std::vector<std::string> args);
    void cmdStep();
    void cmdRun();
    void cmdBreakpoint(std::vector<std::string> args);

    std::unordered_set<Word> breakpoints;
};

