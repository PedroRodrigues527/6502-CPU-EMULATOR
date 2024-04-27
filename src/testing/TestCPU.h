#ifndef TESTCPU_H
#define TESTCPU_H

#include "../components/Memory.h"
#include "../components/Cpu.h"

#include <iostream>

bool isCPUWithErrors(Memory &memory, CPU &cpu);

bool LDA_test(Memory &memory, CPU &cpu);
bool LDA_ZERO_PAGE_test(Memory &memory, CPU &cpu);
bool LDA_ZERO_PAGE_X_test(Memory &memory, CPU &cpu);

bool LDX_test(Memory &memory, CPU &cpu);
bool LDX_ZERO_PAGE_test(Memory &memory, CPU &cpu);

bool LDY_test(Memory &memory, CPU &cpu);
bool LDY_ZERO_PAGE_test(Memory &memory, CPU &cpu);

bool JSR_test(Memory &memory, CPU &cpu);

bool STA_ZERO_PAGE_test(Memory &memory, CPU &cpu);
bool STA_ZERO_PAGE_X_test(Memory &memory, CPU &cpu);
bool STA_ZERO_PAGE_ABSOLUTE_test(Memory &memory, CPU &cpu);
bool STA_ZERO_PAGE_ABSOLUTE_X_test(Memory &memory, CPU &cpu);
bool STA_ZERO_PAGE_ABSOLUTE_Y_test(Memory &memory, CPU &cpu);

bool STX_ZERO_PAGE_test(Memory &memory, CPU &cpu);
bool STY_ZERO_PAGE_test(Memory &memory, CPU &cpu);

#endif