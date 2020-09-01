include(ExternalProject)

set(LUAJIT2_ROOT            ${CMAKE_CURRENT_SOURCE_DIR})
set(LUAJIT2_URL             ${CMAKE_CURRENT_SOURCE_DIR}/luajit2-2.1-agentzh.zip)
set(LUAJIT2_URL_MD5         605edb76eb1ff6b9364afe7fef58c83e)
#set(LUAJIT2_CONFIGURE       )
set(LUAJIT2_MAKE            cd ${LUAJIT2_ROOT}/src/luajit2 && make) 
set(LUAJIT2_INSTALL         cd ${LUAJIT2_ROOT}/src/luajit2 && make install PREFIX=${LUAJIT2_ROOT}/install)

ExternalProject_Add(luajit2
        PREFIX              ${LUAJIT2_ROOT}
        URL                 ${LUAJIT2_URL}
        URL_MD5             ${LUAJIT2_URL_MD5}
        #CONFIGURE_COMMAND   ${LUAJIT2_CONFIGURE}
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ${LUAJIT2_MAKE}
        INSTALL_COMMAND     ${LUAJIT2_INSTALL}
        )
