include(ExternalProject)

set(LUAJIT2_ROOT            ${CMAKE_CURRENT_BINARY_DIR})
set(LUAJIT2_URL             ${CMAKE_CURRENT_SOURCE_DIR}/luajit2-2.1-agentzh.zip)
set(LUAJIT2_URL_MD5         6e17cd2263473f9b85b47d0736cf12da)
set(LUAJIT2_MAKE            cd ${LUAJIT2_ROOT}/luajit2/src/luajit2 && make) 
set(LUAJIT2_INSTALL         cd ${LUAJIT2_ROOT}/luajit2/src/luajit2 && make install PREFIX=${LUAJIT2_ROOT}/install)

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
set_property(TARGET luajit2-static PROPERTY IMPORTED_LOCATION ${LUAJIT2_ROOT}/install/lib/libluajit-5.1.a)

