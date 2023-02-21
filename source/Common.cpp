#include "Common.h"

Function* GetFunction(Module* Mod , StringRef Name)
{
	Function *SpecFunc = nullptr;
	SpecFunc = Mod->getFunction(Name);
	if (!SpecFunc)
	{
		return nullptr;
	}
	return SpecFunc;
}

CallInst* InsertAsmCode(string AsmString, string Constraints, vector<Value*> Para, Instruction* InsertBefore)
{
	Module* Mod = InsertBefore->getModule();
	Type* VoidTy = Type::getVoidTy(Mod->getContext());
	FunctionType* FuncTy;
	vector<Type*> FuncTy_args;
	Value* ValueAsm = NULL;
	FuncTy_args.clear();
	if (Para.empty())
	{
		FuncTy = FunctionType::get(VoidTy, false);
		ValueAsm = InlineAsm::get(FuncTy, AsmString, Constraints, true, false, InlineAsm::AD_Intel);
		CallInst* CI = CallInst::Create(ValueAsm, "", InsertBefore);
		return CI;
	}
	for (int k = 0; k < Para.size(); k++)
	{
		FuncTy_args.push_back(Para[k]->getType());
	}
	FuncTy = FunctionType::get(VoidTy, FuncTy_args, false);
	ValueAsm = InlineAsm::get(FuncTy, AsmString, Constraints, true, false, InlineAsm::AD_Intel);
	CallInst* CI = CallInst::Create(ValueAsm, Para, "", InsertBefore); 
	return CI;
}
ReturnInst* MyCreateFunc(Module* Mod, string MyFuncName)
{
	FunctionType* FunTy = FunctionType::get(Type::getVoidTy(Mod->getContext()), false);
	Function* AsmApi = Function::Create(FunTy, GlobalValue::LinkageTypes::ExternalLinkage, MyFuncName, Mod);
	BasicBlock* EntryBB = BasicBlock::Create(Mod->getContext(), "entry", AsmApi);
	ReturnInst* RetInst = ReturnInst::Create(Mod->getContext(), EntryBB);
	return RetInst;
}
BOOL MySetFunctionName(Module* Mod, string OriginName, string NewName)
{
	Function* Func = nullptr;
	Func = Mod->getFunction(OriginName);
	if (!Func)
	{
		errs() << "Faile to set specified function name...\n";
		return false;
	}
	Func->setName(NewName);
	return true;
}
char* getfileAll(const char *fname)
{
	FILE *fp;
	char *str;
	char txt[1000];
	int filesize;
	if ((fp = fopen(fname, "r")) == NULL) {
		printf("open file %s fail \n", fname);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	str = (char *)malloc(filesize);
	str[0] = 0;

	rewind(fp);
	while ((fgets(txt, 1000, fp)) != NULL) {
		strcat(str, txt);
	}
	fclose(fp);
	return str;
}

