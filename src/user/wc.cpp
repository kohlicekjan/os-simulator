#include "../api/api.h"
#include "wc.h"
#include "rtl.h"
#include "parser.h"

size_t __stdcall wc(const kiv_os::TRegisters &regs) {
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);

	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->stdin;
	kiv_os::THandle std_out = process_info->stdout;
	kiv_os::THandle std_err = process_info->stderr;

	char args[3][1025];
	int argc, real_argc;
	char *buffer = new char[100];
	size_t written = 0;

	int lines = 1;
	int words = 1;
	int chars = 0;
	char *pom = new char[10];
	kiv_os::THandle file;

	parse_args(args, &argc, arg, str_len(arg));
	real_argc = argc;
	if (args[argc - 1][0] == 0) {
		real_argc = argc - 1;
	}

	//kdyz je na konci pipy
	if (argc == 1)argc = 2;
	//z pipy
	if (input_cmp(args[real_argc - 1], str_len(args[real_argc - 1]), "wc", 2) ||
		input_cmp(args[real_argc - 1], str_len(args[real_argc - 1]), "-l", 2) ||
		input_cmp(args[real_argc - 1], str_len(args[real_argc - 1]), "-w", 2) ||
		input_cmp(args[real_argc - 1], str_len(args[real_argc - 1]), "-c", 2)) {
		
		file = std_in;
	}
	else {
		//zkusi otevrit soubor
		file = kiv_os_rtl::Create_File(args[argc - 1], 5);
	}

	//vypis error
	if (file == kiv_os::erInvalid_Handle) {
		str_cpy(buffer, "File '", str_len("File '"));
		str_cat(buffer, args[argc - 1]);
		str_cat(buffer, "' doesn't exist!\n");
		kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
	}
	else {
		//vypis file
		while (true) {
			kiv_os_rtl::Read_File(file, buffer, 100, &written);
			chars += (int)written;
			for (int i = 0; i < written; i++) {
				switch (buffer[i]) {
				case '\n': lines++;
							words++;
					break;
				case ' ': words++;
					break;
				default: break;
				}
			}
			if (written == 0) {			
				break;
			}
		}
		buffer = new char[100];
		buffer[0] = '\0';

		if (argc == 3 && input_cmp(args[1], 2, "-l", 2)) {
			str_cat(buffer, "Lines: ");
			str_cat(buffer, atoi(lines, pom));
			str_cat(buffer, "\n");
		}
		else if (argc == 3 && input_cmp(args[1], 2, "-w", 2)) {
			str_cat(buffer, "Words: ");
			str_cat(buffer, atoi(words, pom));
			str_cat(buffer, "\n");
		}
		else if (argc == 3 && input_cmp(args[1], 2, "-c", 2)) {
			str_cat(buffer, "Chars: ");
			str_cat(buffer, atoi(chars, pom));
			str_cat(buffer, "\n");
		}
		else if (argc == 2) {
			str_cat(buffer, "Lines: ");
			str_cat(buffer, atoi(lines, pom));
			str_cat(buffer, "\tWords: ");
			str_cat(buffer, atoi(words, pom));
			str_cat(buffer, "\tChars: ");
			str_cat(buffer, atoi(chars, pom));
			str_cat(buffer, "\n");
		}
		else {
			str_cat(buffer, "Invalid argument!\n");
		}

		kiv_os_rtl::Write_File(std_out, buffer, str_len(buffer), written);
	}

	return 0;
}