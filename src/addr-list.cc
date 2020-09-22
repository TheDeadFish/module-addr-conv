#include <stdshit.h>
#include "addr-list.h"

void toLower(char* str) {
	for(;*str; str++) {
		*str = toLower(*str); }
}

void AddrList::free()
{
	files.Clear();
}

int AddrList::load(cch* file)
{
	this->free();
	FILE* fp = xfopen(file, "r");
	if(!fp) return errno;
	
	char buff[3968];
	
	while(fgets(buff, 3968, fp)) {
		char* name = strtok(buff, ",");
		char* str = strtok(NULL, ",");
		u32 base = strtoull(str, &str, 16);
		files.push_back(xstrdup(name), base);
	}
	
	int err = ferror(fp);
	fclose(fp); return err;
}

int AddrList::FileInfo::match_(cstr name)
{
	if(!strcmp(this->name, name.data)) return 1; 
	if(getName2(this->name).istr(name)) return -1;
	return 0;
}

u64 AddrList::lookup(cch* name_)
{
	cstr name = getName(name_);
	u64 close_match = 0;
	
	for(auto& file : files) {
		int x = file.match_(name);
		if(x) { close_match = file.base;
			if(x > 0) break; }
	}

	return close_match;
}
