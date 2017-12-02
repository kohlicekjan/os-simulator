#pragma once
#include "..\api\api.h"

#include <queue>
#include <Windows.h>

/*
const int BUFFER_SIZE = 1000;

typedef struct pipe {
	const char *name;										//jmeno pipy
	kiv_os::THandle in;										//deskriptory na vstup/vystup do pipy
	kiv_os::THandle out;

}pipe;

extern char buffer[BUFFER_SIZE];
*/
static const int MAX_SIZE = 2048;
typedef struct pipe {

	bool closed_in;
	bool closed_out;
	std::queue<char> queue;
	CRITICAL_SECTION crit_sec;
	CONDITION_VARIABLE buffer_full, buffer_empty;
	
} pipe;


bool write(pipe *pipe, char c);
char read(pipe *pipe);

pipe *create_pipe();
bool write(pipe *pipe, char * s, size_t len, size_t * written);
bool read(pipe *pipe, size_t count, char *str, size_t * read);
bool peek(pipe *pipe, size_t * available);
void close_read(pipe *pipe);
void close_write(pipe *pipe);
void destroy(pipe *pipe);
