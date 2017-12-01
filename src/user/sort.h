#pragma once


extern "C" size_t __stdcall sort(const kiv_os::TRegisters &regs);

void sort_asc(char words[][1025], int num_words);