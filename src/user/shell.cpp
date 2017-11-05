
#include "shell.h"
#include <string>
#include <iostream>
#include "rtl.h"

size_t __stdcall shell(const kiv_os::TRegisters &regs) {
	kiv_os::THandle std_out = kiv_os_rtl::Create_File("CONOUT$", /*FILE_SHARE_WRITE*/2);	//nahradte systemovym resenim, zatim viz Console u CreateFile na MSDN
	//kiv_os::THandle std_in = kiv_os_rtl::Create_File("CONIN$", 1);
	
	kiv_os::THandle std_in = 0;

	std::string output = "Boot successful\n";
	char * output_ = (char *)output.c_str();
	size_t written;
	kiv_os_rtl::Write_File(std_out, output_, std::strlen(output_), written);

	char * buf_command = new char[1001];
	buf_command[1000] = '\0';
	bool run = true;
	std::string input;

	while (std::getline(std::cin, input)) {
		
		std::string cur_path = "\nC:\>";
		char * cur_path_ = (char *)cur_path.c_str();

		kiv_os_rtl::Write_File(std_out, cur_path_, std::strlen(cur_path_), written);
		char *input_ = (char *)input.c_str();
		if (std::strcmp(input_, "ps") == 0) {
			ps(regs);
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