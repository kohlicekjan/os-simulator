#include "../api/api.h"
#include "process.h"
#include "kernel.h"

std::mutex pcb_mutex;							// mutex pro zamèení PCB
PCB * process_table[PCB_SIZE] = { nullptr };	//PCB max 256

void HandleProcess(kiv_os::TRegisters &regs) {
	switch (regs.rax.l){
		case kiv_os::scClone: {
			int pid = -1;
			regs.rax.r = (decltype(regs.rax.r))createProcess(reinterpret_cast<char *>(regs.rdx.r), &pid, reinterpret_cast<kiv_os::TProcess_Startup_Info*>(regs.rdi.r));
			break;
		}
		case kiv_os::scWait_For:{
			
			break;
		}
	}
}

HRESULT createProcess(char *name, int *parent_pid, kiv_os::TProcess_Startup_Info *arg) {
	int pid = -1;
	std::vector<kiv_os::THandle> descriptors;
	
		descriptors.push_back(static_cast<kiv_os::THandle>(arg->std_in));
		descriptors.push_back(static_cast<kiv_os::THandle>(arg->std_out));
		descriptors.push_back(static_cast<kiv_os::THandle>(arg->std_err));
	
	//vytvoøení záznamu o procesu
	{
		std::lock_guard<std::mutex> lock(pcb_mutex);
		for (int i = 0; i < PCB_SIZE; i++) {
			if (process_table[i] == nullptr) {
				process_table[i] = new PCB();
				pid = i;
				
				break;
			}
		}
	}

	if (pid == -1) {
		//v tabulce neni misto	
		return S_FALSE;
	}
	
	process_table[pid]->name = name;
	process_table[pid]->par_pid = *parent_pid;
	
	
		for(auto &descriptor : descriptors){
			process_table[pid]->descriptors.push_back(descriptor);
		}
	
	
	kiv_os::TEntry_Point func = (kiv_os::TEntry_Point)GetProcAddress(User_Programs, name);
	if (!func) {
		//vstupni bod se nepovedlo nalezt 
		std::lock_guard<std::mutex> lock(pcb_mutex);
		delete process_table[pid];
		process_table[pid] = nullptr;
		SetLastError(kiv_os::erInvalid_Handle);
		return S_FALSE;
	}
	process_table[pid]->thread = std::thread(runProcess, func, pid, arg->arg , true);
	
	return S_OK;
}


void runProcess(kiv_os::TEntry_Point func, int pid, char* arg, bool stdinIsConsole) {
	kiv_os::TRegisters regs;

	regs.rdx.r = (decltype(regs.rdx.r))process_table[pid]->name;
	regs.rcx.r = (decltype(regs.rcx.r))arg;
	regs.rax.r = (decltype(regs.rax.r))&process_table[pid]->descriptors;

	size_t ret = func(regs);
}