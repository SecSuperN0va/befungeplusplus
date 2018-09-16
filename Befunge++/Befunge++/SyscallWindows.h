#pragma once
#include <Windows.h>

#define IS_ADDRESS_BETWEEN( left, right, address ) ( (address) >= (left) && (address) < (right) )


PIMAGE_SECTION_HEADER SectionByRVA(PIMAGE_SECTION_HEADER pSections, DWORD dwSections, DWORD rva);

DWORD RawOffsetByRVA(PIMAGE_SECTION_HEADER pSections, DWORD dwSections, DWORD dwFileSize, DWORD rva);

void GetAndShowWindowsSyscallTable();
