#include "pipe.h"

#include <iostream>

pipe *create_pipe()
{
	pipe *new_pipe = new pipe();
	InitializeConditionVariable(&(new_pipe->buffer_full));
	InitializeConditionVariable(&(new_pipe->buffer_empty));
	InitializeCriticalSection(&(new_pipe->crit_sec));
	new_pipe->closed_in = false;
	new_pipe->closed_out = false;
	return new_pipe;
}

bool write(pipe *pipe, char * s, size_t len, size_t * written)
{
	*written = 0;
	for (int i = 0; i < len; i++) {
		if (!write(pipe, s[i])) {
			*written = i;
			return false;
		}
	}
	*written = len;
	return true;
}

bool write(pipe *pipe, char c)
{
	if (pipe->closed_out) {
		return false;
	}
	while (pipe->queue.size() >= MAX_SIZE && !pipe->closed_out) {
		SleepConditionVariableCS(&(pipe->buffer_full), &(pipe->crit_sec), INFINITE);
	}
	if (pipe->closed_out) {
		return false;
	}
	EnterCriticalSection(&(pipe->crit_sec));
	pipe->queue.push(c);
	LeaveCriticalSection(&(pipe->crit_sec));
	WakeConditionVariable(&(pipe->buffer_empty));
	return true;
}

bool read(pipe *pipe, size_t count, char *str, size_t *r)
{
	int pos = 0;
	while (pos < count) {
		char c = read(pipe);
		str[pos] = c;
		if (c == EOF) {
			break;
		}
		pos++;
	}
	*r = pos;
	return true;
}


bool peek(pipe *pipe, size_t * available)
{
	if (pipe->closed_in) {
		*available = 1; //EOF is available
	}
	else {
		*available = pipe->queue.size();
	}
	return true;
}


char read(pipe *pipe)
{
	if (pipe->closed_in && pipe->queue.size() < 1) {
		return EOF;
	}
	while (pipe->queue.size() < 1 && !pipe->closed_in) {
		SleepConditionVariableCS(&(pipe->buffer_empty), &(pipe->crit_sec), INFINITE);
	}
	if (pipe->closed_in && pipe->queue.size() < 1) {
		return EOF;
	}
	EnterCriticalSection(&(pipe->crit_sec));
	char ret = pipe->queue.front();
	pipe->queue.pop();
	LeaveCriticalSection(&(pipe->crit_sec));
	WakeConditionVariable(&(pipe->buffer_full));
	return ret;
}

void close_read(pipe *pipe)
{
	pipe->closed_out = true;
	WakeConditionVariable(&(pipe->buffer_full));
}

void close_write(pipe *pipe)
{
	pipe->closed_in = true;
	WakeConditionVariable(&(pipe->buffer_empty));
}

void destroy(pipe *pipe)
{
	DeleteCriticalSection(&(pipe->crit_sec));
	WakeAllConditionVariable(&(pipe->buffer_empty));
	WakeAllConditionVariable(&(pipe->buffer_full));
}