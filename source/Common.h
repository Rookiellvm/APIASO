#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <iosfwd>
#include "llvm-c/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Pass.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Obfuscation/CryptoUtils.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Transforms/Obfuscation/Utils.h"
#include "llvm/Support/raw_ostream.h"
#include <sstream>
#include "llvm/IR/Module.h"
#include <set>
#include <string>
#include <Windows.h>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Obfuscation/Hello.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/Casting.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalValue.h" 
#include "llvm/Transforms/Obfuscation/Flattening.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Obfuscation/CryptoUtils.h"
#include "llvm/Support/Allocator.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Type.h"

using namespace llvm;
using namespace std;

Function* GetFunction(Module* Mod, StringRef Name);
CallInst* InsertAsmCode(string AsmString, string Constraints,
	vector<Value*> Para, Instruction* InsertBefore);
ReturnInst* MyCreateFunc(Module* Mod, string MyFuncName);
BOOL MySetFunctionName(Module* Mod, string OriginName, string NewName);
char* getfileAll(const char *fname);