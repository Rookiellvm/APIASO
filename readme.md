# Othread Obfuscator

OCalling is an obfuscation tool based on LLVM, which can perfectly realize the obfuscation of C/C++ code under the Windows platform of x86 architecture.

## 0x01 Principle

We propose a kind of obfuscation idea based on the fuzzy relationship of function call, which makes the function inline and outline as the basis, combined with the program semantics to randomly generate the function call graph. Experimental results show that the application of the obfuscated algorithm program can effectively obfuscate the function call graph, and at the same time has a low impact on the efficiency of program execution.

## 0x02 How to use

1. Compile the source code and ".\source\*.cpp" into LLVM intermedia representation
2. Utilize llvm-link to merge all intermedia representation files into one
3. Input the file generated by step2, ".\obfuscator\othread.exe" will output obfuscated intermedia representation
4. Link the file output by step3 to generate an executable file

Usage: `OCalling.exe ..\example\aes\aes.bc ..\example\aes\aes-obf.bc`

## 0x03 Example

1. aes		symmetric cryptographic algorithm	`Usage:aes.exe`
2. rsa		asymmetric cryptographic algorithm	`Usage:rsa.exe`
3. des      asymmetric cryptographic algorithm  `Usage:des.exe -e keyfie.key  sample.txt sample.enc`
4. rc4		asymmetric cryptographic algorithm	`Usage:rc4.exe`
5. sha      asymmetric cryptographic algorithm	`Usage:sha.exe`
6. bzip		Lexical analyzer					`Usage:bzip.exe ./input/input.random`
7. gzip		Compression algorithm				`Usage:gzip.exe ./input/input.combined`
8. twoif	Simulated annealing algorithm		`Usage:twoif.exe ./input/test`


*Table1 Program execution Cost before and after obfuscation*

| example | before(s) | after(s) | Cost    |
| ------- | --------- | -------- | ------- |
| rc4     | 0.253     | 0.265    | 1.05    |
| sha     | 0.243     | 0.277    | 1.08    |
| des     | 21.379    | 23.255   | 1.02    |
| rsa     | 0.302     | 0.852    | 1.04    |
| aes     | 0.128     | 0.447    | 1.05    |
| bzip    | 0.128     | 0.447    | 1.02    |
| gzip    | 0.128     | 0.447    | 1.22    |
| twoif   | 0.128     | 0.447    | 1.25    |

## 0x04 Screenshot

*Figure1.1 Function call relationship before and after obfuscation-aes*

![aes](img/TimeCost/aes.png)

*Figure1.2 Function call relationship before and after obfuscation-rsa*

![rsa](img/TimeCost/rsa.png)

*Figure1.3 Function call relationship before and after obfuscation-des*

![des](img/TimeCost/des.png)

*Figure1.4 Function call relationship before and after obfuscation-rc4*

![rc4](img/TimeCost/rc4.png)

*Figure1.5 Function call relationship before and after obfuscation-sha*

![sha](img/TimeCost/sha.png)

*Figure1.6 Function call relationship before and after obfuscation-bzip*

![bzip](img/TimeCost/bzip.png)

*Figure1.7 Function call relationship before and after obfuscation-gzip*

![gzip](img/TimeCost/gzip.png)

*Figure1.8 Function call relationship before and after obfuscation-twoif*

![twoif](img/TimeCost/twoif.png)

*Figure2.1 Program similarity before and after obfuscation-aes*

![aes](img/Bindiff/aes.png)

*Figure2.2 Program similarity before and after obfuscation-rsa*

![rsa](img/Bindiff/rsa.png)

*Figure2.3 Program similarity before and after obfuscation-des*

![des](img/Bindiff/des.png)

*Figure2.4 Program similarity before and after obfuscation-rc4*

![rc4](img/Bindiff/rc4.png)

*Figure2.5 Program similarity before and after obfuscation-sha*

![sha](img/Bindiff/sha.png)

*Figure2.6 Program similarity before and after obfuscation-bzip*

![bzip](img/Bindiff/bzip.png)

*Figure2.7 Program similarity before and after obfuscation-gzip*

![gzip](img/Bindiff/gzip.png)

*Figure2.8 Program similarity before and after obfuscation-twoif*

![twoif](img/Bindiff/twoif.png)

*Figure3.1 Comparison of call graphs before and after obfuscation-aes*

![aes](img/CallGraph/aes.png)

*Figure3.2 Comparison of call graphs before and after obfuscation-rsa*

![rsa](img/CallGraph/rsa.png)

*Figure3.3 Comparison of call graphs before and after obfuscation-des*

![des](img/CallGraph/des.png)

*Figure3.4 Comparison of call graphs before and after obfuscation-rc4*

![rc4](img/CallGraph/rc4.png)

*Figure3.5 Comparison of call graphs before and after obfuscation-sha*

![sha](img/CallGraph/sha.png)

*Figure3.6 Comparison of call graphs before and after obfuscation-bzip*

![bzip](img/CallGraph/bzip.png)

*Figure3.7 Comparison of call graphs before and after obfuscation-gzip*

![gzip](img/CallGraph/gzip.png)

*Figure3.8 Comparison of call graphs before and after obfuscation-twoif*

![twoif](img/CallGraph/twoif.png)



## 0x05 Paper

You can get our paper one week later