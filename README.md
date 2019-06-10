使用方法：<br>
1. 编辑zcp_tool/Inc.mk文件，找到PROJ_PATH变量，需要赋值为zcp_tool工程在linux中的绝对路径<br>
2. 到zcp_tool目录下，先make install编译安装dep下的第三方库，再make编译自己的代码<br>

使用说明：<br>
1. zcp_tool/Inc.mk中有通用的自动计算目标文件生成的规则，主要是把.c/.cpp等文件生成.o<br>
2. 源文件文件夹中的Makefile必须include ../Inc.mk，因为需要自动计算目标文件生成的规则，必须赋值INCLUDE和LDFLAGS，INCLUDE是给Inc.mk中目标文件生成使用的，LDFLAGS是给本Makefile生成可执行文件使用的，两个变量需要根据自己引用的头文件和库文件而定<br>
3. 所有的可执行程序都生成在zcp_tool/bin目录，所有的库文件都生成在zcp_tool/lib目录，zcp_tool/obj文件夹可选，在源文件文件夹找那个的Makefile中，OBJS变量前面加上了OBJ_DIR，那么就选择生成在zcp_tool/obj<br>
