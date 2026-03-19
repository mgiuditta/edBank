#include "edBank/edBank.h"
#include "edBank/edBankFiler.h"
#include "edBank/edBankStackFile.h"

edCBankConfig edBankConfig = { 0, 0, 0 };

byte _edBankAllowNoWait;

// Should be in: D:/projects/EdenLib/edBank/sources/edBank.cpp
void edBankTerm()
{
	edBankFilerRemove();
	edBankStack.terminate();
	//edBankDebug.terminate();
	return;
}

// Should be in: D:/projects/EdenLib/edBank/sources/edBank.cpp
void edBankInit(void)
{
	//edBankDebug.initialize();
	edBankStack.initialize();
	edBankFilerInstall();
	return;
}