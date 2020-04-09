cc_library(
   name = 'colib',
   srcs = [
      'co_hook_sys_call.cpp',
      'co_routine.cpp',
      'co_routine_specific.cpp',
      'coctx_mov.cpp',
      'coctx_swap_mov.S',
      'co_aio.cpp',
      'co_routine_pool.cpp',
      'co_comm.cpp',
      'sync/co_condition_variable.cpp',
      'sync/co_mutex.cpp',
   ],
   deps = [
      '#aio',
      '#dl',
      '#pthread',
   ],
   defs = [
      'LINUX',
      '_PTHREADS',
      'TIXML_USE_STL',
   ],
   optimize = [
      #'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   export_incs = [ '.', 'sync' ],
)

cc_binary(
   name = 'global_sym',
   srcs = [
      'demangle.cpp',
      'global_sym.cpp'
   ],
   deps = [
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   incs = [
      #INCS_POS__
   ],
)
cc_binary(
   name = 'test_specific',
   srcs = [
      'test_specific.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'echosvr',
   srcs = [
      'example_echosvr_copystack.cpp',

   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'test_timeout',
   srcs = [
      'test_timeout.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'example_echosvr',
   srcs = [
      'example_echosvr.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'example_co_pool',
   srcs = [
      'example_co_pool.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'example_pending',
   srcs = [
      'example_pending.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'example_auto_free',
   srcs = [
      'example_auto_free.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
   name = 'example_gethostbyname',
   srcs = [
      'example_gethostbyname.cpp',
   ],
   deps = [
      ':colib',
      #GEN_SOURCE_POS__
   ],
   defs = [
      'LINUX',
      '_GNU_SOURCE',
      '_REENTRANT',
   ],
   optimize = [
      'O2',
   ],
   extra_cppflags = [
      '--std=c++11',
   ],
   incs = [
      #INCS_POS__
   ],
)

cc_binary(
    name = 'example_co_mutex',
    srcs = [ 
      'example_co_mutex.cpp',
    ],
    deps = [ 
		  ':colib',
#GEN_SOURCE_POS__
    ],
    defs = [
		  'LINUX',
      '_PTHREADS',
      'TIXML_USE_STL',
      '_NEW_LIC',
      '_GNU_SOURCE',
      '_REENTRANT',
    ],
	  optimize = [
      'O2',
    ],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
        '-pipe',
        '-fPIC',
        '-Wno-deprecated',
		    '--std=c++11',
    ],
	incs = [ 
#INCS_POS__
	],
)

cc_binary(
    name = 'example_cv',
    srcs = [ 
      'example_condition_variable.cpp',
    ],
    deps = [ 
		  ':colib',
#GEN_SOURCE_POS__
    ],
    defs = [
		  'LINUX',
      '_PTHREADS',
      'TIXML_USE_STL',
      '_NEW_LIC',
      '_GNU_SOURCE',
      '_REENTRANT',
    ],
	  optimize = [
      'O2',
    ],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
        '-pipe',
        '-fPIC',
        '-Wno-deprecated',
		    '--std=c++11',
    ],
	incs = [ 
#INCS_POS__
	],
)
