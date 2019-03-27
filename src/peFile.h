#pragma once


ULONGLONG peFile_load(const char* name);
int peFile_ofsToRva(DWORD ofs);
int peFile_rvaToOffs(DWORD rva);
int peFile_rvaToSect(DWORD rva);
int peFile_sect(char* buff, DWORD i);

int peFile_addrToRva(ULONGLONG addr);
ULONGLONG peFile_rvaToAddr(int rva);
