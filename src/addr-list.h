#pragma once

struct AddrList
{
	struct FileInfo {
		xstr name; u64 base;
		int match_(cstr name);
	};
	
	xArray<FileInfo> files;
	u64 lookup(cch* name);
	
	int load(cch* file);
	void free();
	
	
	~AddrList() { free(); }
};
