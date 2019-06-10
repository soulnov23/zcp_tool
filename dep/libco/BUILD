cc_library(
   name = 'colib',
   target_bases = [ 'comm',],
   srcs = [
        'co_hook_sys_call.cpp',
        'co_routine.cpp',
		'co_routine_specific.cpp',
		'coctx_mov.cpp',
		'coctx_swap_mov.S',
		'co_aio.cpp',
		'co_routine_pool.cpp',
    'co_comm.cpp',
   ],
   deps = [
   '#aio',
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
#'-O2' , 
        #'-Werror',
		'--std=c++11',
		'-g',
    ],
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
		'//comm2/core:core',

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
		'//comm2/core:core',

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
		'//comm2/core:core',

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
    name = 'example_echosvr',
    srcs = [ 
        'example_echosvr.cpp',

    ],
    deps = [ 
		':colib',
		'//comm2/core:core',

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
    name = 'example_co_pool',
    srcs = [ 
        'example_co_pool.cpp',

    ],
    deps = [ 
		':colib',
		'//comm2/core:core',

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
    name = 'example_pending',
    srcs = [ 
        'example_pending.cpp',

    ],
    deps = [ 
		':colib',
		'//comm2/core:core',

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
    name = 'example_auto_free',
    srcs = [ 
        'example_auto_free.cpp',

    ],
    deps = [ 
		':colib',
		'//comm2/core:core',

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
    name = 'example_test',
    srcs = [ 
        'example_test.cpp',

    ],
    deps = [ 
		':colib',
		'//comm2/core:core',

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
    name = 'example_gethostbyname',
    srcs = [ 
        'example_gethostbyname.cpp',

    ],
    deps = [ 
		':colib',
		'//comm2/core:core',

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
