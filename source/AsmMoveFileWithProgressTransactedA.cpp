#include "AsmMoveFileWithProgressTransactedA.h"

AsmMoveFileWithProgressTransactedA::AsmMoveFileWithProgressTransactedA(Module* M)
{
	Mod = M;
	CallMoveFileA = nullptr;
	AsmApi = nullptr;
	Basep8BitStringToDynamicUnicode_VA = nullptr;
	__imp__MoveFileWithProgressTransactedW_VA = nullptr;
	__imp__RtlFreeUnicodeString_VA = nullptr;
	Basep8BitStringToDynamicUnicodeString_RVA = 128112;
	__imp__MoveFileWithProgressTransactedW_RVA = 527276;
	__imp__RtlFreeUnicodeString_RVA = 531624;
}

BOOL AsmMoveFileWithProgressTransactedA::CreateAsmApi()
{
	Type* VoidTy = Type::getVoidTy(Mod->getContext());
	vector<Value*> Para;
	string AsmString;
	string Constraints;

	FunctionType* FunTy = FunctionType::get(VoidTy, false);
	AsmApi = Function::Create(FunTy,GlobalValue::LinkageTypes::ExternalLinkage,"AsmMoveFileWithProgressTransactedA",Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);

	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);

	Para.clear();
	AsmString =
		"mov edi, edi\n\tpush ebp\n\tmov ebp, esp\n\tsub esp,10h\n\tlea eax, [ebp-8]\n\t"
		"push esi\n\tpush ecx\n\tpush eax\n\tmov esi, edx\n\t";
	Constraints = "~{dirflag},~{fpsr},~{flags}";
	InsertAsmCode(AsmString, Constraints, Para, RetInst);

	Para.clear();
	Para.push_back(Basep8BitStringToDynamicUnicode_VA);
	AsmString = "mov eax,$0";
	Constraints = "*m,~{eax},~{dirflag},~{fpsr},~{flags}";
	InsertAsmCode(AsmString, Constraints, Para, RetInst);

	Para.clear();
	AsmString = "call eax";
	Constraints = "~{dirflag},~{fpsr},~{flags}";
	InsertAsmCode(AsmString, Constraints, Para, RetInst);

	AsmString =
		"test eax,eax\n\tjz L__MSASMLABEL_.${:uid}loc_6B82FA28\n\ttest esi, esi\n\t"
		"jz L__MSASMLABEL_.${:uid}loc_6B81F464\n\tpush esi\n\tlea eax,[ebp-10h]\n\tpush eax\n\t"
		"mov eax, $0\n\tcall eax\n\t"
		"test eax, eax\n\t"
		"jz L__MSASMLABEL_.${:uid}loc_6B82FA1E\n\t"
		"mov eax, [ebp-0ch]\n\t"
//loc_6B81F431:
		"L__MSASMLABEL_.${:uid}loc_6B81F431:\n\t"
		"push [ebp+14h]\n\tpush [ebp+10h]\n\tpush [ebp+0ch]\n\tpush [ebp+8h]\n\t"
		"push eax\n\tpush [ebp-4h]\n\t"
// 80bac __imp__MoveFileWithProgressTransactedW@24
		"mov eax,$1\n\tmov eax, [eax]\n\tcall eax\n\t"
		"mov esi,eax\n\tlea eax, [ebp-8h]\n\tpush eax\n\t"
// __imp__RtlFreeUnicodeString@4
		"mov eax,$2\n\tmov eax, [eax]\n\tcall eax\n\t"
		"lea eax, [ebp-10h]\n\tpush eax\n\t"
//__imp__RtlFreeUnicodeString@4
		"mov eax,$3\n\tmov eax, [eax]\n\tcall eax\n\t"
		"mov eax, esi\n\t"
//loc_6B81F45F:
		"L__MSASMLABEL_.${:uid}loc_6B81F45F:\n\tpop esi\n\tleave\n\tretn 10h\n\t"
//loc_6B81F464:
		"L__MSASMLABEL_.${:uid}loc_6B81F464:\n\txor eax, eax\n\tmov [ebp-0ch],eax\n\t"
		"jmp L__MSASMLABEL_.${:uid}loc_6B81F431\n\t"
//loc_6B82FA1E:
		"L__MSASMLABEL_.${:uid}loc_6B82FA1E:\n\tlea eax, [ebp-8]\n\tpush eax\n\t"
//__imp__RtlFreeUnicodeString@4
		"mov eax,$4\n\tmov eax, [eax]\n\tcall eax\n\t"
//loc_6B82FA28:
		"L__MSASMLABEL_.${:uid}loc_6B82FA28:\n\txor eax, eax\n\tjmp L__MSASMLABEL_.${:uid}loc_6B81F45F";
	Constraints = "*m,*m,*m,*m,*m,~{dirflag},~{fpsr},~{flags}";
	Para.clear();
	Para.push_back(Basep8BitStringToDynamicUnicode_VA);
	Para.push_back(__imp__MoveFileWithProgressTransactedW_VA);
	Para.push_back(__imp__RtlFreeUnicodeString_VA);
	Para.push_back(__imp__RtlFreeUnicodeString_VA);
	Para.push_back(__imp__RtlFreeUnicodeString_VA);
	InsertAsmCode(AsmString, Constraints, Para, RetInst);
	return true;
}
BOOL AsmMoveFileWithProgressTransactedA::RelocFuncAddr(Instruction* InsertBefore,CallInst* CallRelocInst)
{
	Basep8BitStringToDynamicUnicode_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())), 
		"Basep8BitStringToDynamicUnicode_RVA");
	ConstantInt *offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), Basep8BitStringToDynamicUnicodeString_RVA);
	BinaryOperator *ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, Basep8BitStringToDynamicUnicode_VA, InsertBefore);
	__imp__MoveFileWithProgressTransactedW_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())), 
		"__imp__MoveFileWithProgressTransactedW");
	offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), __imp__MoveFileWithProgressTransactedW_RVA);
	ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, __imp__MoveFileWithProgressTransactedW_VA, InsertBefore);

	__imp__RtlFreeUnicodeString_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"__imp__MoveFileWithProgressTransactedW");
	offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), __imp__RtlFreeUnicodeString_RVA);
	ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, __imp__RtlFreeUnicodeString_VA, InsertBefore);
	return true;
}
BOOL AsmMoveFileWithProgressTransactedA::GetKernel32AddrtoGlobal()
{
	Function* MainFunc = GetFunction(Mod,"main");
	Function* RelocFunc = GetFunction(Mod,"GetKernel32ImageBase");
	if (!RelocFunc)
		return false;
	if (!MainFunc)
		return false;
	Instruction* MainFirstInst = &*MainFunc->begin()->begin();
	CallInst *CallRelocFunc = CallInst::Create(RelocFunc, "", MainFirstInst);
	RelocFuncAddr(MainFirstInst, CallRelocFunc);
	return true;
}

BOOL AsmMoveFileWithProgressTransactedA::obfMoveFileWithProgressTransactedA()
{
	if (!GetKernel32AddrtoGlobal())
	{
		errs() << "Faile to Get Kernel32 ImageBase..\n";
		return false;
	}
	if (!CreateAsmApi())
	{
		errs() << "Faile to Create AsmApi Function..\n";
		return false;
	}
	return true;
}
