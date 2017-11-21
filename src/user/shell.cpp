
#include "shell.h"
#include <iostream>
#include <string>
#include "rtl.h"
#include "parser.h"

size_t __stdcall shell(kiv_os::TRegisters &regs) {
	kiv_os::THandle std_out = kiv_os_rtl::Create_File("CONOUT$", /*FILE_SHARE_WRITE*/2);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	//kiv_os::THandle std_in = kiv_os_rtl::Create_File("CONIN$", 1);
	
	kiv_os::THandle std_in = 0;

	kiv_os::TProcess_Startup_Info process_info;
	
	char* output = "Boot successful\n";
	
	size_t written;
	kiv_os_rtl::Write_File(std_out, output, 16, written);

	char * buf_command = new char[1001];
	buf_command[1000] = '\0';
	bool run = true;
	std::string input;

	while (std::getline(std::cin, input)) {
		

		std::string cur_path = "\nC:\>";
		char * cur_path_ = (char *)cur_path.c_str();

		kiv_os_rtl::Write_File(std_out, cur_path_, std::strlen(cur_path_), written);
		char *input_ = (char *)input.c_str();
		int input_size = input.length();
		if (std::strcmp(input_, "ps") == 0) {	
			regs.rdx.r = (decltype(regs.rdx.r))input_;
			process_info.arg = "ps";
			process_info.std_in = std_in;
			process_info.std_out = std_out;
			process_info.std_err = 2;
			regs.rdi.r = (decltype(regs.rdi.r))&process_info;
			kiv_os_rtl::Create_Process(regs);
		}

		if (input_cmp(input_, input_size, "shutdown", 8)) {
			break;
		}
		
		/*
		size_t filled;
		kiv_os_rtl::Read_File(std_out, buf_command, 1000, &filled);
		if (filled == 0 && buf_command[filled] == EOF) { //goodbye
			break;
		}
		buf_command[filled] = '\0';
		*/
	}

	kiv_os_rtl::Close_File(std_out);
	return 0;
}

void parser(char *buffer) {
}