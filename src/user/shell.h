#pragma once

#include "..\api\api.h"

extern "C" size_t __stdcall shell(kiv_os::TRegisters &regs);

void dir(char arg[256][1025], int argc, char * cur_path, kiv_os::TProcess_Startup_Info process_info, size_t buffer_size);