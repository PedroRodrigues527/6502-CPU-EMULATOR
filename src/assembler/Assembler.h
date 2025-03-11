#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../components/Cpu.h"
#include "../components/Memory.h"

void compileAssemblyProgram(CPU &cpu, Memory &memory, std::string assembly_code_file = "assembly_code.txt");
int getCycles(std::string instruction, bool isAddress);
Byte getOpcode(std::string instruction, bool isAddress);

#endif