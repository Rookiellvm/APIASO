#pragma once
#include "Common.h"
struct API_Info
{
	string FuncName;
	vector<CallInst*> APICallInst_vec;
	vector<InvokeInst*> APIInvokeInst_vec;
	Function* APIFunc;
};

static string LoadAPIName[];
class DynamicStolenAPI
{
public:
	DynamicStolenAPI(Module* M);
	BOOL DynamicStolenAPIEntry();
protected:
	BOOL SaveAPIName();
	BOOL ProcessCallInst(CallInst* APICI, string ApiNameStr);
	BOOL APIAddrtoGlobal(GlobalVariable* APIAddr_Global, string ApiNameStr, Instruction* InsertAfter);
	BOOL MatchApiNameStr(string API_Name);

	vector<API_Info> API_Info_vec;
	Module* Mod;
};

