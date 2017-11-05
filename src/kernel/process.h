#pragma once
#include "..\api\api.h"

#include <string>
#include <Windows.h>
#include <mutex>

const int PCB_SIZE = 256;

typedef struct process_control_block {
	int pid;
	char *name;

}PCB;

//name - jmeno procesu, par_pid - id rodice
HRESULT createProcess(char *name, int *par_pid);

//tabulka procesu
extern PCB *process_table[PCB_SIZE];