#include "edBank/edBankFiler.h"

#include "edBank/edBankFile.h"
#include "edFile/edFile.h"
#include "edFile/edFilePath.h"
#include "edStr.h"
#include "edFile/edFileNoWaitStack.h"

CFilerBank FilerBank;

edCFiler_Bnk_static BnkUnitTable[4];
edFILEH* edBankFilerCurrentDirtyFileHandle = NULL;

// Inlined
CFilerBank::CFilerBank()
{
	int iVar2;

	iVar2 = 0;
	do {
		bnkInUse[iVar2] = 0;
		memset(&field_0x218[iVar2], 0, sizeof(edCFiler_BNK_8));
		iVar2 = iVar2 + 1;
	} while (iVar2 < 4);

	return;
}

bool CFilerBank::isnowaitcmdend(edFILE_STACK_ELEMENT* pElement)
{
	edFILEH* pDebugBankData;
	edFILEH* pDVar1;
	edCFiler* peVar2;
	EBankAction EVar3;
	bool bSuccess;
	edCFiler_BNK_8* pCVar4;
	long lVar5;
	byte* pbVar6;
	int iVar7;

	pDebugBankData = reinterpret_cast<edFILEH*>(pElement->pData);
	pCVar4 = (edCFiler_BNK_8*)GetInternalData_0025b2e0(pDebugBankData);
	if (pCVar4 == (edCFiler_BNK_8*)0x0) {
		bSuccess = false;
	}
	else {
		pDVar1 = pCVar4->pDebugBankData;
		peVar2 = pDVar1->pOwningFiler;
		pDVar1->action = pElement->action;
		EVar3 = pElement->action;
		if (((EVar3 == EBankAction::WRITE) || (EVar3 == EBankAction::READ)) || (bSuccess = true, EVar3 == EBankAction::SEEK)) {
			pElement->pData = pDVar1;
			lVar5 = peVar2->isnowaitcmdend(pElement);
			if (lVar5 == 0) {
				pElement->pData = pDebugBankData;
				bSuccess = false;
			}
			else {
				pElement->pData = pDebugBankData;
				bSuccess = true;
			}
		}
		else {
			if (EVar3 == EBankAction::CLOSE) {
				pElement->pData = pDVar1;
				lVar5 = peVar2->isnowaitcmdend(pElement);
				if (lVar5 == 0) {
					pElement->pData = pDebugBankData;
					bSuccess = false;
				}
				else {
					pElement->pData = pDebugBankData;
					iVar7 = 0;
					do {
						if (&field_0x218[iVar7] == pCVar4) {
							this->bnkInUse[iVar7] = 0;
							memset(&field_0x218[iVar7], 0, sizeof(edCFiler_BNK_8));
							break;
						}
						iVar7 = iVar7 + 1;
					} while (iVar7 < 4);
					iVar7 = 0;
					do {
						if (&edFileHandleData[iVar7] == pDVar1) {
							edFileHandleTable[iVar7] = 0;
							memset(&edFileHandleData[iVar7], 0, sizeof(edFILEH));
							break;
						}
						iVar7 = iVar7 + 1;
					} while (iVar7 < 0x10);
					bSuccess = true;
				}
			}
		}
	}

	return bSuccess;
}

bool CFilerBank::seek(edFILEH* pDebugBank)
{
	edFILEH* pData;
	bool uVar1;
	edCFiler_BNK_8* ppiVar2;

	ppiVar2 = (edCFiler_BNK_8*)GetInternalData_0025b2e0(pDebugBank);
	if (ppiVar2 == (edCFiler_BNK_8*)0x0) {
		uVar1 = false;
	}
	else {
		if (*(uint*)((int)ppiVar2->pFileData + 4) < (uint)pDebugBank->seekOffset) {
			uVar1 = false;
		}
		else {
			pData = ppiVar2->pDebugBankData;
			pData->seekOffset = *(int*)ppiVar2->pFileData + pDebugBank->seekOffset;
			uVar1 = pData->pOwningFiler->seek(pData);
		}
	}
	return uVar1;
}

