#pragma once
#include "..\api\api.h"

#include <Windows.h>
#include <mutex>
#include <vector>

const int PCB_SIZE = 256;

typedef struct process_control_block {
	char *name;
	int par_pid;
	std::thread thread;
	std::vector<kiv_os::THandle> descriptors;

}PCB;

void HandleProcess(kiv_os::TRegisters &regs);

//name - jmeno procesu, parent_pid - id rodice
HRESULT createProcess(char *name, int *parent_pid, kiv_os::TProcess_Startup_Info *arg);

void runProcess(kiv_os::TEntry_Point func, int pid, char *arg, bool stdinIsConsole);

//tabulka procesu
extern PCB *process_table[PCB_SIZE];