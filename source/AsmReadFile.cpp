#include "AsmReadFile.h"
#include "jsoncpp/json.h"

AsmReadFile::AsmReadFile(Module* M)
{
	Mod = M;
	OriginApiCallInst = nullptr;
	vec_RelocDataStruct.clear();
	ImageBaseStruct.Kernel32ImageCallInst = nullptr;
	ImageBaseStruct.KernelBaseImageCallInst = nullptr;
	ImageBaseStruct.NtdllImageCallInst = nullptr;
}
BOOL AsmReadFile::GetDLLImageBase()
{
	Function* MainFunc = GetFunction(Mod, "main");
	if (!MainFunc)
		return false;
	BasicBlock* DataInitBB = BasicBlock::Create(Mod->getContext(), "DataInitBB", MainFunc, &*MainFunc->begin());
	BranchInst* DataInitBrInst = BranchInst::Create(&*MainFunc->begin()->getNextNode(), DataInitBB);
	Function* RelocFunc = GetFunction(Mod, "GetKernel32ImageBase");
	if (!RelocFunc)
		return false;
	CallInst *CallRelocFunc = CallInst::Create(RelocFunc, "", DataInitBrInst);
	ImageBaseStruct.Kernel32ImageCallInst = CallRelocFunc;
	RelocFunc = GetFunction(Mod, "GetNtdllImageBase");
	if (!RelocFunc)
		return false;
	CallRelocFunc = CallInst::Create(RelocFunc, "", DataInitBrInst);
	ImageBaseStruct.NtdllImageCallInst = CallRelocFunc;
	RelocFunc = GetFunction(Mod, "GetKernelBaseImageBase");
	if (!RelocFunc)
		return false;
	CallRelocFunc = CallInst::Create(RelocFunc, "", DataInitBrInst);
	ImageBaseStruct.KernelBaseImageCallInst = CallRelocFunc;
	return true;
}
BOOL AsmReadFile::UpdataFunAddr(GlobalVariable* OldGlobal, Function* AsmFunc)
{
	Function* MainFunc = GetFunction(Mod, "main");
	if (!MainFunc)
		return false;
	Instruction* DataInitBBTerInst = MainFunc->begin()->getTerminator();
	PtrToIntInst* AsmFunc2PtrInst = new PtrToIntInst(AsmFunc, Type::getInt32Ty(Mod->getContext()), "", DataInitBBTerInst);
	GlobalVariable* PtrGlobal = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		OldGlobal->getName() + "_ptr");
	PtrToIntInst* Global2IntAddr = new PtrToIntInst(PtrGlobal, Type::getInt32Ty(Mod->getContext()), "", DataInitBBTerInst);
	new StoreInst(AsmFunc2PtrInst, PtrGlobal, DataInitBBTerInst);
	new StoreInst(Global2IntAddr, OldGlobal, DataInitBBTerInst);
	return true;
}
BOOL AsmReadFile::RelocDataAddr()
{
	Function* MainFunc = GetFunction(Mod, "main");
	if (!MainFunc)
		return false;
	Instruction* InsertBefore = MainFunc->begin()->getTerminator();
	for (int k = 0; k < vec_RelocDataStruct.size(); k++)
	{
		for (int i = 0; i < vec_RelocDataStruct[k].Data_VA_RVA.size(); i++)
		{
			ConstantInt *offset = (ConstantInt*)ConstantInt::get(vec_RelocDataStruct[k].ImageBaseCallInst->getType(), vec_RelocDataStruct[k].Data_VA_RVA[i].first);
			BinaryOperator *ApiVa = BinaryOperator::Create(Instruction::Add,
				offset, vec_RelocDataStruct[k].ImageBaseCallInst,"", InsertBefore);
			new StoreInst(ApiVa, vec_RelocDataStruct[k].Data_VA_RVA[i].second, InsertBefore);
		}
	}
	UpdataFunAddr(vec_RelocDataStruct[0].Data_VA_RVA[2].second, GetFunction(Mod, "NtWriteFile_Ntdll"));
	return true;
}

