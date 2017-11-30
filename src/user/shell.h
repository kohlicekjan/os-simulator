#pragma once

#include "..\api\api.h"

extern "C" size_t __stdcall shell(kiv_os::TRegisters &regs);


//nasledujici funkce si dejte do vlastnich souboru
extern "C" size_t __stdcall rgen(const kiv_os::TRegisters &regs) { return 0; }
extern "C" size_t __stdcall freq(const kiv_os::TRegisters &regs) { return 0; }
