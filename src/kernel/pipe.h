#pragma once
#include "..\api\api.h"

const int BUFFER_SIZE = 1000;

typedef struct pipe {
	const char *name;										//jmeno pipy
	kiv_os::THandle in;										//deskriptory na vstup/vystup do pipy
	kiv_os::THandle out;

}pipe;

extern char buffer[BUFFER_SIZE];