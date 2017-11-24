#include "../api/api.h"
#include "process.h"
#include "kernel.h"
#include "file_system.h"
#include <stdio.h>

#include <chrono>

std::mutex pcb_mutex;							// mutex pro zam�en� PCB
PCB * process_table[PCB_SIZE];	//PCB max 256

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
	
	//vytvo�en� z�znamu o procesu
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
	else {
	//	process_table[pid]->thread = std::this_thread;
	}

	return S_OK;
}


void runProcess(kiv_os::TEntry_Point func, int pid, char* arg, bool stdinIsConsole) {
	//uspani kvuli ulozeni threadu do PCB tabulky
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	kiv_os::TRegisters regs;
	kiv_os::TProcess_Startup_Info process_info;
	process_info.arg = arg;
	process_info.std_in = process_table[pid]->descriptors.at(0);
	process_info.std_out = process_table[pid]->descriptors.at(1);
	process_info.std_err = process_table[pid]->descriptors.at(2);

	//ulozeni hodnot do registru
	regs.rdx.r = (decltype(regs.rdx.r))process_table[pid]->name;
	regs.rcx.r = (decltype(regs.rax.r))pid;
	regs.rax.r = (decltype(regs.rax.r))&process_info;

	//spusteni procesu
	size_t ret = func(regs);
	{
		std::lock_guard<std::mutex> lock(pcb_mutex);
		process_table[pid]->thread.detach();
		//smazani zaznamu o procesu
		delete process_table[pid];
		process_table[pid] = nullptr;
	}
	
}

HRESULT joinProcess(int pid) {
	std::thread::id tid = process_table[pid]->thread.get_id();
	//ukonceni procesu
	{
		std::lock_guard<std::mutex> lock(pcb_mutex);
		process_table[pid]->thread.detach();
		//smazani zaznamu o procesu
		delete process_table[pid];
		process_table[pid] = nullptr;
	}
	return S_OK;
}

void Get_PCB(kiv_os::TRegisters &regs) {
	regs.rdx.r = (decltype(regs.rdx.r))process_table;
	int pids[PCB_SIZE];
	int proc_count = 0;
	for (int i = 0; i < PCB_SIZE; i++) {
		if (process_table[i] != nullptr) {
			pids[proc_count] = i;
			proc_count++;
			printf("%d\n", i);
		}
	}

	pids[proc_count] = -1;

	regs.rcx.r = (decltype(regs.rcx.r))sizeof(PCB);
	regs.rdi.r = (decltype(regs.rdi.r))pids;



}