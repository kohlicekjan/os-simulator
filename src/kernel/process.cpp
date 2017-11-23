#include "../api/api.h"
#include "process.h"
#include "kernel.h"
#include "file_system.h"

std::mutex pcb_mutex;							// mutex pro zamèení PCB
PCB * process_table[PCB_SIZE] = { nullptr };	//PCB max 256

void HandleProcess(kiv_os::TRegisters &regs) {
	switch (regs.rax.l){
		case kiv_os::scClone: {
			
			regs.rax.r = (decltype(regs.rax.r))createProcess(reinterpret_cast<char *>(regs.rdx.r), reinterpret_cast<kiv_os::TProcess_Startup_Info*>(regs.rdi.r));
			break;
		}
		case kiv_os::scWait_For: {
			
			break;
		}
		case kiv_os::scReturnPCB: {
			Get_PCB(regs);
			break;
		}
	}
}

HRESULT createProcess(char *name, kiv_os::TProcess_Startup_Info *arg) {
	int pid = -1;
	int parent_pid = -1;
	std::vector<kiv_os::THandle> descriptors;
	
	//descriptory
	descriptors.push_back(static_cast<kiv_os::THandle>(arg->std_in));
	descriptors.push_back(static_cast<kiv_os::THandle>(arg->std_out));
	descriptors.push_back(static_cast<kiv_os::THandle>(arg->std_err));
	
	//vytvoøení záznamu o procesu
	{
		std::lock_guard<std::mutex> lock(pcb_mutex);
		for (int i = 0; i < PCB_SIZE; i++) {
			//kdyz se najde volne misto, vytvori novy zaznam
			if (process_table[i] == nullptr) {
				process_table[i] = new PCB();
				pid = i;		
				break;
			}
		}
	}

	//kdyz neni misto pro dalsi proces, vrati false
	if (pid == -1) {
		return S_FALSE;
	}

	for (int i = 0; i < PCB_SIZE; i++) {
		if(process_table[i] != NULL){
			if (process_table[i]->thread.get_id() == std::this_thread::get_id()) {
				parent_pid = i;
				break;
			}
		}
	}

	
	process_table[pid]->name = name;
	process_table[pid]->par_pid = parent_pid;	
	
	if(parent_pid > 0 && process_table[pid] != NULL){
		process_table[pid]->path = process_table[parent_pid]->path;
	}
	else {
		process_table[pid]->path = get_root()->filePath.c_str();	
	}

	for(auto &descriptor : descriptors){
		process_table[pid]->descriptors.push_back(descriptor);
	}
	
	//najde vstupni pod noveho procesu
	kiv_os::TEntry_Point func = (kiv_os::TEntry_Point)GetProcAddress(User_Programs, name);
	
	//vstupni bod se nepovedlo nalezt 
	if (!func) {
		std::lock_guard<std::mutex> lock(pcb_mutex);
		delete process_table[pid];
		process_table[pid] = nullptr;
		SetLastError(kiv_os::erInvalid_Handle);
		return S_FALSE;
	}

	// u prvniho shellu nechceme vytvaret dalsi vlakno - to prvni by skoncilo a zustal by sirotek :(
	if(pid != 0){
		process_table[pid]->thread = std::thread(runProcess, func, pid, arg->arg , true);
	}

	return S_OK;
}


void runProcess(kiv_os::TEntry_Point func, int pid, char* arg, bool stdinIsConsole) {
	
	kiv_os::TRegisters regs;
	//ulozeni hodnot do registru
	regs.rdx.r = (decltype(regs.rdx.r))process_table[pid]->name;
	regs.rcx.r = (decltype(regs.rcx.r))arg;
	regs.rax.r = (decltype(regs.rax.r))&process_table[pid]->descriptors;
	
	//spusteni procesu
	size_t ret = func(regs);
	
}

HRESULT joinProcess(int pid) {
	std::thread::id tid = process_table[pid]->thread.get_id();
	//ukonceni procesu
	process_table[pid]->thread.join();
	//smazani zaznamu o procesu
	delete process_table[pid];
	process_table[pid] = nullptr;
	return S_OK;
}

void Get_PCB(kiv_os::TRegisters regs) {
	regs.rdx.r = (decltype(regs.rdx.r))&process_table;
	
}