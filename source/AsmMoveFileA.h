#pragma once
#include "Common.h"

class AsmMoveFileA
{
public:
	BOOL ObfMoveFileA();
	AsmMoveFileA(Module* M);
protected:
	Module* Mod;
	Function* CallMoveFileA;
	Function* AsmApi;
	Function *RelocFunc;
	CallInst *ApiCallInst;
	vector<Value*> Parameter;
	DWORD _MoveFileWithProgressTransactedA_RVA;
	GlobalVariable* _MoveFileWithProgressTransactedA_VA;

	//Function
	BOOL GetKernel32AddrtoGlobal();
	BOOL RelocFuncAddr(Instruction* InsertBefore, CallInst* CallRelocInst);
	BOOL CreateAsmApi();
	BOOL TransformArg();
	BOOL FindArgv();
	BOOL EableSecondFunc;
};