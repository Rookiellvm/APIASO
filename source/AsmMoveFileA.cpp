#include "AsmMoveFileA.h"
#include <sstream>
#include <iostream>
AsmMoveFileA::AsmMoveFileA(Module* M)
{
	Mod = M;
	CallMoveFileA = nullptr;
	AsmApi = nullptr;
	ApiCallInst = nullptr;
	RelocFunc = nullptr;
	_MoveFileWithProgressTransactedA_RVA = 127995;
	_MoveFileWithProgressTransactedA_VA = nullptr;
	EableSecondFunc = true;
	Parameter.clear();
}

BOOL AsmMoveFileA::FindArgv()
{
	Function* ApiFunc = GetFunction(Mod, "\01_MoveFileA@8");
	if (!ApiFunc)
	{
		errs() << "Faile to Find Specified Function...\n";
		return false;
	}
	CallInst* CI = nullptr;
	for (Function &Func : *Mod)//找到被替换API对应的Call指令
	{
		for (BasicBlock &BB : Func)
		{
			for (Instruction &I : BB)
			{
				if (CI = dyn_cast<CallInst>(&I))
				{
					if (CI->getCalledFunction() == ApiFunc)
					{
						ApiCallInst = CI;
						break;
					}
						
				}
			}
		}
	}
	if (!ApiCallInst)
	{
		return false;
	}
	for (Use &U : ApiCallInst->operands())
	{
		Value *v = U.get();
		if (isa<Instruction>(v))
			Parameter.push_back(v);
	}
	return true;
}

BOOL AsmMoveFileA::TransformArg()
{
	vector<Type*>FuncTy_args;
	for (int k = Parameter.size() - 1; k >= 0; k--)
	{
		FuncTy_args.clear();
		AllocaInst* AI = new AllocaInst(Parameter[k]->getType(), 0, 0, 4, "", ApiCallInst);
		new StoreInst(Parameter[k], AI, ApiCallInst);
		FuncTy_args.push_back(AI->getType());	
		FunctionType *FuncTy = FunctionType::get(Type::getVoidTy(Mod->getContext()), FuncTy_args, false);
		Value *pushasm = InlineAsm::get(FuncTy, "push dword ptr $0", 
			"*m,~{esp},~{dirflag},~{fpsr},~{flags}", true, false, InlineAsm::AD_Intel);
		CallInst* CallPush = CallInst::Create(pushasm, AI, "", ApiCallInst);
	}
	CallInst* Callobfapi = CallInst::Create(AsmApi, "", ApiCallInst);
	ApiCallInst->eraseFromParent();
	return true;
}

BOOL AsmMoveFileA::CreateAsmApi()
{
	Type* VoidTy = Type::getVoidTy(Mod->getContext());
	vector<Value*> Para;
	string AsmString;
	string Constraints;
	FunctionType* FunTy = FunctionType::get(VoidTy, false);
	AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, "AsmMoveFileA", Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);
	if (EableSecondFunc)
	{
		Para.clear();
		AsmString =
			"mov edi, edi\n\tpush ebp\n\tmov ebp, esp\n\t"
			"mov edx, [ebp+0ch]\n\txor eax, eax\n\tmov ecx,[ebp+8]\n\t"
			"push eax\n\tpush 2\n\tpush eax\n\tpush eax\n\t";
		Constraints = "~{dirflag},~{fpsr},~{flags}";
		InsertAsmCode(AsmString, Constraints, Para, RetInst);
		CallInst::Create(Mod->getFunction("AsmMoveFileWithProgressTransactedA"), "", RetInst);
		AsmString = "pop ebp\n\tretn 8";
		Constraints = "~{dirflag},~{fpsr},~{flags}";
		InsertAsmCode(AsmString, Constraints, Para, RetInst);	
	}
	else
	{
		Para.clear();
		AsmString =
			"mov edi, edi\n\tpush ebp\n\tmov ebp, esp\n\t"
			"mov edx, [ebp+0ch]\n\txor eax, eax\n\tmov ecx,[ebp+8]\n\t"
			"push eax\n\tpush 2\n\tpush eax\n\tpush eax\n\t"
			"mov eax, $0\n\tcall eax\n\t"
			"pop ebp\n\tretn 8";
		Constraints = "*m,~{dirflag},~{fpsr},~{flags}";
		Para.push_back(_MoveFileWithProgressTransactedA_VA);
		InsertAsmCode(AsmString, Constraints, Para, RetInst);
	}
	return true;
}
BOOL AsmMoveFileA::RelocFuncAddr(Instruction* InsertBefore, CallInst* CallRelocInst)
{
	_MoveFileWithProgressTransactedA_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"_MoveFileWithProgressTransactedA_VA");
	ConstantInt *offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), _MoveFileWithProgressTransactedA_RVA);
	BinaryOperator *ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, _MoveFileWithProgressTransactedA_VA, InsertBefore);
	return true;
}

BOOL AsmMoveFileA::GetKernel32AddrtoGlobal()
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

BOOL AsmMoveFileA::ObfMoveFileA()
{
	EableSecondFunc = true;
	if (!GetKernel32AddrtoGlobal())
	{
		errs() << "Faile to Reloc Function Address...\n";
		return false;
	}
	if (!CreateAsmApi())
	{
		errs() << "Faile to Create AsmApi Function..\n";
		return false;
	}
	if (!FindArgv())
	{
		errs() << "Faile to Create Find  Api Arguments..\n";
		return false;
	} 
	if (!TransformArg())
	{
		errs() << "Faile Transform Arg to AsmApi..\n";
		return false;
	}

	return true;
}


