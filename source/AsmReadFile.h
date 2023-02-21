#pragma once
#include "Common.h"


typedef struct _RelocData
{
	string FunName;
	string DLLName;
	string AsmString;
	string AsmConstraints;
	string Data_Global_Name;
	vector<string> DirectRelyAsmApi;
	vector<string> IndirectRelyAsmApi;
	vector<pair<DWORD,GlobalVariable*>> Data_VA_RVA; 
	CallInst* ImageBaseCallInst;          
	Instruction* AsmApiRetInst;
	BOOL isOutApi;
}RelocDataStruct;

typedef struct _DLLImageBase
{
	CallInst* Kernel32ImageCallInst;
	CallInst* KernelBaseImageCallInst;
	CallInst* NtdllImageCallInst;
}DLLImageBaseStruct;

class AsmReadFile
{
public:
	AsmReadFile(Module* Mod);
	BOOL AsmReadFileEntry();
protected:
	Module* Mod;
	CallInst* OriginApiCallInst;
	vector<RelocDataStruct> vec_RelocDataStruct;
	vector<Value*> OriginApiPara;
	DLLImageBaseStruct ImageBaseStruct;

	//\ My Function
	BOOL GetDLLImageBase();
	BOOL FillDataStruct();
	BOOL RelocDataAddr();
	BOOL CreateAsmApi();
	BOOL AsmStr2AsmApi();
	BOOL InsertHook();

	BOOL SubReturnValue();
	BOOL PushApiArg(string OriginApiName);
	BOOL UpdataFunAddr(GlobalVariable* OldGlobal, Function* AsmFunc);
	BOOL HookApi2AsmApi(string InduceApiName, string AsmApiName);
	BOOL CreateTrampoline_0(string AsmApiName, string InduceApiName);
	BOOL CreateHook(string TrampolineFuncName, string InduceApiName);
	BOOL CallInduceApi(string InduceApiName, Instruction* InsertBefore);
	RelocDataStruct ReadJson2str(const char* JsonFileName, string FunctionName);

	//char* getfileAll(const char *fname);
	string AsmReadFileStr_Kernel32 =
		"mov eax, $0\n\tmov eax,[eax]\n\tjmp eax";

	string AsmZwReadFileStr_Ntdll =
		"mov  eax,1A0006h\n\t"
//\  mov     edx, offset j_Wow64Transition
		"mov  edx, $0\n\t"
		"call edx\n\t"
		"retn 24h\n\t";

