#include "OAPI.h"

int main(int argc,char** argv)
{
	std::string OriginalFile = argv[1];
	std::string TransformedFile = argv[2];
	Module* Mod = MyParseIRFile(OriginalFile);
	if (Mod == nullptr)
	{
		errs() << "Failed to read input file\n";
		return -1;
	}

	AsmGetProcAddress doAsmGetProcAddress(Mod);
	doAsmGetProcAddress.ObfGetProcAddress();

	AsmMoveFileWithProgressTransactedA doAsmMoveFileWithProgressTransactedA(Mod);
	doAsmMoveFileWithProgressTransactedA.obfMoveFileWithProgressTransactedA();

	AsmMoveFileA doAsmMoveFileA(Mod);
	doAsmMoveFileA.ObfMoveFileA();

	AsmReadFile doAsmReadFile(Mod);
	doAsmReadFile.AsmReadFileEntry();

	DynamicStolenAPI doDynamicStolenAPI(Mod);
	doDynamicStolenAPI.DynamicStolenAPIEntry();
	if (!doWriteBackLL(Mod, TransformedFile))
	{
		errs() << "Faile to Write IR to file...\n";
		return -1;
	}
	return 0;
}

