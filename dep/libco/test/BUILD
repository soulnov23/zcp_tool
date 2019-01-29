cc_binary(
   name = 'test_colib',
    srcs = [ 
'test_colib.cpp',
 'test_xmm.S',
    ],
    deps = [ 
'//basic/colib:colib',
'//mm3rd/gtest:gtest',
'#dl',
#GEN_SOURCE_POS__
    ],
    defs = [
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
		'-DLIBCO-USE-GETTICK',
        '-g',
        '--std=c++11',

    ],
	incs = [ 
#INCS_POS__
	],
)

cc_binary(
    name = 'test_socket',
    srcs = [ 
     'test_socket.cpp',
	 'test_xmm.S',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'//mm3rd/gflags:gflags',
	'#dl',

#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
'-pipe',
'-std=c++11',
	'-ldl',
        '-fPIC',
        '-Wno-deprecated',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
    name = 'test_tick',
    srcs = [ 
     'test_tick.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'//mm3rd/gflags:gflags',
	'#dl',

#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
'-pipe',
'-std=c++11',
	'-ldl',
        '-fPIC',
        '-Wno-deprecated',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
   name = 'test_comm',
   srcs = [ 
		'test_comm.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
   name = 'test_rpc',
   srcs = [ 
		'test_rpc.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
   name = 'test_svr',
   srcs = [ 
		'test_svr.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
   name = 'echosvr',
   srcs = [ 
		'echosvr.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'//mm3rd/gflags:gflags',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
   name = 'echocli',
   srcs = [ 
		'echocli.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'//mm3rd/gflags:gflags',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)

cc_binary(
   name = 'test_buffer',
   srcs = [ 
		'test_buffer.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'//mm3rd/gflags:gflags',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)
cc_binary(
   name = 'test_signal',
   srcs = [ 
		'test_signal.cpp',
    ],
    deps = [ 
	'//basic/colib:colib',
	'//mm3rd/gtest:gtest',
	'//mm3rd/gflags:gflags',
	'#dl',
	#GEN_SOURCE_POS__
    ],
    defs = [
           ],
	optimize = [
#'O2',
		],
    extra_cppflags = [ 
        #'-Werror',
        #'-Werror',
		'-pipe',
		'-std=c++11',
		'-g',
        '-fPIC',
        '-Wno-deprecated',
		'-DLIBCO-USE-GETTICK',

    ],
	incs = [ 
#INCS_POS__
	],
)
