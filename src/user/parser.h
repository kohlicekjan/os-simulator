#pragma once
#include "../api/api.h"


void input_parser(char *input, int input_size, kiv_os::TRegisters &regs);
void do_command(char args[][1025], int argc);
void str_cpy(char *destination, char *string, int size_of_string);
int str_len(char *input, int max_index = -1);

// teoreticky parsuje ��dek a d�v� to do pole - zatim omezena delka
void parse_args(char args[][1025], int *argc, char *input, int size);

void parse_echo(char args[], char *input, int size);

//precise - je pokud chce� p�esnou delku
bool input_cmp(char *first, int first_size, char *second, int second_size, bool isPrecise = false);

char* atoi(int value, char *str);

//retezec str prida na konec retezce dest
char* str_cat(char* dest, char* str);

