#include "../api/api.h"
#include "process.h"
#include "kernel.h"
#include "file_system.h"
#include "file_descriptor.h"
#include "handles.h"
#include "io.h"
#include <string.h>

#include <chrono>

std::mutex pcb_mutex;							// mutex pro zamèení PCB
PCB * process_table[PCB_SIZE];	//PCB max 256

void HandleProcess(kiv_os::TRegisters &regs) {
	switch (regs.rax.l){
		case kiv_os::scClone: {
			
			int pid = createProcess(reinterpret_cast<char *>(regs.rdx.r), reinterpret_cast<kiv_os::TProcess_Startup_Info*>(regs.rdi.r));
			if (pid != -1) {
				regs.rax.r = pid;
			}
			else {
				regs.rax.r = kiv_os::erInvalid_Argument;
				regs.flags.carry = 1;
			}

			break;
		}
		case kiv_os::scWait_For: {
			if (regs.rcx.h != 0) {
				Wait_For(regs.rcx.h, static_cast<kiv_os::THandle>(regs.rdx.r));
			}
			else {
				if (process_table[regs.rcx.l] == nullptr) {
					regs.rax.r = kiv_os::erInvalid_Handle;
					regs.flags.carry = 1;
					return;
				}
				Wait_For((int)regs.rdx.r);
			}
			
			break;
		}
	}
}

int createProcess(char *name, kiv_os::TProcess_Startup_Info *arg) {
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
	
	//kdyz neni misto pro dalsi proces, vrati -1
	if (pid == -1) {
		return -1;
	}

	for (int i = 0; i < PCB_SIZE; i++) {
		if(process_table[i] != nullptr){
			if (process_table[i]->thread_id == std::this_thread::get_id()) {
				parent_pid = i;
				break;
			}
		}
	}
	
	
	process_table[pid]->name = name;
	process_table[pid]->par_pid = parent_pid;	

	if(parent_pid >= 0 && process_table[pid] != NULL){
		process_table[pid]->path = process_table[parent_pid]->path;
	}
	else {
		char* path = (char *)get_root()->filePath.c_str();
		
		process_table[pid]->path = path;
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
		return -1;
	}

	// u prvniho shellu nechceme vytvaret dalsi vlakno - to prvni by skoncilo a zustal by sirotek :(
	if(pid != 0){
		process_table[pid]->thread = std::thread(runProcess, func, pid, arg->arg, false);
	}
	else {
		process_table[pid]->thread_id = std::this_thread::get_id();
	}

	return pid;
}


void runProcess(kiv_os::TEntry_Point func, int pid, char* arg, bool stdinIsConsole) {
	//uspani kvuli ulozeni threadu do PCB tabulky
	process_table[pid]->thread_id = std::this_thread::get_id();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	kiv_os::TRegisters regs;
	kiv_os::TProcess_Startup_Info process_info;
	process_info.arg = arg;
	process_info.std_in = process_table[pid]->descriptors.at(0);
	process_info.std_out = process_table[pid]->descriptors.at(1);
	process_info.std_err = process_table[pid]->descriptors.at(2);
	
	if (memcmp(arg, "ps", 2) == 0) {
		process_info.std_in = Get_PCB();
	}
	
	//ulozeni hodnot do registru
	regs.rcx.r = (decltype(regs.rcx.r))pid;
	regs.rdx.r = (decltype(regs.rdx.r))&process_info;

	//spusteni procesu
	size_t ret = func(regs);

	if (memcmp(arg, "shutdown", 8) == 0) {	
		exit(0);
	}

	
	{
		std::lock_guard<std::mutex> lock(pcb_mutex);
		if(process_table[pid] != nullptr){
			process_table[pid]->thread.detach();
			//smazani zaznamu o procesu
			delete process_table[pid];
			process_table[pid] = nullptr;
		}
		
	}
	
}

HRESULT joinProcess(int pid) {
	std::thread::id tid = process_table[pid]->thread_id;
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

void Wait_For(int pid, kiv_os::THandle std_in) {
	char buffer[100];
	kiv_os::TRegisters regs;
	regs.rdx.x = std_in;
	regs.rcx.r = 100;
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	while (process_table[pid] != nullptr) {
		Read_File(regs);
		if (buffer[0] == 26) {
			joinProcess(pid);
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Wait_For(int miliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
}

kiv_os::THandle Get_PCB() {
	char buffer[100];
	f_des* proc = open_file("C://system/proc/ps", false, WRITE);
	f_des* read = open_file("C://system/proc/ps", false, READ_UPDATE);
	int tid;
	for (int i = 0; i < PCB_SIZE; i++) {
		if (process_table[i] != nullptr) {
			memcpy(&tid, &process_table[i]->thread_id, sizeof(process_table[i]->thread_id));
			sprintf_s(buffer, 100, "%d\t %d\t %s \t\t %s\n", i, tid, process_table[i]->name, process_table[i]->path);
			write_file(proc, buffer, (char)strlen(buffer));
			
		}
	}
	return Convert_Native_Handle(read);
}