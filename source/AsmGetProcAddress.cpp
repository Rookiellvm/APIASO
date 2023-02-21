#include "AsmGetProcAddress.h"
AsmGetProcAddress::AsmGetProcAddress(Module* M)
{
	Mod = M;
	__imp__GetProcAddressForCaller_RVA = 527524;
	__imp__RtlInitString_RVA = 1950704;
	__imp__LdrGetProcedureAddressForCaller_RVA = 1951444;
	sub_1011F120_RVA = 1119120;
	__imp__RtlInitString_Addr = nullptr;
	StackVar = nullptr;
	MainFirstInst = nullptr;
	AsmApi = nullptr;
	EableSecondFunc = true;
	ApiCallInst = nullptr;
	AsmFunRetValue = nullptr;
	StackFlag_ESP = nullptr;
	StackFlag_EBP = nullptr;
	Parameter.clear();
}
BOOL AsmGetProcAddress::FindArgv()
{
	Function* ApiFunc = GetFunction(Mod, "\01_GetProcAddress@8");
	if (!ApiFunc)
	{
		errs() << "Faile to Find Specified Function...\n";
		return false;
	}
	CallInst* CI = nullptr;
	for (Function &Func : *Mod)
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
		if (!isa<Function>(v))
		{
			GlobalVariable* ArgGlobal = new GlobalVariable(*Mod, v->getType(), false,
				GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(v->getType()),
				v->getName()+"__ArgGlobal");
			new StoreInst(v, ArgGlobal, ApiCallInst);
			Parameter.push_back(ArgGlobal);
		}	
	}
	return true;
}
BOOL AsmGetProcAddress::TransformArg()
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
	Callobfapi = CallInst::Create(AsmApi, "", ApiCallInst);
	return true;
}
BOOL AsmGetProcAddress::RelocFuncAddr(Instruction* InsertBefore, CallInst* CallRelocInst)
{
	__imp__GetProcAddressForCaller_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"__imp__GetProcAddressForCaller_VA");
	ConstantInt *offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), __imp__GetProcAddressForCaller_RVA);
	BinaryOperator *ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, __imp__GetProcAddressForCaller_VA, InsertBefore);
	return true;
}
BOOL AsmGetProcAddress::RelocFuncAddr1(Instruction* InsertBefore, CallInst* CallRelocInst)
{
	__imp__RtlInitString_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"__imp__RtlInitString_VA");
	ConstantInt *offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), __imp__RtlInitString_RVA);
	BinaryOperator* ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, __imp__RtlInitString_VA, InsertBefore);

	__imp__LdrGetProcedureAddressForCaller_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"__imp__LdrGetProcedureAddressForCaller_VA");
	offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), __imp__LdrGetProcedureAddressForCaller_RVA);
	ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, __imp__LdrGetProcedureAddressForCaller_VA, InsertBefore);

	sub_1011F120_VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"sub_1011F120_VA");
	offset = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), sub_1011F120_RVA);
	ApiVa = BinaryOperator::Create(Instruction::Add, (BinaryOperator*)CallRelocInst, offset, "", InsertBefore);
	new StoreInst(ApiVa, sub_1011F120_VA, InsertBefore);
	return true;
}
BOOL AsmGetProcAddress::GetKernel32AddrtoGlobal()
{
	Function* MainFunc = GetFunction(Mod, "main");
	Function* Kernel32Func = GetFunction(Mod, "GetKernel32ImageBase");
	if (!Kernel32Func)
		return false;
	if (!MainFunc)
		return false;
	MainFirstInst = &*MainFunc->begin()->begin();
	CallInst *CallRelocFunc = CallInst::Create(Kernel32Func, "", MainFirstInst);
	RelocFuncAddr(MainFirstInst, CallRelocFunc);
	Function* KernelBaseFunc = GetFunction(Mod, "GetKernelBaseImageBase");
	if (!KernelBaseFunc)
		return false;
	CallRelocFunc = CallInst::Create(KernelBaseFunc, "", MainFirstInst);
	RelocFuncAddr1(MainFirstInst, CallRelocFunc);
	Function* HookFunc = GetFunction(Mod, "\01_MoveFileA@8");
	return true;
}

BOOL AsmGetProcAddress::ProcessCallInst()
{
	ApiCallInst->eraseFromParent();
	for (User *U : ApiCallInst->users())
	{
		if (Instruction *Inst = dyn_cast<Instruction>(U))
		{
			errs() << "F is used in instruction:\n";
			errs() << *Inst << "\n";
		}
	}
	return true;
}

