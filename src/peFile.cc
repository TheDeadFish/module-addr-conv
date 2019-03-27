#include <stdio.h>
#include <windows.h>
#include <imagehlp.h>

static PLOADED_IMAGE image;
static ULONGLONG image_base;
static DWORD image_size;
static DWORD image_hdrsz;

template <class T>
void peFile_init(T* ioh) {
	image_base = ioh->ImageBase;
	image_size = ioh->SizeOfImage;
	image_hdrsz = ioh->SizeOfHeaders;
}

ULONGLONG peFile_load(const char* name)
{
	ImageUnload(image);
	image = ImageLoad(name, NULL);
	if(!image) return image_base = 0;
	
	// get image base
	auto ioh = &image->FileHeader->OptionalHeader;
	if(ioh->Magic == 0x20B) {
		peFile_init((PIMAGE_OPTIONAL_HEADER64)ioh); }
	else { peFile_init((PIMAGE_OPTIONAL_HEADER32)ioh); }
	return image_base;
}

int peFile_ofsToRva(DWORD ofs)
{
	if(image == NULL) return -1;
	if(ofs < image_hdrsz) return ofs;
	for(int i = 0; i < image->NumberOfSections; i++) {
		auto& sect = image->Sections[i];
		DWORD tmp = ofs-sect.PointerToRawData;
		if(tmp < sect.SizeOfRawData) {
			return tmp+sect.VirtualAddress; }
	}
	return -1;
}

int peFile_rvaToOffs(DWORD rva)
{
	if(image == NULL) return -1;
	if(rva < image_hdrsz) return rva;
	auto sect = ImageRvaToSection(image->FileHeader, 0, rva);
	if(sect == NULL) return -1;
	return sect->PointerToRawData+ (rva-sect->VirtualAddress);
}

int peFile_rvaToSect(DWORD rva)
{
	if(image == NULL) return -1;
	auto sect = ImageRvaToSection(image->FileHeader, 0, rva);
	if(sect == NULL) return -1;
	return sect - image->Sections;
}

int peFile_sect(char* buff, DWORD i)
{
	if(image == NULL) return -1;
	if(i >= image->NumberOfSections) return 0;
	auto& sect = image->Sections[i];
	return sprintf(buff, "%-8.8s %08X, %08X", sect.Name,
		sect.VirtualAddress, sect.Misc.VirtualSize);
}

int peFile_addrToRva(ULONGLONG addr)
{
	if(image == NULL) return -1;
	addr -= image_base;
	if(addr >= image_size) return -1;
	return addr;
}

ULONGLONG peFile_rvaToAddr(int rva)
{
	if(image == NULL) return -1;
	if(rva > image_size) return -1;
	return image_base + rva;
}
