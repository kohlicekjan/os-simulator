#pragma once

#include "..\api\api.h"

void HandleIO(kiv_os::TRegisters &regs);

void Create_File(kiv_os::TRegisters &regs);

void Write_File(kiv_os::TRegisters &regs);

void Read_File(kiv_os::TRegisters &regs);

void Delete_File(kiv_os::TRegisters &regs);

void Set_File_Position(kiv_os::TRegisters &regs);

void Get_File_Position(kiv_os::TRegisters &regs);

void Get_Current_Directory(kiv_os::TRegisters &regs);

void Set_Current_Directory(kiv_os::TRegisters &regs);

void Create_Pipe(kiv_os::TRegisters &regs);

kiv_os::THandle get_stdin();
kiv_os::THandle get_stdout();
kiv_os::THandle get_stderr();