#include "process.h"
#include "kernel.h"
#include "io.h"

std::mutex pcb_mutex;							// mutex pro zamèení PCB
PCB * process_table[PCB_SIZE] = { nullptr };	//PCB max 256

HRESULT createProcess(char *name, int *proc_pid)
{
	int pid = -1;
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
	return S_OK;
}
