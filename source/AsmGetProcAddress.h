#pragma once
#include "Common.h"
class AsmGetProcAddress
{
public:
	BOOL ObfGetProcAddress();
	AsmGetProcAddress(Module* M);
protected:
	Module* Mod;
	Function* AsmApi;
	Instruction* MainFirstInst;
	CallInst* Callobfapi;
	Function* PatchFunc;
	Function* HookApi;
	GlobalVariable* StackFlag_ESP;
	GlobalVariable* StackFlag_EBP;
	GlobalVariable* __imp__GetProcAddressForCaller_VA;
	GlobalVariable* __imp__RtlInitString_VA;
	GlobalVariable* __imp__LdrGetProcedureAddressForCaller_VA;
	GlobalVariable* sub_1011F120_VA;
	GlobalVariable* StackVar;

	DWORD __imp__GetProcAddressForCaller_RVA;
	DWORD __imp__RtlInitString_RVA;
	DWORD __imp__LdrGetProcedureAddressForCaller_RVA;
	DWORD sub_1011F120_RVA;

	GlobalVariable* __imp__RtlInitString_Addr;
	BOOL EableSecondFunc;
	CallInst *ApiCallInst,*AsmFunRetValue;
	vector<Value*> Parameter;
	string AsmRtlInitStringStr =
		"push edi\n\t"
		"mov edi, [esp+0x4+0x8]\n\t"
		"mov edx, [esp+0x4+0x4]\n\t"
		"mov dword ptr [edx], 0\n\t"
		"mov [edx+0x4], edi\n\t"
		"or edi, edi\n\t"
		"jz L__MSASMLABEL_.${:uid}loc_4B2F5024\n\t"
		"or ecx, 0x0FFFFFFFF\n\t"
		"xor eax, eax\n\t"
		"repne scasb\n\t"
		"not ecx\n\t"
		"cmp ecx,0x0FFFF\n\t"
		"jbe L__MSASMLABEL_.${:uid}loc_4B2F501C\n\t"
		"mov ecx, 0x0FFFF\n\t"
//\ loc_4B2F501C:
		"L__MSASMLABEL_.${:uid}loc_4B2F501C:\n\t"
		"mov [edx+0x2], cx\n\t"
		"dec ecx\n\t"
		"mov [edx], cx\n\t"
//\ loc_4B2F5024:
		"L__MSASMLABEL_.${:uid}loc_4B2F5024:\n\t"
		"pop edi\n\t"
		"retn 0x8";

	string AsmGetProcAddressForCallerStr =
		"mov edi, edi\n\tpush ebp\n\tmov ebp, esp\n\t"
		"sub esp, 0ch\n\tpush ebx\n\tpush edi\n\t"
		"mov edi, 0FFFFh\n\t"
		"cmp [ebp+0ch], edi\n\t"
		"jbe L__MSASMLABEL_.${:uid}loc_1011BC1E\n\t"
		"push [ebp+0ch]\n\t"
		"lea eax, [ebp-0ch]\n\t"
		"push eax\n\t"
//\ RtlInitString
		"mov eax, $0\n\tcall eax\n\t"
		"mov ebx, [ebp+8h]\n\t"
		"test ebx, ebx\n\t"
		"jz L__MSASMLABEL_.${:uid}loc_1011BC53\n\t"
		"mov al, bl\n\t"
		"and al, 3h\n\t"
		"movzx eax, al\n\t"
		"neg eax\n\t"
		"sbb eax, eax\n\t"
		"not eax\n\t"
		"and eax, ebx\n\t"
//\ loc_1011BBDB:
		"L__MSASMLABEL_.${:uid}loc_1011BBDB:\n\t"
		"push [ebp+10h]\n\t"
		"lea ecx, [ebp-4h]\n\t"
		"push 0\n\t"
		"push ecx\n\t"
		"push 0\n\t"
		"lea ecx, [ebp-0ch]\n\t"
		"push ecx\n\t"
//\ jmp loc_1011BBEA
		"jmp L__MSASMLABEL_.${:uid}loc_Return_BB\n\t"
//\ loc_1011BC1E:
		"L__MSASMLABEL_.${:uid}loc_1011BC1E:\n\t"
		"mov ebx, [ebp+8h]\n\t"
		"test ebx,ebx\n\t"
		"jz L__MSASMLABEL_.${:uid}loc_1018E8A0\n\t"
		"mov al, bl\n\t"
		"and al, 3\n\t"
		"movzx eax, al\n\t"
		"neg eax\n\t"
		"sbb eax, eax\n\t"
		"not eax\n\t"
		"and eax, ebx\n\t"
//\ loc_1011BC38:
		"L__MSASMLABEL_.${:uid}loc_1011BC38:\n\t"
		"push [ebp+10h]\n\t"
		"lea ecx, [ebp-4h]\n\t"
		"push 0\n\t"
		"push ecx\n\t"
		"push [ebp+0ch]\n\t"
		"push 0\n\t"
		"jmp L__MSASMLABEL_.${:uid}loc_Return_BB\n\t"
//\ loc_1011BC53:
		"L__MSASMLABEL_.${:uid}loc_1011BC53:\n\t"
		"mov eax, fs:[0x30]\n\t"
		"mov eax, [eax+8h]\n\t"
		"jmp L__MSASMLABEL_.${:uid}loc_1011BBDB\n\t"
//\ loc_1018E8A0:
		"L__MSASMLABEL_.${:uid}loc_1018E8A0:\n\t"
		"mov eax, fs:[0x30]\n\t"
		"mov eax, [eax+8h]\n\t"
		"jmp L__MSASMLABEL_.${:uid}loc_1011BC38\n\t"
//\ Return BB:
		"L__MSASMLABEL_.${:uid}loc_Return_BB:\n\t"
		"nop\n\t";

