#pragma once
#include "Common.h"

class AsmMoveFileWithProgressTransactedA
{
public:
	BOOL obfMoveFileWithProgressTransactedA();
	AsmMoveFileWithProgressTransactedA(Module* M);
private:
	Module* Mod;
	Function* CallMoveFileA;
	Function* AsmApi;
	GlobalVariable* Basep8BitStringToDynamicUnicode_VA;
	GlobalVariable* __imp__MoveFileWithProgressTransactedW_VA;
	GlobalVariable* __imp__RtlFreeUnicodeString_VA;
	DWORD Basep8BitStringToDynamicUnicodeString_RVA;
	DWORD __imp__MoveFileWithProgressTransactedW_RVA;
	DWORD __imp__RtlFreeUnicodeString_RVA;
	//Function
	BOOL GetKernel32AddrtoGlobal();
	BOOL RelocFuncAddr(Instruction* InsertBefore, CallInst* CallRelocInst);
	BOOL CreateAsmApi();
};

