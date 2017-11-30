#pragma once

#include "kernel.h"
#include "io.h"
#include "process.h"
#include "handles.h"
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

void creating_os_structure() {
	kiv_os::TRegisters regs{ 0 };

	regs.rax.h = kiv_os::scIO;
	regs.rax.l = kiv_os::scCreate_File;
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>("C://system");
	regs.rcx.h = 2;
	regs.rcx.l = 1;

	HandleIO(regs);
	regs.rdx.x = regs.rax.x;
	regs.rax.l = kiv_os::scClose_Handle;
	HandleIO(regs);
	
	regs.rax.h = kiv_os::scIO;
	regs.rax.l = kiv_os::scCreate_File;
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>("C://system/term");
	regs.rcx.h = 2;
	regs.rcx.l = 1;

	HandleIO(regs);
	regs.rdx.x = regs.rax.x;
	regs.rax.l = kiv_os::scClose_Handle;
	HandleIO(regs);

	regs.rax.h = kiv_os::scIO;
	regs.rax.l = kiv_os::scCreate_File;
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>("C://system/proc");
	regs.rcx.h = 2;
	regs.rcx.l = 1;

	HandleIO(regs);
	regs.rdx.x = regs.rax.x;
	regs.rax.l = kiv_os::scClose_Handle;
	HandleIO(regs);
}

void __stdcall Run_VM() {
	Initialize_Kernel();

	//spustime shell - v realnem OS bychom ovsem spousteli login
	kiv_os::TEntry_Point shell = (kiv_os::TEntry_Point)GetProcAddress(User_Programs, "shell");

	if (shell) {

		creating_os_structure();
		char *boot_string = "Boot succesful\n";
		fwrite(boot_string, sizeof(char), strlen(boot_string), stdout);
		//spravne se ma shell spustit pres clone!
		kiv_os::TRegisters regs{ 0 };

		//struktura pro inicializaci argumentu procesu
		kiv_os::TProcess_Startup_Info init;

		init.std_in = Convert_Native_Handle(stdin);
		init.std_out = Convert_Native_Handle(stdout);
		init.std_err = Convert_Native_Handle(stderr);
		int pid = -1;
		//vytvoreni init procesu s PID = 0
		if (createProcess("shell", &init) == S_OK) {
			regs.rdx.r = (decltype(regs.rdx.r))&init;
			shell(regs);
		}
		
	}

	Shutdown_Kernel();
}
