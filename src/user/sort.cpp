#include "../api/api.h"
#include "sort.h"
#include "rtl.h"
#include "parser.h"

#include <stdio.h>

size_t __stdcall sort(const kiv_os::TRegisters &regs) {
	kiv_os::TProcess_Startup_Info* process_info = reinterpret_cast<kiv_os::TProcess_Startup_Info *>(regs.rdx.r);
	
	char *arg = process_info->arg;

	kiv_os::THandle std_in = process_info->std_in;
	kiv_os::THandle std_out = process_info->std_out;
	kiv_os::THandle std_err = process_info->std_err;

	char command[1025];
	char args[100][1025];
	int argc = 0;
	char *buffer = new char[100];
	size_t written = 0;
	bool asc = true;
	kiv_os::THandle file = kiv_os::erInvalid_Handle;
	
	parse_args(args, &argc, arg, str_len(arg));
	if (args[argc - 1][0] == 0) {
		argc--;
	}
	//nechcem parsovat mezery
	parse_echo(command, arg, str_len(arg));
	if (input_cmp(command, 2, "-r", 2)) {
		asc = false;
		parse_echo(command, command, str_len(command));
	}
	
	if (argc >= 2 && asc == true) {
		file = kiv_os_rtl::Create_File(args[argc - 1], 5);
	}
	if (argc == 1 || (asc == false  && argc == 2)) {
		file = std_in;
	}
	
	
	if (file == kiv_os::erInvalid_Handle) {
		char words[100][1025];
		int count_word;
		parse_args(words, &count_word, command, str_len(command));

		if (asc) {
			sort_asc(words, count_word);
		}
		else {
			sort_desc(words, count_word);
		}
		
		for (int i = 0; i < count_word; i++) {
			kiv_os_rtl::Write_File(std_out, words[i], str_len(words[i]), written);
			kiv_os_rtl::Write_File(std_out, "\n", str_len("\n"), written);
		}
		
	}
	else if(file != kiv_os::erInvalid_Handle){
		
		//precte 100kB soubor
		char file_content[100000];
		//vypis file
		kiv_os_rtl::Read_File(file, file_content, 100000, &written);
		int num_lines = count_lines(file_content, (int)written);
		char** lines = new char*[num_lines];
		for (int i = 0; i < num_lines; i++) {
			lines[i] = new char[1025];
		}
		
		parse_lines(lines, file_content, (int)written);

		if (asc) {
			sort_asc(lines, num_lines);
		}
		else {
			sort_desc(lines, num_lines);
		}
		
		for (int i = 0; i < num_lines; i++) {
			kiv_os_rtl::Write_File(std_out, lines[i], str_len(lines[i]), written);
		}
	}
	
	return 0;
}

void sort_asc(char words[][1025], int num_words) {
	char pom[1025];
	int max_index;

	for (int i = 0; i < num_words - 1; i++) {
		max_index = i;
		for (int j = i + 1; j < num_words; j++) {
			if (words[j][0] < words[max_index][0]) {
				max_index = j;
			}
			else if (words[j][0] == words[max_index][0] && j != max_index) {
				for (int k = 1; k < 1025; k++) {
					if (words[j][k] < words[max_index][k]) {
						max_index = j;
						break;
					}
				}
			}
			
		}
		str_cpy(pom, words[i], str_len(words[i]));
		str_cpy(words[i], words[max_index], str_len(words[max_index]));
		str_cpy(words[max_index], pom, str_len(pom));
	}
}

void sort_asc(char** lines, int num_lines) {
	char pom[1025];
	int max_index;

	for (int i = 0; i < num_lines - 1; i++) {
		max_index = i;
		for (int j = i + 1; j < num_lines; j++) {
			if (lines[j][0] < lines[max_index][0]) {
				max_index = j;
			}
			else if (lines[j][0] == lines[max_index][0] && j != max_index) {
				for (int k = 1; k < 1025; k++) {
					if (lines[j][k] < lines[max_index][k]) {
						max_index = j;
						break;
					}
				}
			}

		}
		str_cpy(pom, lines[i], str_len(lines[i]));
		str_cpy(lines[i], lines[max_index], str_len(lines[max_index]));
		str_cpy(lines[max_index], pom, str_len(pom));
	}
}

void sort_desc(char words[][1025], int num_words) {
	char pom[1025];
	int max_index;

	for (int i = 0; i < num_words - 1; i++) {
		max_index = i;
		for (int j = i + 1; j < num_words; j++) {
			if (words[j][0] > words[max_index][0]) {
				max_index = j;
			}
			else if (words[j][0] == words[max_index][0] && j != max_index) {
				for (int k = 1; k < 1025; k++) {
					if (words[j][k] > words[max_index][k]) {
						max_index = j;
						break;
					}
				}
			}

		}
		str_cpy(pom, words[i], str_len(words[i]));
		str_cpy(words[i], words[max_index], str_len(words[max_index]));
		str_cpy(words[max_index], pom, str_len(pom));
	}
}

void sort_desc(char** lines, int num_lines) {
	char pom[1025];
	int max_index;

	for (int i = 0; i < num_lines - 1; i++) {
		max_index = i;
		for (int j = i + 1; j < num_lines; j++) {
			if (lines[j][0] > lines[max_index][0]) {
				max_index = j;
			}
			else if (lines[j][0] == lines[max_index][0] && j != max_index) {
				for (int k = 1; k < 1025; k++) {
					if (lines[j][k] > lines[max_index][k]) {
						max_index = j;
						break;
					}
				}
			}

		}
		str_cpy(pom, lines[i], str_len(lines[i]));
		str_cpy(lines[i], lines[max_index], str_len(lines[max_index]));
		str_cpy(lines[max_index], pom, str_len(pom));
	}
}