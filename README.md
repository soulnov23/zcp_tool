使用方法：<br>
1. 编辑zcp_tools/Inc.mk文件，找到PROJ_PATH变量，需要赋值为绝对路径。<br>
2. 在zcp_tools目录下，先make install安装dep下的第三方库，再make编译自己的代码。<br>

使用说明：<br>
1. zcp_tools/Inc.mk中自动计算目标文件生成的规则，其中INCLUDE变量，需要在源文件文件夹中自己赋值。<br>
2. 源文件文件夹中的Makefile只需要赋值INCLUDE和LDFLAGS，根绝自己需要的头文件和库文件而定。<br>
3. 所有的可执行程序都生成在zcp_tools/bin目录，所有的库文件都生成在zcp_tools/lib文件，obj文件夹可选，也可以让目标文件就在源文件文件夹。<br>
