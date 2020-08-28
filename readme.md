******************************************************
这个库需要依赖3rd里面的luajit库，与标准luajit库有区别

编译方法：
首先先编译luajit库
1, cd tsg_lua_module/3rd/LuaJIT-2.1
2，make

然后编译tsglua
1，cd tsg_lua_module/src
2, mkdir build
3, cd build && cmake ..
4, make
5, cp libtsglua.* ../../lib

