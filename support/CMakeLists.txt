# CMakeFiles for 3rd vendor library
include(ExternalProject)

### luajit-2.1
set(LUAJIT2_ROOT            ${CMAKE_CURRENT_BINARY_DIR})
set(LUAJIT2_URL             ${CMAKE_CURRENT_SOURCE_DIR}/luajit2-2.1-agentzh.zip)
set(LUAJIT2_URL_MD5         37c03c469aa461ec44a0e4cebf1ceb52)
set(LUAJIT2_MAKE            cd ${LUAJIT2_ROOT}/luajit2/src/luajit2 && make BUILD_STATIC_LIB=${BUILD_STATIC_LIB}) 
set(LUAJIT2_INSTALL         cd ${LUAJIT2_ROOT}/luajit2/src/luajit2 && make install PREFIX=${LUAJIT_INSTALL_PREFIX})

ExternalProject_Add(luajit2
        PREFIX              luajit2
        URL                 ${LUAJIT2_URL}
        URL_MD5             ${LUAJIT2_URL_MD5}
        #CONFIGURE_COMMAND   ${LUAJIT2_CONFIGURE}
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ${LUAJIT2_MAKE}
        INSTALL_COMMAND     ${LUAJIT2_INSTALL}
        BUILD_IN_SOURCE     1
        )


add_library(luajit2-static STATIC IMPORTED GLOBAL)
add_dependencies(luajit2-static luajit2)
set_property(TARGET luajit2-static PROPERTY IMPORTED_LOCATION ${LUAJIT_INSTALL_PREFIX}/lib/libluajit-5.1.a)

#### GoogleTest
ExternalProject_Add(GoogleTest 
        PREFIX GoogleTest
        URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-release-1.8.0.tar.gz
        URL_MD5 16877098823401d1bf2ed7891d7dce36
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=ON)

ExternalProject_Get_Property(GoogleTest INSTALL_DIR)
file(MAKE_DIRECTORY ${INSTALL_DIR}/include)
add_library(gtest SHARED IMPORTED GLOBAL)
add_dependencies(gtest GoogleTest)
set_property(TARGET gtest PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libgtest.so)
#set_property(TARGET gtest PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libgtest_main.so)
set_property(TARGET gtest PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)
set_property(TARGET gtest PROPERTY INTERFACE_LINK_LIBRARIES pthread)
