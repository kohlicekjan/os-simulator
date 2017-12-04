#include "../api/api.h"
#include "freq.h"
#include "rtl.h"
#include "parser.h"

size_t __stdcall freq(const kiv_os::TRegisters &regs) {
	//systemove volani do jadra
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->std_in;
	kiv_os::THandle std_out = process_info->std_out;
	kiv_os::THandle std_err = process_info->std_err;

	int byte_freq[256];
	char input[1025];
	char buffer[256];
	char pom[10];
	size_t written = 0;

	for (int i = 0; i < 256; i++) {
		byte_freq[i] = 0;
	}

	while (true) {
		kiv_os_rtl::Read_File(std_in, input, 1025, &written);
		if (input[0] == 26) {
			break;
		}
		for (int i = 0; i < str_len(input); i++) {
			byte_freq[input[i]]++;
		}
	}
	char hex[2];
	for (int i = 0; i < 256; i++) {
		buffer[0] = '\0';	
		if (byte_freq[i] > 0) {
			dec_To_Hex(i, hex);
			str_cat(buffer, "0x");
			str_cat(buffer, hex);
			str_cat(buffer, ": ");
			str_cat(buffer, atoi(byte_freq[i], pom));
			str_cat(buffer, "\n");
			kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
		}
	}

	return 0;
}

void dec_To_Hex(int decimal, char hex[]) {
	switch (decimal / 16) {
	case 10: hex[0] = 'A'; break;
	case 11: hex[0] = 'B'; break;
	case 12: hex[0] = 'C'; break;
	case 13: hex[0] = 'D'; break;
	case 14: hex[0] = 'E'; break;
	case 15: hex[0] = 'F'; break;
	default: hex[0] = (char)(decimal / 16 + 48);
	}
	switch (decimal % 16) {
	case 10: hex[1] = 'A'; break;
	case 11: hex[1] = 'B'; break;
	case 12: hex[1] = 'C'; break;
	case 13: hex[1] = 'D'; break;
	case 14: hex[1] = 'E'; break;
	case 15: hex[1] = 'F'; break;
	default: hex[1] = (char)(decimal % 16 + 48);
	}
}