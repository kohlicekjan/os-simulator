#pragma once
#include "..\api\api.h"

#include <Windows.h>
#include <mutex>
#include <vector>

const int PCB_SIZE = 256;

//PCB tabulka pro ulozeni zaznamu o procesech
typedef struct process_control_block {
	char *name;										//jmeno procesu
	int par_pid;									//pid rodice --- bude potreba?
	std::thread thread;								//vlakno ve kterem se spusti proces - pro ziskani thread ID pouzit .get_id()
	std::vector<kiv_os::THandle> descriptors;		//descriptory pro in, out, err

}PCB;

// handle pro procesy (podobne jako HandleIO)
void HandleProcess(kiv_os::TRegisters &regs);

//name - jmeno procesu, parent_pid - id rodice, arg - odkaz na strukturu s argumenty pro proces
HRESULT createProcess(char *name, int *parent_pid, kiv_os::TProcess_Startup_Info *arg);

//spusteni procesu v novem vlakne
void runProcess(kiv_os::TEntry_Point func, int pid, char *arg, bool stdinIsConsole);

//tabulka procesu
extern PCB *process_table[PCB_SIZE];