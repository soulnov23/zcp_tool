### libco
libco是一个有趣的库,仅有的几个函数接口  co_create/co_resume/co_yield  再配合 co_poll，可以支持同步或者异步的写法，如线程库一样轻松。

库里面提供了socket族函数的hook，使得后台逻辑服务程序几乎不用修改逻辑代码就可以完成异步化改造。