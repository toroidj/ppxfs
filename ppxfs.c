/*-----------------------------------------------------------------------------
	Paper Plane xUI Text file Module							(c)TORO
-----------------------------------------------------------------------------*/
#define STRICT
#define UNICODE
#include <windows.h>
#include "torowin.h"
#include "ppcommon.h"
#include "vfs.h"
#undef EXTDLL
#define EXTDLL

#define RELEASEVERSION L"R0"

int MakeDirList(PPXAPPINFOW *ppxa, DWORD cmdID, PPXMCOMMANDSTRUCT *command);
int SaveText(PPXAPPINFOW *ppxa, PPXMCOMMANDSTRUCT *command);

/*-----------------------------------------------------------------------------
	dll ‚Ì‰Šú‰»^I—¹ˆ—
-----------------------------------------------------------------------------*/
#pragma argsused
BOOL WINAPI DllEntry(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	return TRUE;
}

/*-----------------------------------------------------------------------------
	–{‘Ì
-----------------------------------------------------------------------------*/
#pragma argsused
int PPXAPI ModuleEntry(PPXAPPINFOW *ppxa, DWORD cmdID, PPXMODULEPARAM pxs)
{
	if ( cmdID == PPXMEVENT_FUNCTION ){
		// ====================================================================
		if ( (pxs.command->commandhash == 0xd37f94d6) && !wcscmp(pxs.command->commandname, L"MAKEDIRLIST") ){
			return MakeDirList(ppxa, cmdID, pxs.command);
		}
	} else if ( cmdID == PPXMEVENT_COMMAND ){
		if ( (pxs.command->commandhash == 0xd37f94d6) && !wcscmp(pxs.command->commandname, L"MAKEDIRLIST") ){
			return MakeDirList(ppxa, cmdID, pxs.command);
		}
		if ( (pxs.command->commandhash == 0xc5555e55) && !wcscmp(pxs.command->commandname, L"SAVETEXT") ){
			return SaveText(ppxa, pxs.command);
		}
	} else if ( cmdID == PPXM_INFORMATION ){
		if ( pxs.info->infotype == 0 ){
			pxs.info->typeflags = PPMTYPEFLAGS(PPXMEVENT_COMMAND) |
				PPMTYPEFLAGS(PPXMEVENT_FUNCTION);
			wcscpy(pxs.info->copyright, L"PPx Text file Module " RELEASEVERSION L"  Copyright (c)TORO");
			return PPXMRESULT_DONE;
		}
	}
	return PPXMRESULT_SKIP;
}


typedef struct {
	FN_REGEXP fn;
	ThSTRUCT th;
	TCHAR path[VFPS + MAX_PATH], *last;
	BOOL subdir;
} DIRSTRUCT;


