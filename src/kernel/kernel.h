#pragma once

#include "..\api\api.h"
#include <Windows.h>

extern HMODULE User_Programs;

void Set_Error(const bool failed, kiv_os::TRegisters &regs);
void creating_os_structure();

extern "C" void __stdcall Sys_Call(kiv_os::TRegisters &regs);
extern "C" void __stdcall Run_VM();
