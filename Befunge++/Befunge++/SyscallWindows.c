#include "SyscallWindows.h"
#include <stdio.h>
#include <Windows.h>


PIMAGE_SECTION_HEADER SectionByRVA(PIMAGE_SECTION_HEADER pSections, DWORD dwSections, DWORD rva)
{
	PIMAGE_SECTION_HEADER pSectionHeader = pSections;
	DWORD i;
	for (i = 0; i < dwSections; i++, pSectionHeader++)
	{
		// Is the RVA within this section?
		if (IS_ADDRESS_BETWEEN(pSectionHeader->VirtualAddress, (pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData), rva))
			return pSectionHeader;
	}
	return 0;
}
DWORD RawOffsetByRVA(PIMAGE_SECTION_HEADER pSections, DWORD dwSections, DWORD dwFileSize, DWORD rva)
{
	PIMAGE_SECTION_HEADER pSectionHeader;
	DWORD dwOffset, dwDelta;
	pSectionHeader = SectionByRVA(pSections, dwSections, rva);
	if (!pSectionHeader)
	{
		return 0;
	}
	dwDelta = rva - pSectionHeader->VirtualAddress;
	dwOffset = pSectionHeader->PointerToRawData + dwDelta;
	if (dwOffset >= dwFileSize)
		return 0;
	else
	{
		return dwOffset;
	}
}
#define GET_POINTER(RVA) ( pBuffer + RawOffsetByRVA( Sections, dwSections, dwFileSize, (RVA) ) )


void GetAndShowWindowsSyscallTable()
{
	HANDLE hFile = INVALID_HANDLE_VALUE,
		hMap = NULL;
	PBYTE pBuffer = NULL, pOps = NULL;
	DWORD dwFileSize = 0,
		dwSizeOfHeaders = 0,
		dwSections = 0,
		dwBaseAddress = 0,
		dwImageSize = 0,
		dwExportRVA = 0,
		dwExportSize = 0,
		dwExportRaw = 0,
		dwExports = 0;
	PDWORD pdwFunctions, pszFunctionNames;
	PWORD  pwOrdinals;
	PIMAGE_NT_HEADERS     NTHeader;
	PIMAGE_DOS_HEADER     DOSHeader;
	PIMAGE_SECTION_HEADER Sections;
	PIMAGE_EXPORT_DIRECTORY pExportDirectory;
	hFile = CreateFile
	(
		"c:\\windows\\system32\\ntdll.dll",
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Could not open file: %08X\n", GetLastError());
		goto done;
	}
	dwFileSize = GetFileSize(hFile, NULL);
	hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap == NULL)
	{
		fprintf(stderr, "Could not create memory map: %08X\n", GetLastError());
		goto done;
	}
	pBuffer = (PBYTE)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (hMap == NULL)
	{
		fprintf(stderr, "Could not obtain memory map view: %08X\n", GetLastError());
		goto done;
	}
	if (pBuffer[0] != 'M' || pBuffer[1] != 'Z')
	{
		fprintf(stderr, "Unexpected file header.\n");
		goto done;
	}
	// start reading PE headers
	DOSHeader = (PIMAGE_DOS_HEADER)pBuffer;
	NTHeader = (PIMAGE_NT_HEADERS)(pBuffer + DOSHeader->e_lfanew);
	dwSizeOfHeaders = NTHeader->OptionalHeader.SizeOfHeaders;
	dwBaseAddress = NTHeader->OptionalHeader.ImageBase;
	dwImageSize = NTHeader->OptionalHeader.SizeOfImage;
	dwSections = NTHeader->FileHeader.NumberOfSections;
	// get first section header
	Sections = (PIMAGE_SECTION_HEADER)
		(
			pBuffer +
			DOSHeader->e_lfanew +
			sizeof(IMAGE_NT_HEADERS)
			);
	// now parse the export directory
	dwExportRVA = NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	dwExportSize = NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	dwExportRaw = RawOffsetByRVA(Sections, dwSections, dwFileSize, dwExportRVA);
	if (!dwExportRVA || !dwExportSize || !dwExportRaw)
	{
		fprintf(stderr, "Unexpected export directory structure.\n");
		goto done;
	}
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pBuffer + dwExportRaw);
	pdwFunctions = (PDWORD)GET_POINTER(pExportDirectory->AddressOfFunctions);
	pwOrdinals = (PWORD)GET_POINTER(pExportDirectory->AddressOfNameOrdinals);
	pszFunctionNames = (PDWORD)GET_POINTER(pExportDirectory->AddressOfNames);
	dwExports = pExportDirectory->NumberOfNames;
	printf("SYSCALL   RVA       NAME\n");
	printf("-----------------------------------------------\n");
	// loop each exported symbol by name
	for (DWORD i = 0; i < pExportDirectory->NumberOfNames; ++i)
	{
		DWORD dwNameRVA = pszFunctionNames[i],
			dwApiRVA = pdwFunctions[pwOrdinals[i]],
			dwSyscall = 0,
			dwApiRaw = RawOffsetByRVA(Sections, dwSections, dwFileSize, dwApiRVA),
			dwNameRaw = RawOffsetByRVA(Sections, dwSections, dwFileSize, dwNameRVA);
		pOps = pBuffer + dwApiRaw;
		/*
		* Check if the API entry begins with:
		*
		*	MOV  EAX, IMM32
		*  XOR  ECX, ECX
		*  LEA  EDX, [ESP+04h]
		*  CALL FS:[C0h]
		*
		* Or
		*
		*	MOV  EAX, IMM32
		*  MOV  ECX, IMM32
		*  LEA  EDX, [ESP+04h]
		*  CALL FS:[C0h]
		*/
		if (pOps[0] == 0xB8 &&									            // mov eax, imm32
			(
			(
				pOps[5] == 0x33 && pOps[6] == 0xC9 &&		            // xor ecx, ecx
				!memcmp(&pOps[7], "\x8D\x54\x24\x04", 4) &&          // lea edx, [esp+04h]
				!memcmp(&pOps[11], "\x64\xFF\x15\xC0\x00\x00\x00", 7) // call fs:[C0h]
				)
				||
				(
					pOps[5] == 0xB9 &&								        // mov ecx, imm32
					!memcmp(&pOps[10], "\x8D\x54\x24\x04", 4) &&          // lea edx, [esp+04h]
					!memcmp(&pOps[13], "\x64\xFF\x15\xC0\x00\x00\x00", 7) // call fs:[C0h]
					)
				)
			)
		{
			/*
			* Extract the IMM32 part, this is our syscall number.
			*/
			dwSyscall = *(DWORD *)(pOps + 1);
			printf("%08X  %08X  %s\n", dwSyscall, dwBaseAddress + dwApiRVA, pBuffer + dwNameRaw);
		}
	}
done:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	if (pBuffer != NULL)
	{
		UnmapViewOfFile(pBuffer);
	}
	if (hMap != NULL)
	{
		CloseHandle(hMap);
	}
	return;
}