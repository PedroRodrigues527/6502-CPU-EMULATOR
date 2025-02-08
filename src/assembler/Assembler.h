#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../components/Cpu.h"
#include "../components/Memory.h"

void newCompileAssemblyProgram(CPU &cpu, Memory &memory);
void compileAssemblyProgram(CPU &cpu, Memory &memory, std::string line);
int getCycles(std::string instruction, bool isAddress);
Byte getOpcode(std::string instruction, bool isAddress);

#endif