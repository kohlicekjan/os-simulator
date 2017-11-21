#include "parser.h"

void input_parser(char *input, int input_size) {

	char args[256][1025];
	int argc;
	parse_args(args, &argc, input, input_size);
	if (input_cmp(input, input_size, "ps", 2)) {
		
	}
	else if (input_cmp(input, input_size, "shutdown", 8)) {

	}

}


//return length of string - max index - predchazeni nekonecne smycky 
int str_len(char *input, int max_index) {
	int i = 0;

	while (input[i] != '\0') {
		i++;

		if (i == max_index) {
			return i;
		}
	}

	return i;
}

// teoreticky parsuje øádek a dává to do pole - zatim omezena delka
void parse_args (char args[][1025], int *argc, char *input, int size) {
	int i, 
		arg_num = 0,
		act_index = 0;
	bool next_word = false;

	for (i = 0; i < size; i++) {
		if (input[i] == '\0') {
			break;
		}

		//pokud neni mezera nacitej dalsi znaky 
		//TODO: rozmyslet kde bude gramatika - zda se znaky ">" bez mezery budou brát take nebo ne 
		if (input[i] != ' ') {
			args[arg_num][act_index++] = input[i];
			next_word = false;
		}
		else {
			//zapocita se jen jedna mezera protoze pak je jiz next_word = true
			if (!next_word) {
				next_word = true;
				args[arg_num][act_index++] = '\0';
				arg_num++;
				act_index = 0;
			}
		}
	}

	argc = &arg_num;
}

//precise - je pokud chceš pøesnou delku - default value false
bool input_cmp(char *first, int first_size, char *second, int second_size, bool isPrecise) {
	int i;
	if (second_size > first_size) {
		return false;
	}

	if (isPrecise && second_size != first_size) {
		return false;
	}

	//zkousi zda jsou retezce stejne
	for (i = 0; i < second_size; i++) {
		if (first[i] != second[i]) {
			return false;
		}
	}

	return true;
}


