#pragma once

extern "C" size_t __stdcall freq(const kiv_os::TRegisters &regs);

char* dec_To_Hex(int decimal);