uint CFilerBank::read(edFILEH* pFile, void* pDst, uint requiredSize)
{
	edCFiler* peVar1;
	edCFiler_BNK_8* ppiVar2;
	uint uVar2;

	ppiVar2 = (edCFiler_BNK_8*)GetInternalData_0025b2e0(pFile);
	if (ppiVar2 == (edCFiler_BNK_8*)0x0) {
		uVar2 = 0;
	}
	else {
		if (requiredSize == (pFile->fileInfo).fileSize) {
			peVar1 = ppiVar2->pDebugBankData->pOwningFiler;
			uVar2 = peVar1->read(ppiVar2->pDebugBankData, pDst, requiredSize);
		}
		else {
			uVar2 = 0;
		}
	}
	return uVar2;
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBankFiler.cpp
bool CFilerBank::close(edFILEH* pDebugBank)
{
	edCFiler* peVar1;
	edCFiler_BNK_8* ppiVar1;

	MY_LOG("edCFiler_BNK::close\n");

	ppiVar1 = (edCFiler_BNK_8*)GetInternalData_0025b2e0(pDebugBank);
	if (ppiVar1 != (edCFiler_BNK_8*)0x0) {
		peVar1 = ppiVar1->pDebugBankData->pOwningFiler;
		peVar1->close(ppiVar1->pDebugBankData);
	}
	return ppiVar1 != (edCFiler_BNK_8*)0x0;
}

bool CFilerBank::open(edFILEH* pOutData, char* filePath)
{
	bool bVar1;
	int iVar2;
	char* pcVar3;
	int iVar4;
	edFILEH* pDVar5;
	FileHeaderFileData* puVar6;
	ulong uVar7;
	byte* pbVar8;
	edCFiler_Bnk_static* pAlreadyLoadedData;
	edCFiler_BNK_8* bufferStart;
	char acStack1024[512];
	char acStack512[512];

	MY_LOG("edCFiler_BNK::open {}\n", filePath);

	iVar4 = 0;
	do {
		if (bnkInUse[iVar4] == 0) {
			bnkInUse[iVar4] = 1;
			bufferStart = field_0x218 + iVar4;
			memset(bufferStart, 0, sizeof(edCFiler_BNK_8));
			break;
		}
		iVar4 = iVar4 + 1;
		bufferStart = (edCFiler_BNK_8*)0x0;
	} while (iVar4 < 4);
	bVar1 = false;
	if (bufferStart != (edCFiler_BNK_8*)0x0) {
		edFilePathSplit((char*)0x0, acStack512, (char*)0x0, (char*)0x0, filePath);
		iVar4 = 0;
		/* Look to see if we already loaded the file. */
		pAlreadyLoadedData = BnkUnitTable;
		do {
			iVar2 = edStrICmp(pAlreadyLoadedData->path, acStack512);
			if (iVar2 == 0) goto LAB_00247280;
			iVar4 = iVar4 + 1;
			pAlreadyLoadedData = pAlreadyLoadedData + 1;
		} while (iVar4 < 4);
		pAlreadyLoadedData = (edCFiler_Bnk_static*)0x0;
	LAB_00247280:
		bVar1 = false;
		if (pAlreadyLoadedData != (edCFiler_Bnk_static*)0x0) {
			pcVar3 = edStrChr(filePath, '>');
			pcVar3 = edStrChr(pcVar3 + 1, ':');
			edStrCopy(acStack1024, pcVar3 + 1);
			edCFiler_Bnk_static_header* pStaticFileHeader = pAlreadyLoadedData->pFileData;
			/* Look for the index of the file we want. */
			iVar4 = pStaticFileHeader->fileHeader.get_entryindex_from_filename(acStack1024);
			if (iVar4 == -1) {
				MY_LOG("edCFiler_BNK::open ERROR could not find file index {}\n", acStack1024);
				bVar1 = false;
			}
			else {
				/* Okay we found the index, load the file. */
				pDVar5 = edFileOpen(pAlreadyLoadedData->diskPath, 1);
				iVar2 = 0;
				if (pDVar5 == (edFILEH*)0x0) {
					pbVar8 = bnkInUse;
					do {
						if ((edCFiler_BNK_8*)(pbVar8 + 4) == bufferStart) {
							bnkInUse[iVar2] = 0;
							memset(field_0x218 + iVar2, 0, sizeof(edCFiler_BNK_8));
							break;
						}
						iVar2 = iVar2 + 1;
						pbVar8 = pbVar8 + 8;
					} while (iVar2 < 4);
					bVar1 = false;
				}
				else {
					bufferStart->pDebugBankData = pDVar5;
					edBankFilerCurrentDirtyFileHandle = pDVar5;
					puVar6 = pStaticFileHeader->fileHeader.get_entry(iVar4);
					uVar7 = edFileSeek(bufferStart->pDebugBankData, puVar6->offset, ED_SEEK_SET);
					if (uVar7 == 0) {
						pDVar5->pOwningFiler->close(pDVar5);
						iVar4 = 0;
						pbVar8 = bnkInUse;
						do {
							if ((edCFiler_BNK_8*)(pbVar8 + 4) == bufferStart) {
								bnkInUse[iVar4] = 0;
								memset(field_0x218 + iVar4, 0, sizeof(edCFiler_BNK_8));
								break;
							}
							iVar4 = iVar4 + 1;
							pbVar8 = pbVar8 + 8;
						} while (iVar4 < 4);
						bVar1 = false;
					}
					else {
						bufferStart->pFileData = puVar6;
						memset(&pOutData->fileInfo, 0, sizeof(edFILE_INFO));
						pcVar3 = edFilePathGetFilePath(filePath);
						edStrCopy((pOutData->fileInfo).name, pcVar3 + 1);
						bVar1 = true;
						(pOutData->fileInfo).flags = 0x21;
						(pOutData->fileInfo).fileSize = bufferStart->pFileData->size;
						pOutData->pFileData = bufferStart;
					}
				}
			}
		}
	}
	return bVar1;
}

bool CFilerBank::get_physical_filename(char* outFilePath, char* pathBuff)
{
	bool bVar1;

	if ((outFilePath == (char*)0x0) || (pathBuff == (char*)0x0)) {
		bVar1 = false;
	}
	else {
		*outFilePath = '\0';
		bVar1 = true;
	}
	return bVar1;
}


bool CFilerBank::unmount_unit(char* filePath)
{
	// TODO
	IMPLEMENTATION_GUARD();
	return false;
}

bool CFilerBank::mount_unit(char* filePath, char* bankPath)
{
	bool bSuccess;
	edCFiler* peVar1;
	int iVar2;
	char* pcVar3;
	int iVar4;
	edCFiler_Bnk_static* __s;
	edCFiler_Bnk_static* peVar5;
	char formattedPath[512];
	char outString[512];

	peVar1 = edFileGetFiler(outString, bankPath, 0);
	if (peVar1 == (edCFiler*)0x0) {
		bSuccess = false;
	}
	else {
		edFilePathSplit((char*)0x0, formattedPath, (char*)0x0, (char*)0x0, filePath);
		iVar4 = 0;
		/* Check if the bank is already loaded. */
		peVar5 = BnkUnitTable;
		do {
			iVar2 = edStrICmp(peVar5->path, formattedPath);
			if (iVar2 == 0) goto LAB_002475e0;
			iVar4 = iVar4 + 1;
			peVar5 = peVar5 + 1;
		} while (iVar4 < 4);
		peVar5 = (edCFiler_Bnk_static*)0x0;
	LAB_002475e0:
		__s = (edCFiler_Bnk_static*)0x0;
		if (peVar5 == (edCFiler_Bnk_static*)0x0) {
			__s = BnkUnitTable;
			iVar4 = 0;
			do {
				if (__s->path[0] == '\0') {
					memset(__s, 0, sizeof(edCFiler_Bnk_static));
					edStrCopy(__s->path, formattedPath);
					goto LAB_00247648;
				}
				iVar4 = iVar4 + 1;
				__s = __s + 1;
			} while (iVar4 < 4);
			__s = (edCFiler_Bnk_static*)0x0;
		}
	LAB_00247648:
		bSuccess = false;
		if (__s != (edCFiler_Bnk_static*)0x0) {
			edStrCopy(__s->diskPath, outString);
			pcVar3 = edBankFilerReadHeader(TO_HEAP(H_MAIN), bankPath, 1, (edFILEH**)0x0);
			__s->pFileData = (edCFiler_Bnk_static_header*)pcVar3;
			bSuccess = true;
		}
	}
	return bSuccess;
}


// Should be in: D:/projects/EdenLib/edBank/sources/edBankFiler.cpp
bool CFilerBank::terminate()
{
	return true;
}


// Should be in: D:/projects/EdenLib/edBank/sources/edBankFiler.cpp
bool CFilerBank::initialize()
{
	/* <BNK> */
	this->pDriveName_0x0 = "<BNK>";
	this->flags = 1;
	this->set_default_unit("0:");
	return true;
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBankFiler.cpp
void edBankFilerRemove(void)
{
	// TODO 
	//edFilerList.remove_filer(&FilerBank);
	return;
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBankFiler.cpp
void edBankFilerInstall(void)
{
	edFilerList.add_filer(&FilerBank);
	/* <BNK> */
	FilerBank.pDriveName_0x0 = "<BNK>";
	FilerBank.flags = 1;
	FilerBank.set_default_unit("0:");
	return;
}