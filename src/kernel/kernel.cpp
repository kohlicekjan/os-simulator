#pragma once

#include "kernel.h"
#include "io.h"
#include "process.h"
#include <Windows.h>



HMODULE User_Programs;


void Set_Error(const bool failed, kiv_os::TRegisters &regs) {
	if (failed) {
		regs.flags.carry = true;
		regs.rax.r = GetLastError();
	}
	else
		regs.flags.carry = false;
}


void Initialize_Kernel() {
	User_Programs = LoadLibrary(L"user.dll");	
}

void Shutdown_Kernel() {
	FreeLibrary(User_Programs);
}

void __stdcall Sys_Call(kiv_os::TRegisters &regs) {
	
	switch (regs.rax.h) {
		case kiv_os::scIO:		HandleIO(regs);
			break;
		case kiv_os::scProc:	HandleProcess(regs);
			break;
	}

}

void __stdcall Run_VM() {
	Initialize_Kernel();

	//spustime shell - v realnem OS bychom ovsem spousteli login
	kiv_os::TEntry_Point shell = (kiv_os::TEntry_Point)GetProcAddress(User_Programs, "shell");
	if (shell) {
		//spravne se ma shell spustit pres clone!
		kiv_os::TRegisters regs{ 0 };
		//struktura pro inicializaci argumentu procesu
		kiv_os::TProcess_Startup_Info init;
		int pid = -1;
		//vytvoreni init procesu s PID = 0
		if (createProcess("shell", &init) == S_OK) {
			shell(regs);
		}
		
	}

	Shutdown_Kernel();
}