void MakeDirDir(DIRSTRUCT *dir)
{
	WCHAR *last;
	HANDLE hFF;
	WIN32_FIND_DATA ff;

	last = dir->last + wcslen(dir->last);
	if ( (last - dir->path) >= VFPS ) return;
	CatPath(NULL, dir->path, L"*.*");
	if ( (*last == '\\') || (*last == '/') ) last++;
	hFF = FindFirstFileL(dir->path, &ff);
	*last = '\0';

	if ( hFF != INVALID_HANDLE_VALUE ) do{
		if ( IsRelativeDir(ff.cFileName) ) continue;
		if ( FinddataRegularExpression(&ff, &dir->fn) ){
			wcscpy(last, ff.cFileName);
			if ( (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && dir->subdir ){
				MakeDirDir(dir);
			}else{
				wcscat(last, L"\r\n");
				ThCatString(&dir->th, dir->path);
			}
		}
	}while( IsTrue(FindNextFile(hFF, &ff)) );
	FindClose(hFF);
	return;
}

void SaveTextMain(PPXAPPINFOW *ppxa, const WCHAR *filename, const WCHAR *text, int length, UINT codepage)
{
	int size;
	HANDLE hFile = CreateFileL(filename, GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile == INVALID_HANDLE_VALUE ){
		PPErrorBox(ppxa->hWnd, L"MakeDirList", PPERROR_GETLASTERROR);
		return;
	}

	size = length * sizeof(WCHAR);

	if ( (codepage == CP__UTF16L) || (codepage == VTYPE_UNICODE) ){
		if ( codepage == VTYPE_UNICODE ){ // BOM
			WriteFile(hFile, UCF2HEADER, UCF2HEADERSIZE, NULL, NULL);
		}
		WriteFile(hFile, text, size, NULL, NULL);
	}else{
		DWORD writesize;
		char *buf;

		if ( codepage == VTYPE_UTF8 ){ // BOM
			codepage = CP_UTF8;
			WriteFile(hFile, UTF8HEADER, UTF8HEADERSIZE, NULL, NULL);
		}
		writesize = WideCharToMultiByte(codepage, 0, text, length, NULL, 0, NULL, NULL);
		buf = HeapAlloc(GetProcessHeap(), 0, writesize);
		writesize = WideCharToMultiByte(codepage, 0, text, length, buf, writesize, NULL, NULL);
		WriteFile(hFile, buf, writesize, NULL, NULL);
		HeapFree(GetProcessHeap(), 0, buf);
	}
	CloseHandle(hFile);
}

BOOL CodepageOption(const WCHAR *param, UINT *codepage)
{
	if ( wcscmp(param, L"utf8") == 0 ){
		*codepage = CP_UTF8;
	}else if ( wcscmp(param, L"utf8bom") == 0 ){
		*codepage = VTYPE_UTF8;
	}else if ( wcscmp(param, L"system") == 0 ){
		*codepage = CP_ACP;
	}else if ( wcscmp(param, L"unicode") == 0 ){
		*codepage = CP__UTF16L;
	}else if ( wcscmp(param, L"unicodebom") == 0 ){
		*codepage = VTYPE_UNICODE;
	}else{
		return FALSE;
	}
	return TRUE;
}

int MakeDirList(PPXAPPINFOW *ppxa, DWORD cmdID, PPXMCOMMANDSTRUCT *command)
{
	DIRSTRUCT dir;
	const WCHAR *path, *mask, *param, *savepath = NULL;
	UINT codepage = VTYPE_UNICODE;

	ThInit(&dir.th);
	dir.subdir = TRUE;

	if ( command->paramcount >= 1 ){
		path = command->param;
		mask = path + wcslen(path) + 1;
	}else{
		path = L"";
	}
	if ( command->paramcount < 2 ) mask = L"";
	if ( command->paramcount >= 3 ){
		command->paramcount -= 2;
		param = mask;
		while ( command->paramcount ){
			param += wcslen(param) + 1;
			if ( wcscmp(param, L"subdir") == 0 ){
				dir.subdir = 1;
			}else if ( wcscmp(param, L"nosubdir") == 0 ){
				dir.subdir = 0;
			}else if ( CodepageOption(param, &codepage) == FALSE ){
				savepath = param;
			}
			command->paramcount--;
		}
	}

	VFSFullPath(dir.path, (WCHAR *)path, NULL);

	dir.last = dir.path + wcslen(dir.path);

	MakeFN_REGEXP(&dir.fn, mask);
	MakeDirDir(&dir);

	FreeFN_REGEXP(&dir.fn);

	if ( cmdID == PPXMEVENT_FUNCTION ){
		ppxa->Function(ppxa, PPXCMDID_LONG_RESULT, dir.th.bottom);
		command->resultstring[0] = '\0';
	}

	if ( (savepath != NULL) && (dir.th.top > 0) ){
		SaveTextMain(ppxa, savepath, (const WCHAR *)dir.th.bottom, dir.th.top / sizeof(WCHAR), codepage);
	}

	ThFree(&dir.th);
	return PPXMRESULT_DONE;
}

int SaveText(PPXAPPINFOW *ppxa, PPXMCOMMANDSTRUCT *command)
{
	const WCHAR *param, *savepath, *text;
	UINT codepage = VTYPE_UNICODE;

	if ( command->paramcount <= 1 ){
		return PPXMRESULT_DONE;
	}else{
		savepath = command->param;
		param = savepath;
		command->paramcount -= 1;
		while ( command->paramcount ){
			param += wcslen(param) + 1;
			if ( CodepageOption(param, &codepage) == FALSE ){
				text = param;
			}
			command->paramcount--;
		}
	}

	SaveTextMain(ppxa, savepath, text, wcslen(text), codepage);

	return PPXMRESULT_DONE;
}
