#pragma once


void input_parser(char *input, int input_size);

int str_len(char *input, int max_index = -1);

// teoreticky parsuje ��dek a d�v� to do pole - zatim omezena delka
void parse_args(char args[][1025], int *argc, char *input, int size);

//precise - je pokud chce� p�esnou delku
bool input_cmp(char *first, int first_size, char *second, int second_size, bool isPrecise = false);


