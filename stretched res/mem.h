#pragma once
#include "pch.h"
namespace mem
{
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
	uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
	uintptr_t FindDMAAddy(uintptr_t ptr, std::vector<unsigned int> offsets);

	bool Detour32(BYTE* src, BYTE* dst, const uintptr_t len, uintptr_t& addr);
	BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len, uintptr_t& addr);
};

