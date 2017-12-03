#include "rtl.h"

#include <atomic>

extern "C" __declspec(dllimport) void __stdcall Sys_Call(kiv_os::TRegisters &context);


std::atomic<size_t> Last_Error;

size_t Get_Last_Error() {
	return Last_Error;
}

kiv_os::TRegisters Prepare_SysCall_Context(uint8_t major, uint8_t minor) {
	kiv_os::TRegisters regs;
	regs.rax.h = major;
	regs.rax.l = minor;
	return regs;
}

bool Do_SysCall(kiv_os::TRegisters &regs) {
	Sys_Call(regs);

	if (regs.flags.carry) Last_Error = regs.rax.r;
		else Last_Error = kiv_os::erSuccess;

	return !regs.flags.carry;
}



kiv_os::THandle kiv_os_rtl::Create_File(const char* file_name, size_t flags, size_t isDir) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scCreate_File);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(file_name);
	
	regs.rcx.h = decltype(regs.rcx.h)(flags);
	regs.rcx.l = decltype(regs.rcx.l)(isDir);
	Do_SysCall(regs);
	return static_cast<kiv_os::THandle>(regs.rax.x);
}

bool kiv_os_rtl::Write_File(const kiv_os::THandle file_handle, const void *buffer, const size_t buffer_size, size_t &written) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scWrite_File);
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;	
	const bool result = Do_SysCall(regs);
	written = regs.rax.r;
	return result;
}

bool kiv_os_rtl::Read_File(const kiv_os::THandle file_handle, const void *buffer, const size_t buffer_size, size_t *filled) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scRead_File);
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;
	bool result = Do_SysCall(regs);
	*filled = (size_t)regs.rax.r;

	return result;
}

bool kiv_os_rtl::Delete_File(const char* file_name, size_t isDir) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scDelete_File);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(file_name);

	regs.rcx.l = decltype(regs.rcx.l)(isDir);
	Do_SysCall(regs);
	return static_cast<kiv_os::THandle>(regs.rax.x);
}

bool kiv_os_rtl::Set_File_Position(const kiv_os::THandle file_handle, size_t new_position) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scSet_File_Position);

	regs.rdx.r = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = new_position;

	return Do_SysCall(regs);
}

bool kiv_os_rtl::Get_File_Position(const kiv_os::THandle file_handle, const uint8_t position_type) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scGet_File_Position);

	regs.rdx.r = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rcx.h = position_type;

	//regs.rax.e == file position
	return Do_SysCall(regs);
}

bool kiv_os_rtl::Close_File(const kiv_os::THandle file_handle) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scClose_Handle);
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	return Do_SysCall(regs);
}

bool kiv_os_rtl::Get_Current_Directory(const char * buffer, const size_t buffer_size) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scGet_Current_Directory);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(buffer);
	regs.rcx.r = buffer_size;

	return Do_SysCall(regs);
}

bool kiv_os_rtl::Set_Current_Directory(const char * buffer) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scSet_Current_Directory);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(buffer);

	return Do_SysCall(regs);
}

kiv_os::THandle* kiv_os_rtl::Create_Pipe(kiv_os::THandle file_in, kiv_os::THandle file_out, int count) {
	kiv_os::TRegisters regs = Prepare_SysCall_Context(kiv_os::scIO, kiv_os::scCreate_Pipe);
	regs.rcx.x = static_cast<decltype(regs.rcx.x)>(file_in);
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_out);
	regs.rdi.r = count;
	Do_SysCall(regs);
	
	kiv_os::THandle pipe[2];
	pipe[0] = static_cast<kiv_os::THandle>(regs.rcx.x);
	pipe[1] = static_cast<kiv_os::THandle>(regs.rdx.x);

	return pipe;
}

bool kiv_os_rtl::Create_Process(kiv_os::TRegisters &regs) {
	char* name = reinterpret_cast<char *>(regs.rdx.r);
	uint64_t process_info = regs.rdi.r;
	regs = Prepare_SysCall_Context(kiv_os::scProc, kiv_os::scClone);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(name);
	regs.rdi.r = process_info;
	
	return Do_SysCall(regs);
}

bool kiv_os_rtl::Wait_For(kiv_os::TRegisters &regs) {
	size_t pid = regs.rax.r;
	size_t time = regs.rcx.r;
	size_t cur_pid = regs.rdi.r;
	kiv_os::THandle hnd = regs.rdx.r;
	regs = Prepare_SysCall_Context(kiv_os::scProc, kiv_os::scWait_For);
	regs.rcx.h = (decltype(regs.rcx.h))pid;
	if (pid == 0) {
		regs.rdx.r = time;
		regs.rcx.l = cur_pid;
	}
	else {
		regs.rdx.r = hnd;
	}
	return Do_SysCall(regs);
}