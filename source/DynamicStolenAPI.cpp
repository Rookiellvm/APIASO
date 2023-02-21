#include "DynamicStolenAPI.h"

DynamicStolenAPI::DynamicStolenAPI(Module* M)
{
	Mod = M;
	API_Info_vec.clear();
}


BOOL DynamicStolenAPI::MatchApiNameStr(string API_Name)
{
	int i = 0;
	while (LoadAPIName[i] != "\0")
	{
		if (API_Name.find(LoadAPIName[i]) != StringRef::npos)
		{
			return true;
		}
		i++;
	}
	return false;
}

BOOL DynamicStolenAPI::SaveAPIName()
{
	vector<string> APINameStr;
	APINameStr.clear();
	string FuncNameStr;
	API_Info tmp_API_Info;
	for (Function &Func : *Mod)
	{
		if (Func.isDeclaration() && Func.getCallingConv() == CallingConv::X86_StdCall)
		{
			FuncNameStr = Func.getName().split("@").first.split("_").second;  
			if (MatchApiNameStr(FuncNameStr))
				continue;
			tmp_API_Info.APIFunc = &Func;
			tmp_API_Info.FuncName = FuncNameStr;
			API_Info_vec.push_back(tmp_API_Info);
		}
	}
	if (API_Info_vec.size() == 0) return false;
	for (int j = 0; j < API_Info_vec.size(); j++)
	{
		Function* tmpFunction = API_Info_vec[j].APIFunc;
		for (llvm::Value::user_iterator Useri = tmpFunction->user_begin(); Useri != tmpFunction->user_end(); Useri++)
		{
			if (llvm::CallInst* CI = llvm::dyn_cast<llvm::CallInst>(*Useri))
				API_Info_vec[j].APICallInst_vec.push_back(CI);
			if (llvm::InvokeInst * II = llvm::dyn_cast<llvm::InvokeInst>(*Useri))
				API_Info_vec[j].APIInvokeInst_vec.push_back(II);
		}
	}
	return true;
}

BOOL DynamicStolenAPI::ProcessCallInst(CallInst* CI,string ApiNameStr)
{
	StringRef API_Name = CI->getCalledFunction()->getName();
	PointerType* CurrentFunTy = PointerType::get(CI->getCalledFunction()->getValueType(), 0);
	AllocaInst* ptr_fn_CurrentFunc = new AllocaInst(CurrentFunTy, 0,0,4,"fn_" + CI->getCalledFunction()->getName().str(), CI);

	GlobalVariable* gvar_pAPI = new GlobalVariable(*Mod, 
		Type::getInt32Ty(Mod->getContext()), false,
		GlobalValue::LinkageTypes::ExternalLinkage, 
		Constant::getNullValue(Type::getInt32Ty(Mod->getContext())),
		"gvar_p" + ApiNameStr);

	LoadInst* load_g_APIAddr = new LoadInst(gvar_pAPI, gvar_pAPI->getName().str(), false, CI);
	APIAddrtoGlobal(gvar_pAPI, ApiNameStr, load_g_APIAddr);
	CastInst* castInt2ptr = new IntToPtrInst(load_g_APIAddr, CurrentFunTy, "funptr_" + ApiNameStr, CI);
	StoreInst* store2fn = new StoreInst(castInt2ptr, ptr_fn_CurrentFunc, false, CI);

	std::vector<Value*> ptr_fn_params;
	for (int i = 0; i < CI->getNumArgOperands(); i++)
	{
		ptr_fn_params.push_back(CI->getArgOperand(i)); 
	}
	LoadInst* load_call = new LoadInst(ptr_fn_CurrentFunc, "", false, CI);
	CallInst* ptr_call_CurrentFunc = CallInst::Create(load_call, ptr_fn_params, "call_" + CI->getCalledFunction()->getName().str(), CI);
	ptr_call_CurrentFunc->setCallingConv(CallingConv::X86_StdCall);
	ptr_call_CurrentFunc->setTailCall(false);

	CI->replaceAllUsesWith(ptr_call_CurrentFunc); 
	CI->dropAllReferences();
	CI->removeFromParent();
	return true;
}
BOOL DynamicStolenAPI::APIAddrtoGlobal(GlobalVariable* APIAddr_Global,string ApiNameStr,Instruction* InsertAfter)
{
	vector<Value*> GetAPIAddr_Func_Param;
	GetAPIAddr_Func_Param.clear();
	Constant* const_string = ConstantDataArray::getString(Mod->getContext(), ApiNameStr, true);
	GlobalVariable* gvar_pAPI = new GlobalVariable(*Mod,
		const_string->getType(),
		false,
		GlobalValue::LinkageTypes::ExternalLinkage,
		const_string,
		"my_string");
	ConstantInt* i_const = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), 0);
	ConstantInt* i_const1 = ConstantInt::get(Type::getInt32Ty(Mod->getContext()), 0);
	SmallVector<Value*, 2> indexVector;
	indexVector.push_back(i_const);
	indexVector.push_back(i_const1);
	GetElementPtrInst* GE = GetElementPtrInst::CreateInBounds(gvar_pAPI, indexVector,"", InsertAfter);
	GetAPIAddr_Func_Param.push_back((Value*)GE);
	Function* GetAPIAddr_Func = GetFunction(Mod,"?GetAPIAddr@@YAKPAD@Z");
	if (!GetAPIAddr_Func)
	{
		errs() << "Failed to Find GetAPIAddr...\n";
		return false;
	}
	CallInst* RetV = CallInst::Create(GetAPIAddr_Func, GetAPIAddr_Func_Param, "",InsertAfter);
	new StoreInst(RetV, APIAddr_Global, InsertAfter);
	return true;
}

BOOL DynamicStolenAPI::DynamicStolenAPIEntry()
{
	if (!SaveAPIName())
	{
		errs() << "This is no API to substitution....\n";
		return false;
	}
	for (int j = 0; j < API_Info_vec.size(); j++)
	{
		errs() << API_Info_vec[j].FuncName << "\n";
		for (int k = 0; k < API_Info_vec[j].APICallInst_vec.size(); k++)
		{
			if (API_Info_vec[j].APICallInst_vec[k]->getParent()->getParent()->getName().str() != "main")
				continue;
			API_Info_vec[j].APICallInst_vec[k]->dump();
			ProcessCallInst(API_Info_vec[j].APICallInst_vec[k], API_Info_vec[j].FuncName);
		}
	}
	return true;
}