BOOL AsmGetProcAddress::CreateAsmApi()
{
	Type* VoidTy = Type::getVoidTy(Mod->getContext());
	vector<Value*> Para;
	string AsmString;
	string Constraints;
	FunctionType* FunTy = FunctionType::get(VoidTy, false);
	AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, "AsmGetProcAddress", Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);
	if (EableSecondFunc)
	{
		Para.clear();
		AsmString =
			"mov edi, edi\n\tpush ebp\n\tmov ebp, esp\n\t"
			"push dword ptr [ebp+4h]\n\t"
			"push [ebp+0ch]\n\t"
			"push [ebp+8h]";
		Constraints = "~{dirflag},~{fpsr},~{flags}";
		InsertAsmCode(AsmString, Constraints, Para, RetInst);
		CallInst::Create(Mod->getFunction("AsmGetProcAddressForCaller"), "", RetInst);
		AsmString = "pop ebp\n\t"
					"retn 8h";
		InsertAsmCode(AsmString, Constraints, Para, RetInst);
	}
	else
	{
		Para.clear();
		AsmString =
			"mov edi, edi\n\tpush ebp\n\tmov ebp, esp\n\t"
			"push dword ptr [ebp+4h]\n\t"
			"push [ebp+0ch]\n\t"
			"push [ebp+8h]\n\t"
			"mov eax, $0\n\tmov eax, [eax]\n\tcall eax\n\t"
			"pop ebp\n\t"
			"retn 8h";
		Constraints = "*m,~{dirflag},~{fpsr},~{flags}";
		Para.push_back(__imp__GetProcAddressForCaller_VA);
		InsertAsmCode(AsmString, Constraints, Para, RetInst);
	}
	return true;
}

BOOL AsmGetProcAddress::CreateAsmApi_GetProcAddressForCaller()
{
	Type* VoidTy = Type::getVoidTy(Mod->getContext());
	vector<Value*> Para;
	string AsmString;
	string Constraints;

	FunctionType* FunTy = FunctionType::get(VoidTy, false);
	AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, "AsmGetProcAddressForCaller", Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst1 = ReturnInst::Create(Mod->getContext(), EntryBB);
	Para.clear();
	Constraints = "*m,~{dirflag},~{fpsr},~{flags}";
	Para.push_back(__imp__RtlInitString_Addr);
	InsertAsmCode(AsmGetProcAddressForCallerStr, Constraints, Para, RetInst1);

	AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, "AsmGetProcAddressForCaller_1", Mod);
	EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);
	Para.clear();
	Constraints = "*m,*m,*m,*m,~{dirflag},~{fpsr},~{flags}";
	StackVar = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"StackVar");
	Para.push_back(StackVar);
	Para.push_back(__imp__LdrGetProcedureAddressForCaller_VA);
	Para.push_back(StackVar);
	Para.push_back(sub_1011F120_VA);
	InsertAsmCode(AsmGetProcAddressForCallerStr1, Constraints, Para, RetInst);

	CallInst* CallPush = CallInst::Create(AsmApi, "", RetInst1);
	Para.clear();
	Constraints = "~{dirflag},~{fpsr},~{flags}";
	InsertAsmCode("leave\n\tretn 0ch\n\t", Constraints, Para, RetInst1);
	return true;
}

BOOL AsmGetProcAddress::CreateAsmApi_RtlInitString()
{
	Type* VoidTy = Type::getVoidTy(Mod->getContext());
	vector<Value*> Para;
	string AsmString;
	string Constraints;
	
	FunctionType* FunTy = FunctionType::get(VoidTy, false);
	AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, "AsmRtlInitString", Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);
	
	Para.clear();
	Constraints = "~{dirflag},~{fpsr},~{flags}";
	InsertAsmCode(AsmRtlInitStringStr, Constraints, Para, RetInst);
	__imp__RtlInitString_Addr = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"__imp__RtlInitString_Addr");
	PtrToIntInst* PtrIntInst = new PtrToIntInst(AsmApi, Type::getInt32Ty(Mod->getContext()), "", MainFirstInst);
	new StoreInst(PtrIntInst, __imp__RtlInitString_Addr, MainFirstInst);	
	return true;
}

