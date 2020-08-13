#include "pch.h"
#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess) {
	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, dst, src, size, 0);
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
}
void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess) {
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}
uintptr_t mem::FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}
void mem::Nop(BYTE* dst, unsigned int size) {
	DWORD oldProtect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtect, &oldProtect);
}
uintptr_t mem::FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		addr = *(uintptr_t*)addr;
		addr += offsets[i];
	}
	return addr;
}

bool mem::Detour32(BYTE* src, BYTE* dst, const uintptr_t len, uintptr_t& addr) {
	DWORD oldProtect{};
	if (len < 5) {
		return false;
	}
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oldProtect);
	uintptr_t relativeAddr{ (uintptr_t)(dst - src - 5) };
	addr = relativeAddr;
	//memcpy(dst, (BYTE*)0x90, len);
	*src = 0xE9;
	*(uintptr_t*)(src + 1) = relativeAddr;
	VirtualProtect(src, len, oldProtect, &oldProtect);
	return true;
}

BYTE* mem::TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len, uintptr_t& addr) {
	if (len < 5) {
		return 0;
	}
	BYTE* gateway{ (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) };

	memcpy_s(gateway, len, src, len);
	uintptr_t gatewayRelativeAddr{ (uintptr_t)(src - gateway - 5) };

	*(gateway + len) = 0xE9;

	*(uintptr_t*)(gateway + len + 1) = gatewayRelativeAddr;

	Detour32(src, dst, len, addr);

	return gateway;
}