#include <stdshit.h>
#include <win32hlp.h>
#include "resource.h"
#include "peFile.h"

const char progName[] = "Module Address Converter";

static 
u64 getDlgItemHex64(HWND hwnd, int ctrlID)
{
	char buff[32]; char* end;
	GetDlgItemTextA(hwnd, ctrlID, buff, 32);
	return strtoull(buff, &end, 16);
}

static
void setDlgItemHex64(HWND hwnd, 
	int ctrlID, u64 data, int minLen)
{
	char buff[32];
	sprintf(buff, "%0*I64X", minLen, data);
	SetDlgItemTextA(hwnd, ctrlID, buff);
}

// loaded file into

void edt_init(HWND hwnd, u64 base)
{
	setDlgItemHex64(hwnd, IDC_MOD_BASE, base, 8);
	setDlgItemHex64(hwnd, IDC_LOD_BASE, base, 8);
	setDlgItemHex64(hwnd, IDC_MOD_ADDR, base, 8);
}

void mainDlgInit(HWND hwnd)
{
	HFONT hFont = (HFONT) GetStockObject(OEM_FIXED_FONT);
	sendDlgMsg(hwnd, IDC_LIST1,  WM_SETFONT, (WPARAM)hFont, TRUE);
	edt_init(hwnd, 0);
}

void load_module(HWND hwnd)
{
	// load the module file
	OpenFileName ofn;
	if(!ofn.doModal(hwnd)) return;
	auto base = peFile_load(ofn.lpstrFile);
	if(!base) { contError(hwnd, 
		"failed to load module"); return; }
		
	setDlgItemText(hwnd, IDC_EDIT1, ofn.lpstrFile);
		
		
	edt_init(hwnd, base);
	EnableDlgItem(hwnd, IDC_MOD_BASE, FALSE);
	EnableDlgItem(hwnd, IDC_RVA_ADDR, TRUE);
	EnableDlgItem(hwnd, IDC_MOD_OFFS, TRUE);

	// initialize list
	listBox_reset(hwnd, IDC_LIST1);
	char buff[64];
	for(int i = 0; peFile_sect(buff, i); i++)
		listBox_addStr(hwnd, IDC_LIST1, buff);
}

void edt_validate(HWND hwnd, int ctrlId)
{
	setDlgItemHex64(hwnd, ctrlId, 
		getDlgItemHex64(hwnd, ctrlId), 8);
}


// masked update
byte update_mask[4];
bool update_mask_chk(int ctrlId) {
	return update_mask[ctrlId-IDC_MOD_ADDR]; }
void update_mask_set(int ctrlId, int x) {
	update_mask[ctrlId-IDC_MOD_ADDR] = x; }


void edt_update(HWND hwnd, int ctrlId)
{
	if(is_one_of(ctrlId, IDC_MOD_BASE, IDC_LOD_BASE))
		ctrlId = IDC_MOD_ADDR;
	update_mask_set(ctrlId, 1);
	SCOPE_EXIT(update_mask_set(ctrlId, 0));
	
	ULONGLONG lod_diff = getDlgItemHex64(hwnd, IDC_LOD_BASE)
		- getDlgItemHex64(hwnd, IDC_MOD_BASE);
	u64 value = getDlgItemHex64(hwnd, ctrlId);
	
	
	switch(ctrlId) {
	case IDC_LOD_ADDR:
		if(!update_mask_chk(IDC_MOD_ADDR))
			setDlgItemHex64(hwnd, IDC_MOD_ADDR, value - lod_diff, 8);
		break;
		
	case IDC_MOD_ADDR:
		if(!update_mask_chk(IDC_LOD_ADDR))
			setDlgItemHex64(hwnd, IDC_LOD_ADDR, value + lod_diff, 8);
		if(!update_mask_chk(IDC_RVA_ADDR))
			setDlgItemHex64(hwnd, IDC_RVA_ADDR,	peFile_addrToRva(value), 8);
		break;
		
	case IDC_RVA_ADDR:
		if(!update_mask_chk(IDC_MOD_ADDR))
			setDlgItemHex64(hwnd, IDC_MOD_ADDR, peFile_rvaToAddr(value), 8);
		if(!update_mask_chk(IDC_MOD_OFFS))
			setDlgItemHex64(hwnd, IDC_MOD_OFFS, peFile_rvaToOffs(value), 8);
		break;
		
	case IDC_MOD_OFFS:
		value = peFile_ofsToRva(value);
		if(!update_mask_chk(IDC_RVA_ADDR))
			setDlgItemHex64(hwnd, IDC_RVA_ADDR, value, 8);
		listBox_setCurSel(hwnd, IDC_LIST1,
			peFile_rvaToSect(value));
		break;
	}
}


BOOL CALLBACK mainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DLGMSG_SWITCH(
	  CASE_COMMAND(
	    ON_COMMAND(IDCANCEL, EndDialog(hwnd, 0))
			ON_COMMAND(IDC_LOAD, load_module(hwnd))

			ON_CONTROL_RANGE(EN_CHANGE, IDC_MOD_BASE, IDC_RVA_ADDR,
				edt_update(hwnd, LOWORD(wParam)))
			ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_MOD_BASE, IDC_RVA_ADDR,
				edt_validate(hwnd, LOWORD(wParam)))
	  	  
	  ,)
		ON_MESSAGE(WM_INITDIALOG, mainDlgInit(hwnd))
	
	,)
}

int main()
{
	DialogBoxW(NULL, MAKEINTRESOURCEW(IDD_DIALOG1), NULL, mainDlgProc);
}