	string AsmReadFileStr_KernelBase =
		"mov     edi, edi\n\t"
		"push    ebp\n\t"
		"mov     ebp, esp\n\t"
		"push    0FFFFFFFEh\n\t"
		//\       stru_101C5B60   ____ $0
		"mov     eax, $0\n\tpush     eax\n\t"
		//\       __except_handler4   ____ $1
		"mov     eax, $1\n\tpush     eax\n\t"
		"mov     eax,  dword ptr fs:[0]\n\t"
		"push    eax\n\t"
		"sub     esp, 18h\n\t"
		"push    ebx\n\t"
		"push    esi\n\t"
		"push    edi\n\t"
		//\      __security_cookie   ____ $2
		"mov     eax, $2\n\tmov     eax, [eax]\n\t"
		"xor     dword ptr [ebp-8h], eax\n\t"
		"xor     eax, ebp\n\t"
		"push    eax\n\t"
		"lea     eax, [ebp-10h]\n\t"
		"mov     dword ptr fs:[0], eax\n\t"
		"mov     dword ptr [ebp-18h], esp\n\t"
		"mov     dword ptr [ebp-20h], 0h\n\t"
		"mov     dword ptr [ebp-1ch], 0h\n\t"
		"mov     esi, [ebp+14h]\n\t"
		"test    esi, esi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10111160\n\t"
		"mov     dword ptr [esi], 0\n\t"
		//\      loc_10111160:
		"L__MSASMLABEL_.${:uid}loc_10111160:\n\t"
		"mov     ebx, [ebp+8h]\n\t"
		"cmp     ebx, 0FFFFFFF4h\n\t"
		"jnb     L__MSASMLABEL_.${:uid}loc_10141C17\n\t"
		//\      loc_1011116C:
		"L__MSASMLABEL_.${:uid}loc_1011116C:\n\t"
		"mov     edi, [ebp+18h]\n\t"
		"test    edi, edi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_101111E1\n\t"
		"mov     dword ptr [edi], 103h\n\t"
		"mov     eax, [edi+8]\n\t"
		"mov     dword ptr [ebp-28h], eax\n\t"
		"mov     eax, [edi+0Ch]\n\t"
		"mov     dword ptr [ebp-24h], eax\n\t"
		"mov     eax, [edi+10h]\n\t"
		"test    al, 1\n\t"
		"jnz     L__MSASMLABEL_.${:uid}loc_10141C5A\n\t"
		"mov     ecx, edi\n\t"
		//\      loc_10111192:
		"L__MSASMLABEL_.${:uid}loc_10111192:\n\t"
		"push    0\n\t"
		"lea     edx, [ebp-28h]\n\t"
		"push    edx\n\t"
		"push    [ebp+10h]\n\t"
		"push    [ebp+0Ch]\n\t"
		"push    edi\n\t"
		"push    ecx\n\t"
		"push    0\n\t"
		"push    eax\n\t"
		"push    ebx\n\t"
		//\      __imp__NtReadFile@36   ____ $3
		"mov     eax, $3\n\t"
		//"mov     eax, [eax]\n\t"
		"call     eax\n\t"
		"test    eax, eax\n\t"
		"js     L__MSASMLABEL_.${:uid}loc_101111B9\n\t"
		"cmp     eax, 103h\n\t"
		"jnz     L__MSASMLABEL_.${:uid}loc_10111244\n\t"
		//\      loc_101111B9:
		"L__MSASMLABEL_.${:uid}loc_101111B9:\n\t"
		"cmp     eax, 0C0000011h\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10111274\n\t"
		"mov     ecx, eax\n\t"
		//\      loc_101111C6:
		"L__MSASMLABEL_.${:uid}loc_101111C6:\n\t"
		//\       _BaseSetLastNTError@4   ____ $4
		"mov     eax, $4\n\tcall    eax\n\t"
		"xor     eax, eax\n\t"
		"mov     ecx, [ebp-10h]\n\t"
		"mov     dword ptr fs:[0], ecx\n\t"
		"pop     ecx\n\t"
		"pop     edi\n\t"
		"pop     esi\n\t"
		"pop     ebx\n\t"
		"mov     esp, ebp\n\t"
		"pop     ebp\n\t"
		"retn    14h\n\t"
		//\      loc_101111E1:
		"L__MSASMLABEL_.${:uid}loc_101111E1:\n\t"
		"push    0\n\t"
		"push    0\n\t"
		"push    [ebp+10h]\n\t"
		"push    [ebp+0Ch]\n\t"
		"lea     eax, [ebp-20h]\n\t"
		"push    eax\n\t"
		"push    0\n\t"
		"push    0\n\t"
		"push    0\n\t"
		"push    ebx\n\t"
		//\      __imp__NtReadFile@36   ____ $3
		"mov     eax, $3\n\t"
		//"mov     eax, [eax]\n\t"
		"call    eax\n\t"
		"mov     ecx, eax\n\t"
		"cmp     ecx, 103h\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10141C8C\n\t"
		//\      loc_1011120A:
		"L__MSASMLABEL_.${:uid}loc_1011120A:\n\t"
		"test    ecx, ecx\n\t"
		"js     L__MSASMLABEL_.${:uid}loc_10111230\n\t"
		"test    esi, esi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10111217\n\t"
		"mov     eax, [ebp-1ch]\n\t"
		"mov     [esi], eax\n\t"
		//\      loc_10111217:
		"L__MSASMLABEL_.${:uid}loc_10111217:\n\t"
		"mov     eax, 1\n\t"
		"mov     ecx, [ebp-10h]\n\t"
		"mov     dword ptr fs:[0], ecx\n\t"
		"pop     ecx\n\t"
		"pop     edi\n\t"
		"pop     esi\n\t"
		"pop     ebx\n\t"
		"mov     esp, ebp\n\t"
		"pop     ebp\n\t"
		"retn    14h\n\t"
		//\      loc_10111230:
		"L__MSASMLABEL_.${:uid}loc_10111230:\n\t"
		"cmp     ecx, 0C0000011h\n\t"
		"jnz     L__MSASMLABEL_.${:uid}loc_1011125D\n\t"
		"test    esi, esi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10111217\n\t"
		"mov     dword ptr [esi], 0\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_10111217\n\t"
		//\      loc_10111244:
		"L__MSASMLABEL_.${:uid}loc_10111244:\n\t"
		"test    esi, esi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10111217\n\t"
		"mov     dword ptr [ebp-4h], 0\n\t"
		"mov      eax, [edi+4]\n\t"
		"mov     [esi], eax\n\t"
		"mov     dword ptr [ebp-4h], 0FFFFFFFEh\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_10111217\n\t"
		//\      loc_1011125D:
		"L__MSASMLABEL_.${:uid}loc_1011125D:\n\t"
		"mov     eax, ecx\n\t"
		"and     eax, 0C0000000h\n\t"
		"cmp     eax, 80000000h\n\t"
		"jnz     L__MSASMLABEL_.${:uid}loc_101111C6\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_10141CA9\n\t"
		//\      loc_10111274:
		"L__MSASMLABEL_.${:uid}loc_10111274:\n\t"
		"test    esi, esi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_1011127E\n\t"
		"mov     dword ptr [esi], 0\n\t"
		//\      loc_1011127E:
		"L__MSASMLABEL_.${:uid}loc_1011127E:\n\t"
		"mov     ecx, 0C0000011h\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_101111C6\n\t"
		//\      loc_10141C17:
		"L__MSASMLABEL_.${:uid}loc_10141C17:\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10141C49\n\t"
		"cmp     ebx, 0FFFFFFF5h\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_10141C38\n\t"
		"cmp     ebx, 0FFFFFFF6h\n\t"
		"jnz     L__MSASMLABEL_.${:uid}loc_1011116C\n\t"
		"mov     eax, dword ptr fs:[0x30]\n\t"
		"mov     eax, [eax+10h]\n\t"
		"mov     ebx, [eax+18h]\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_1011116C\n\t"
		//\      loc_10141C38:
		"L__MSASMLABEL_.${:uid}loc_10141C38:\n\t"
		"mov     eax, dword ptr fs:[0x30]\n\t"
		"mov     eax, [eax+10h]\n\t"
		"mov     ebx, [eax+1Ch]\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_1011116C\n\t"
		//\      loc_10141C49:
		"L__MSASMLABEL_.${:uid}loc_10141C49:\n\t"
		"mov     eax, dword ptr fs:[0x30]\n\t"
		"mov     eax, [eax+10h]\n\t"
		"mov     ebx, [eax+20h]\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_1011116C\n\t"
		//\      loc_10141C5A:
		"L__MSASMLABEL_.${:uid}loc_10141C5A:\n\t"
		"xor     ecx, ecx\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_10111192\n\t"
		/*
				//\      loc_10141C61:
				"L__MSASMLABEL_.${:uid}loc_10141C61:\n\t"
				"mov     eax, [ebp-14h]\n\t"
				"mov     eax, [eax]\n\t"
				"xor     ecx, ecx\n\t"
				"cmp     dword ptr [eax], 0C00000FDh\n\t"
				"setnz   cl\n\t"
				"mov     eax, ecx\n\t"
				"retn\n\t"
				//\      loc_10141C74:
				"L__MSASMLABEL_.${:uid}loc_10141C74:\n\t"
				"mov     esp, [ebp-18h]\n\t"
				"mov     eax, [ebp+14h]\n\t"
				"mov     dword ptr [eax], 0h\n\t"
				"mov     dword ptr [ebp-4h], 0FFFFFFFEh\n\t"
				"jmp     L__MSASMLABEL_.${:uid}loc_10111217\n\t"
		*/
		//\      loc_10141C8C:
		"L__MSASMLABEL_.${:uid}loc_10141C8C:\n\t"
		"push    0\n\t"
		"push    0\n\t"
		"push    ebx\n\t"
		//\      __imp__NtWaitForSingleObject@12   ____ $5
		"mov     eax, $5\n\tmov     eax, [eax]\n\tcall     eax\n\t"
		"mov     ecx, eax\n\t"
		"test    ecx, ecx\n\t"
		"js     L__MSASMLABEL_.${:uid}loc_10111230\n\t"
		"mov     ecx, dword ptr [ebp-20h]\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_1011120A\n\t"
		//\      loc_10141CA9:
		"L__MSASMLABEL_.${:uid}loc_10141CA9:\n\t"
		"test    esi, esi\n\t"
		"jz     L__MSASMLABEL_.${:uid}loc_101111C6\n\t"
		"mov     eax, [ebp-1ch]\n\t"
		"mov     [esi], eax\n\t"
		"jmp     L__MSASMLABEL_.${:uid}loc_101111C6\n\t";
};

