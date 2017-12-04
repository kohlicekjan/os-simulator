#pragma once

extern "C" size_t __stdcall freq(const kiv_os::TRegisters &regs);

void dec_To_Hex(int decimal, char hex[]);