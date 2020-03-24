# 使用方法：
1. 到zcp_tool目录下，先make install编译安装dep下的第三方库，再make编译自己的代码

# 使用说明：
1. zcp_tool/Inc.mk中有通用的自动计算目标文件生成的规则，主要是把.c/.cc/.cpp等文件生成.o

2. 源文件文件夹中的Makefile必须include ../Inc.mk，因为需要自动计算目标文件生成的规则，必须赋值INCLUDE和LDFLAGS，INCLUDE是给Inc.mk中目标文件生成使用的，LDFLAGS是给本Makefile生成可执行文件使用的，两个变量需要根据自己引用的头文件和库文件而定

3. 所有的可执行程序都生成在zcp_tool/bin目录，所有的库文件都生成在zcp_tool/lib目录，zcp_tool/obj文件夹可选，在源文件文件夹找那个的Makefile中，OBJS变量前面加上了OBJ_DIR，那么就选择生成在zcp_tool/obj

```
Some influential environment variables:
  CC          C compiler command
  CFLAGS      C compiler flags
  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
              nonstandard directory <lib dir>
  LIBS        libraries to pass to the linker, e.g. -l<library>
  CPPFLAGS    (Objective) C/C++ preprocessor flags, e.g. -I<include dir> if
              you have headers in a nonstandard directory <include dir>
  CXX         C++ compiler command
  CXXFLAGS    C++ compiler flags
  build_configargs
              additional configure arguments for build directories
  host_configargs
              additional configure arguments for host directories
  target_configargs
              additional configure arguments for target directories
  AR          AR for the host
  AS          AS for the host
  DLLTOOL     DLLTOOL for the host
  LD          LD for the host
  LIPO        LIPO for the host
  NM          NM for the host
  RANLIB      RANLIB for the host
  STRIP       STRIP for the host
  WINDRES     WINDRES for the host
  WINDMC      WINDMC for the host
  OBJCOPY     OBJCOPY for the host
  OBJDUMP     OBJDUMP for the host
  OTOOL       OTOOL for the host
  READELF     READELF for the host
  CC_FOR_TARGET
              CC for the target
  CXX_FOR_TARGET
              CXX for the target
  GCC_FOR_TARGET
              GCC for the target
  GFORTRAN_FOR_TARGET
              GFORTRAN for the target
  GOC_FOR_TARGET
              GOC for the target
  GDC_FOR_TARGET
              GDC for the target
  AR_FOR_TARGET
              AR for the target
  AS_FOR_TARGET
              AS for the target
  DLLTOOL_FOR_TARGET
              DLLTOOL for the target
  LD_FOR_TARGET
              LD for the target
  LIPO_FOR_TARGET
              LIPO for the target
  NM_FOR_TARGET
              NM for the target
  OBJCOPY_FOR_TARGET
              OBJCOPY for the target
  OBJDUMP_FOR_TARGET
              OBJDUMP for the target
  OTOOL_FOR_TARGET
              OTOOL for the target
  RANLIB_FOR_TARGET
              RANLIB for the target
  READELF_FOR_TARGET
              READELF for the target
  STRIP_FOR_TARGET
              STRIP for the target
  WINDRES_FOR_TARGET
              WINDRES for the target
  WINDMC_FOR_TARGET
              WINDMC for the target
```