		string AsmGetProcAddressForCallerStr1 =
			"pop dword ptr $0\n\t"
			"push eax\n\t"
//\ LdrGetProcedureAddressForCaller
			"mov eax,$1\n\tmov eax, [eax]\n\tcall eax\n\t"
			"test eax, eax\n\t"
			"js L__MSASMLABEL_.${:uid}loc_1011BC48\n\t"
			"test ebx, ebx\n\t"
			"jz L__MSASMLABEL_.${:uid}loc_1018E8AE\n\t"
			"mov al, bl\n\t"
			"and al, 3\n\t"
			"movzx eax, al\n\t"
			"neg eax\n\t"
			"sbb eax, eax\n\t"
			"not eax\n\t"
			"and eax, ebx\n\t"
//\ loc_1011BC0C:
			"L__MSASMLABEL_.${:uid}loc_1011BC0C:\n\t"
			"cmp [ebp-4h], eax\n\t"
			"jz L__MSASMLABEL_.${:uid}loc_1018E8BC\n\t"
			"mov eax, [ebp-4h]\n\t"
//\ loc_1011BC18:
			"L__MSASMLABEL_.${:uid}loc_1011BC18:\n\t"
			"pop edi\n\t"
			"pop ebx\n\t"
			"push dword ptr $2\n\t"
			"retn\n\t"
//\ loc_1018E8AE:
			"L__MSASMLABEL_.${:uid}loc_1018E8AE:\n\t"
			"mov eax, fs:[0x30]\n\t"
			"mov eax, [eax+8h]\n\t"
			"jmp L__MSASMLABEL_.${:uid}loc_1011BC0C\n\t"
//\ loc_1018E8BC:
			"L__MSASMLABEL_.${:uid}loc_1018E8BC:\n\t"
			"cmp edi, [ebp+0ch]\n\t"
			"sbb ecx, ecx\n\t"
			"neg ecx\n\t"
			"add ecx, 0C0000138h\n\t"
			"jmp L__MSASMLABEL_.${:uid}loc_1011BC4A\n\t"
//\ loc_1011BC48:
			"L__MSASMLABEL_.${:uid}loc_1011BC48:\n\t"
			"mov ecx, eax\n\t"
//\ loc_1011BC4A:
			"L__MSASMLABEL_.${:uid}loc_1011BC4A:\n\t"
//\ sub_1011F120
			"mov eax, $3\n\tcall eax\n\t"
			"xor eax, eax\n\t"
			"jmp L__MSASMLABEL_.${:uid}loc_1011BC18";

	BOOL GetKernel32AddrtoGlobal();
	BOOL RelocFuncAddr1(Instruction* InsertBefore, CallInst* CallRelocInst);
	BOOL RelocFuncAddr(Instruction* InsertBefore, CallInst* CallRelocInst);
	BOOL CreateAsmApi();
	BOOL TransformArg();
	BOOL FindArgv();
	BOOL ProcessCallInst();
	BOOL CreateAsmApi_GetProcAddressForCaller();
	BOOL CreateAsmApi_RtlInitString();
	BOOL CreateHook();
	BOOL CreateTrampolineFunc();
};