BOOL AsmGetProcAddress::CreateHook()
{
	Function* HookFunc = GetFunction(Mod,"HookFunc");
	HookApi = GetFunction(Mod, "\01_MoveFileA@8");
	Function* AsmGetProcAddressFunc = GetFunction(Mod, "AsmGetProcAddress");
	vector<Value*> HookFuncPara;
	HookFuncPara.clear();
	PtrToIntInst* PtrIntInst_OriginApi = new PtrToIntInst(HookApi, Type::getInt32Ty(Mod->getContext()), "", MainFirstInst);
	PtrToIntInst* PtrIntInst_AsmGetProcAddressFunc = new PtrToIntInst(AsmGetProcAddressFunc, Type::getInt32Ty(Mod->getContext()), "", MainFirstInst);
	HookFuncPara.push_back(PtrIntInst_OriginApi);
	HookFuncPara.push_back(PtrIntInst_AsmGetProcAddressFunc);
	CallInst::Create(HookFunc, HookFuncPara,"", MainFirstInst);
	HookFuncPara.clear();
	for (Function::arg_iterator args = HookApi->arg_begin(); args != HookApi->arg_end(); ++args)
	{
		GlobalVariable* ArgGlobal = new GlobalVariable(*Mod, args->getType(), false,
			GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(args->getType()),
			HookApi->getName() + "ArgGlobal");
		LoadInst* LI = new LoadInst(ArgGlobal, "", Callobfapi);
		HookFuncPara.push_back(LI);
	}
	vector<Value*> Para;
	CallInst* CI = CallInst::Create(HookApi, HookFuncPara,"", Callobfapi);
	Para.clear();
	string AsmString = "sub esp,8h";
	string Constraints = "~{flags},~{esp},~{dirflag},~{fpsr},~{flags}";
	InsertAsmCode(AsmString, Constraints, Para, Callobfapi);
	return true;
}

BOOL AsmGetProcAddress::CreateTrampolineFunc()
{
	FunctionType* FunTy = FunctionType::get(Type::getVoidTy(Mod->getContext()), false);
	AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, "TrampolineFunc", Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);
	vector<Value*>Para;
	string	AsmString = "push dword ptr $0\n\tpush dword ptr $1";
	string Constraints = "*m,*m,~{dirflag},~{fpsr},~{flags}";
	Para.clear();
	for (int k = Parameter.size() - 1; k >= 0; k--)
	{
		Para.push_back(Parameter[k]);
	}
	InsertAsmCode(AsmString, Constraints, Para, RetInst);
	CallInst::Create(GetFunction(Mod,"AsmGetProcAddress"), "", RetInst);
	HookApi = GetFunction(Mod, "\01_MoveFileA@8");
	PtrToIntInst* HookApi_addr = new PtrToIntInst(HookApi, Type::getInt32Ty(Mod->getContext()), "", RetInst);
	GlobalVariable* HookApi_addr_Global = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"__imp__RtlInitString_Addr");
	new StoreInst(HookApi_addr, HookApi_addr_Global, RetInst);
	Para.clear();
	AsmString = "mov eax,$0\n\tadd eax,5\n\tmov edi,edi\n\tpush ebp\n\tmov ebp,esp\n\tjmp eax";
	Constraints = "*m,~{dirflag},~{fpsr},~{flags}";
	Para.push_back(HookApi_addr_Global);
	InsertAsmCode(AsmString, Constraints, Para, RetInst);
	Function* HookFunc = GetFunction(Mod, "HookFunc");
	Function* OriginApi = GetFunction(Mod, "\01_MoveFileA@8");
	Function* AsmGetProcAddressFunc = GetFunction(Mod, "TrampolineFunc");
	vector<Value*> HookFuncPara;
	HookFuncPara.clear();
	PtrToIntInst* PtrIntInst_OriginApi = new PtrToIntInst(OriginApi, Type::getInt32Ty(Mod->getContext()), "", MainFirstInst);
	PtrToIntInst* PtrIntInst_AsmGetProcAddressFunc = new PtrToIntInst(AsmGetProcAddressFunc, Type::getInt32Ty(Mod->getContext()), "", MainFirstInst);
	HookFuncPara.push_back(PtrIntInst_OriginApi);
	HookFuncPara.push_back(PtrIntInst_AsmGetProcAddressFunc);
	CallInst::Create(HookFunc, HookFuncPara, "", MainFirstInst);
	return true;
}

BOOL AsmGetProcAddress::ObfGetProcAddress()
{
	EableSecondFunc = true;
	if (!GetKernel32AddrtoGlobal())
	{
		errs() << "Faile to Reloc Function Address...\n";
		return false;
	}
	if (!CreateAsmApi_RtlInitString())
	{
		return false;
	}
	if (!CreateAsmApi_GetProcAddressForCaller())
	{
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
	
	if (!ProcessCallInst())
	{
		errs() << "FaileProcess CallInst..\n";
		return false;
	}
	if (!CreateHook())
	{
		return false;
	}
	CreateTrampolineFunc();
	Callobfapi->eraseFromParent();
	return true;
}