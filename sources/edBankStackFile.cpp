#include "edBank/edBankStackFile.h"

#include <string.h>

#include "edBank/edBankFile.h"
#include "edFile/edFile.h"
#include "EdenLib/edSys/sources/EdSystem.h"
#include "edBank/edBankBuffer.h"

edCBankStack edBankStack;


void edBankStackFileCallBack(int idA, int idB, char* pReadBuffer)
{
	edCBankCallback* pTypePairData;
	void* puVar1;
	LoadBankFileFunc pLVar2;
	edFILEH* pDVar3;
	int bankStackCurIndex;
	uint uVar5;
	edCFiler_Bnk_static_header* pcVar1;

	bankStackCurIndex = edBankStack.currentIndex;
	if (edBankStack.loadedBanks != 0) {
		pcVar1 = (edCFiler_Bnk_static_header*)edBankStack.aBankQueue[edBankStack.currentIndex].pReadBuffer;
		if ((edCFiler_Bnk_static_header*)pReadBuffer == pcVar1) {
			pTypePairData = (edCBankCallback*)edBankStack.aBankQueue[edBankStack.currentIndex].pBankTypePairData_0xc;
			puVar1 = edBankStack.aBankQueue[edBankStack.currentIndex].pObjectReference_0x10;
			(edBankStack.aBankQueue[edBankStack.currentIndex].pBankHeader_0x0)->accessFlag = 0;

			if (((pcVar1->fileHeader).flags_0x4 & 1) != 0) {
				pcVar1->fileHeader.unpack();
			}

			if ((edBankStack.aBankQueue[bankStackCurIndex].fileFlagB_0x18 & 8U) == 0) {
				pLVar2 = edBankStack.aBankQueue[bankStackCurIndex].fileFunc_0x14;
				if (pLVar2 != (LoadBankFileFunc)0x0) {
					(pLVar2)(false, puVar1);
				}
				pcVar1->fileHeader.apply_callback(pTypePairData, 0);
				if (pLVar2 != (LoadBankFileFunc)0x0) {
					(pLVar2)(true, puVar1);
				}
				pcVar1->fileHeader.analyse();
			}

			pDVar3 = edBankStack.aBankQueue[bankStackCurIndex].pDebugBankData;
			if (pDVar3 != (edFILEH*)0x0) {
				uVar5 = edFileGetSize(pDVar3);
				(pcVar1->fileHeader).sizePacked = uVar5;
			}

			edBankStack.loadedBanks = edBankStack.loadedBanks + -1;

			if ((edBankStack.loadedBanks != 0) &&
				(edBankStack.currentIndex = edBankStack.currentIndex + 1, edBankStack.currentIndex == 0xc)) {
				edBankStack.currentIndex = 0;
			}
		}
	}

	return;
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBankStackFile.cpp
bool edCBankStack::add_file(edCBankStackElement* pInBank)
{
	int iVar1;
	edCBankStackElement* pRVar2;

	iVar1 = this->loadedBanks;
	if (iVar1 != 0xc) {
		pRVar2 = this->aBankQueue + (this->currentIndex + iVar1) % 0xc;
		pRVar2->pBankHeader_0x0 = pInBank->pBankHeader_0x0;
		pRVar2->pReadBuffer = pInBank->pReadBuffer;
		pRVar2->pDebugBankData = pInBank->pDebugBankData;
		pRVar2->pBankTypePairData_0xc = pInBank->pBankTypePairData_0xc;
		pRVar2->pObjectReference_0x10 = pInBank->pObjectReference_0x10;
		pRVar2->fileFunc_0x14 = pInBank->fileFunc_0x14;
		pRVar2->fileFlagB_0x18 = pInBank->fileFlagB_0x18;
		this->loadedBanks = this->loadedBanks + 1;
	}

	return iVar1 != 0xc;
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBankStackFile.cpp
void edCBankStack::terminate()
{
	edSysHandlersRemove(edFileHandlers.nodeParent, edFileHandlers.entries, edFileHandlers.maxEventID, ED_HANDLER_FILE_READ, edBankStackFileCallBack);
	memset(this, 0, sizeof(edCBankStack));
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBankStackFile.cpp
void edCBankStack::initialize()
{
	memset(this, 0, sizeof(edCBankStack));
	edSysHandlersAdd(edFileHandlers.nodeParent, edFileHandlers.entries, edFileHandlers.maxEventID, ED_HANDLER_FILE_READ, edBankStackFileCallBack, 1, 1);
	return;
}

edCBankStack::edCBankStack()
{
	memset(this, 0, sizeof(edCBankStack));
}