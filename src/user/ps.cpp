#include "../api/api.h"
#include "ps.h"
#include "rtl.h"


size_t __stdcall ps(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	if (kiv_os_rtl::Return_PCB(regs) == true) {
		//todo
	}
	
	return 0; 
}