RelocDataStruct AsmReadFile::ReadJson2str(const char* JsonFileName,string FunctionName)
{
	RelocDataStruct RelocDataStructTmp;
	Json::Reader reader;
	Json::Value  JsonFile;
	reader.parse(getfileAll(JsonFileName), JsonFile, false);
	for (Json::Value::iterator iter = JsonFile.begin(); iter != JsonFile.end(); ++iter)
	{
		Json::Value &Func = *iter;
		Json::Value ValueName = iter.name();
		if (ValueName.asString() == FunctionName)
		{
			RelocDataStructTmp.Data_VA_RVA.clear();
			RelocDataStructTmp.DirectRelyAsmApi.clear();
			RelocDataStructTmp.IndirectRelyAsmApi.clear();

			RelocDataStructTmp.FunName = ValueName.asString();								
			RelocDataStructTmp.DLLName = Func["DLLName"].asString();						
			RelocDataStructTmp.AsmString = Func["AsmString"].asString();					
			RelocDataStructTmp.AsmConstraints = Func["AsmConstraints"].asString();			
			RelocDataStructTmp.isOutApi = Func["flag"].asBool();							
			Json::Value & ValueDirectRelyAsmApi = Func["DirectRelyAsmApi"];				
			for (int i = 0; i < ValueDirectRelyAsmApi.size(); i++)
			{
				RelocDataStructTmp.DirectRelyAsmApi.push_back(ValueDirectRelyAsmApi[i].asString());
			}
			Json::Value & ValueIndirectRelyAsmApi = Func["IndirectRelyAsmApi"];        
			for (int i = 0; i < ValueDirectRelyAsmApi.size(); i++)
			{
				RelocDataStructTmp.IndirectRelyAsmApi.push_back(ValueDirectRelyAsmApi[i].asString());
			}
			Json::Value & ValueData_RVA_Name = Func["Data_RVA_Name"];
			for (int i = 0; i < ValueData_RVA_Name.size(); i++)
			{
				RelocDataStructTmp.IndirectRelyAsmApi.push_back(ValueData_RVA_Name[i].asString());
			}
			Json::Value & ValueData_RVA = Func["Data_RVA"];
			for (int i = 0; i < ValueData_RVA.size(); i++)
			{
				GlobalVariable* VA = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
					GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
					ValueData_RVA_Name[i].asString());
				pair <DWORD, GlobalVariable*> tmppair;
				tmppair = make_pair(ValueData_RVA[i].asInt(), VA);
				RelocDataStructTmp.Data_VA_RVA.push_back(tmppair);
			}
		}
	}
	if (RelocDataStructTmp.DLLName == "Kernel32")
	{
		RelocDataStructTmp.ImageBaseCallInst = ImageBaseStruct.Kernel32ImageCallInst;
	}
	else if (RelocDataStructTmp.DLLName == "KernelBase")
	{
		RelocDataStructTmp.ImageBaseCallInst = ImageBaseStruct.KernelBaseImageCallInst;
	}
	else if(RelocDataStructTmp.DLLName == "Ntdll")
	{
		RelocDataStructTmp.ImageBaseCallInst = ImageBaseStruct.NtdllImageCallInst;
	}
	return RelocDataStructTmp;
}
BOOL AsmReadFile::FillDataStruct()
{
	RelocDataStruct RelocDataStructTmp;
	RelocDataStruct FunInfo;
	RelocDataStructTmp.Data_VA_RVA.clear();
	RelocDataStructTmp.DirectRelyAsmApi.clear();
	RelocDataStructTmp.IndirectRelyAsmApi.clear();
	RelocDataStructTmp = ReadJson2str(".\\APIInfo.json", "WriteFile");
	vec_RelocDataStruct.push_back(RelocDataStructTmp);
	for (int k = 0; k < RelocDataStructTmp.DirectRelyAsmApi.size(); k++)
	{
		FunInfo.Data_VA_RVA.clear();
		FunInfo.DirectRelyAsmApi.clear();
		FunInfo = ReadJson2str(".\\APIInfo.json", RelocDataStructTmp.DirectRelyAsmApi[k]);
		vec_RelocDataStruct.push_back(FunInfo);
	}
	return true;
}
BOOL AsmReadFile::CreateAsmApi()
{
	for (int k = 0; k < vec_RelocDataStruct.size(); k++)
	{
		Instruction* RetInst = MyCreateFunc(Mod, vec_RelocDataStruct[k].FunName + "_" + vec_RelocDataStruct[k].DLLName);
		vec_RelocDataStruct[k].AsmApiRetInst = RetInst;
	}
	return true;
}
BOOL AsmReadFile::AsmStr2AsmApi()
{
	vector<Value*> Para;
	string AsmString;
	string Constraints;
	for (int k = 0; k < vec_RelocDataStruct.size(); k++)
	{
		Para.clear();
		for (int i = 0; i < vec_RelocDataStruct[k].Data_VA_RVA.size(); i++)
		{
			Para.push_back(vec_RelocDataStruct[k].Data_VA_RVA[i].second);
		}
		AsmString = vec_RelocDataStruct[k].AsmString;
		Constraints = vec_RelocDataStruct[k].AsmConstraints;
		InsertAsmCode(AsmString, Constraints, Para, vec_RelocDataStruct[k].AsmApiRetInst);
	}
	return true;
}
BOOL AsmReadFile::PushApiArg(string OriginApiName)
{
	Function* OriginApi = GetFunction(Mod, OriginApiName);
	if (!OriginApi)
	{
		errs() << "There is no Specified Function to substitute..\n";
		return false;
	}
	for (User *U : OriginApi->users())
	{
		if (isa<CallInst>(U))
		{
			OriginApiCallInst = (CallInst*)U;
		}
	}
	vector<Value*> FuncTy_args;
	for (Use &U : OriginApiCallInst->operands())
	{
		Value *v = U.get();
		if (!isa<Function>(v))
		{
			GlobalVariable* ArgGlobal = new GlobalVariable(*Mod, v->getType(), false,
				GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(v->getType()),
				OriginApiName + "__ArgGlobal__");
			new StoreInst(v, ArgGlobal, OriginApiCallInst);
			OriginApiPara.push_back(ArgGlobal);
		}
	}
	return true;
}
BOOL AsmReadFile::CreateTrampoline_0(string AsmApiName, string InduceApiName)
{
	Instruction* RetInst = MyCreateFunc(Mod, "Trampoline_0");
	vector<Value*>Para;
	string AsmString = "push dword ptr $0";
	string Constraints = "*m,~{dirflag},~{fpsr},~{flags}";
	for (int k = OriginApiPara.size() - 1; k >= 0; k--)
	{
		Para.clear();
		Para.push_back(OriginApiPara[k]);
		InsertAsmCode(AsmString, Constraints, Para, RetInst);
	}
	CallInst* CI  = CallInst::Create(GetFunction(Mod, AsmApiName), "", RetInst);
	Function* HookApi = GetFunction(Mod, InduceApiName);
	PtrToIntInst* HookApi_addr = new PtrToIntInst(HookApi, Type::getInt32Ty(Mod->getContext()), "", RetInst);
	GlobalVariable* HookApi_addr_Global = new GlobalVariable(*Mod, Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"HookApi_addr_Global");
	new StoreInst(HookApi_addr, HookApi_addr_Global, RetInst);
	Para.clear();
	AsmString = "mov eax,$0\n\tadd eax,5\n\tmov edi,edi\n\tpush ebp\n\tmov ebp,esp\n\tjmp eax";
	Constraints = "*m,~{dirflag},~{fpsr},~{flags}";
	Para.push_back(HookApi_addr_Global);
	InsertAsmCode(AsmString, Constraints, Para, RetInst);
	return true;
}
BOOL AsmReadFile::CreateHook(string TrampolineFuncName, string InduceApiName)
{
	Function* MainFunc = GetFunction(Mod, "main");
	if (!MainFunc)
		return false;
	Instruction* InsertBefore = MainFunc->begin()->getTerminator();
	Function* HookFunc = GetFunction(Mod, "HookFunc");
	Function* OriginApi = GetFunction(Mod, InduceApiName);
	Function* TrampolineFunc = GetFunction(Mod, TrampolineFuncName);
	vector<Value*> HookFuncPara;
	HookFuncPara.clear();
	PtrToIntInst* PtrIntInst_OriginApi = new PtrToIntInst(OriginApi, Type::getInt32Ty(Mod->getContext()), "", InsertBefore);
	PtrToIntInst* PtrIntInst_AsmGetProcAddressFunc = new PtrToIntInst(TrampolineFunc, Type::getInt32Ty(Mod->getContext()), "", InsertBefore);
	HookFuncPara.push_back(PtrIntInst_OriginApi);
	HookFuncPara.push_back(PtrIntInst_AsmGetProcAddressFunc);
	CallInst::Create(HookFunc, HookFuncPara, "", InsertBefore);
	return true;
}
BOOL AsmReadFile::CallInduceApi(string InduceApiName, Instruction* InsertBefore)
{
	Function* InduceApi = GetFunction(Mod, InduceApiName);
	vector<Value*> InduceApiPara;
	InduceApiPara.clear();
	vector<Value*> InduceApiPara1;
	for (Function::arg_iterator args = InduceApi->arg_begin(); args != InduceApi->arg_end(); ++args)
	{
		GlobalVariable* ArgGlobal = new GlobalVariable(*Mod, args->getType(), false,
			GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(args->getType()),
			InduceApi->getName() + "ArgGlobal");
		LoadInst* LI = new LoadInst(ArgGlobal, "", InsertBefore);
		InduceApiPara1.push_back(ArgGlobal);
		InduceApiPara.push_back(LI);
	}
	CallInst* CI = CallInst::Create(InduceApi, InduceApiPara, "", InsertBefore);
	vector<Value*> Para;
	Para.clear();
	string AsmString = "sub esp,4h";
	string Constraints = "~{dirflag},~{fpsr},~{flags}";
	return true;
}
BOOL AsmReadFile::InsertHook()
{
	string InduceApiName = "\01_MoveFileA@8";
	string Trampoline_0 = "Trampoline_0.5";
	for (int k = 0; k < vec_RelocDataStruct.size(); k++)
	{
		if (vec_RelocDataStruct[k].isOutApi)
		{
			CreateTrampoline_0(vec_RelocDataStruct[k].FunName + "_" + vec_RelocDataStruct[k].DLLName, InduceApiName);
			CreateHook(Trampoline_0, InduceApiName);
			CallInduceApi(InduceApiName, OriginApiCallInst);
		}
	}
	return true;
}
BOOL AsmReadFile::SubReturnValue()
{
	if (OriginApiCallInst->use_empty())  
	{
		OriginApiCallInst->eraseFromParent();
		return true;
	}
	Function* TramFun = GetFunction(Mod, "Trampoline_0");
	Function* AsmAPI = GetFunction(Mod, "WriteFile_KernelBase");
	Function* OriginAPI = GetFunction(Mod, "\01_WriteFile@20");
	vector<CallInst*> AsmAPI_CI;
	AsmAPI_CI.clear();
	for (User *U : AsmAPI->users())
	{
		if (isa<CallInst>(U))
		{
			U->dump();
			AsmAPI_CI.push_back((CallInst*)U);
		}
	}
	vector<Value*> Para;
	for (int k = 0; k < AsmAPI_CI.size(); k++)
	{
		GlobalVariable* ArgGlobal = new GlobalVariable(*Mod, OriginAPI->getReturnType(), false,
			GlobalValue::LinkageTypes::ExternalLinkage, Constant::getNullValue(OriginAPI->getReturnType()),
			OriginAPI->getName() + "RetGlobal");
		Para.clear();
		string Asmstring = "mov $0,eax";
		string Constrains = "*m,~{dirflag},~{fpsr},~{flags}";
		Para.push_back(ArgGlobal);
		for (int k = 0; k < AsmAPI_CI.size(); k++)
			InsertAsmCode(Asmstring, Constrains, Para, AsmAPI_CI[k]->getNextNode());
		LoadInst* RetValueLI = new LoadInst(ArgGlobal, "", OriginApiCallInst);
		OriginApiCallInst->replaceAllUsesWith(RetValueLI);
		OriginApiCallInst->eraseFromParent();
	}
	return true;
}
BOOL AsmReadFile::AsmReadFileEntry()
{
	if (!GetDLLImageBase())
	{
		errs() << "Faile to Get DLL ImageBase...\n";
		return false;
	}
	if (!FillDataStruct())
	{
		return false;
	}
	if (!CreateAsmApi())
	{
		return false;
	}
	if (!RelocDataAddr())
	{
		return false;
	}
	if (!AsmStr2AsmApi())
	{
		return false;
	}
	if (!PushApiArg("\01_WriteFile@20"))
	{
		return false;
	}
	if (!InsertHook())
	{
		return false;
	}
	if (!SubReturnValue())
	{
		return false;
	}
	return true;
}