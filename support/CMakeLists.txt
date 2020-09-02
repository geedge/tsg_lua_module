include(ExternalProject)

set(LUAJIT2_ROOT            ${CMAKE_CURRENT_SOURCE_DIR})
set(LUAJIT2_URL             ${CMAKE_CURRENT_SOURCE_DIR}/luajit2-2.1-agentzh.zip)
set(LUAJIT2_URL_MD5         605edb76eb1ff6b9364afe7fef58c83e)
#set(LUAJIT2_CONFIGURE       )
set(LUAJIT2_MAKE            cd ${LUAJIT2_ROOT}/src/luajit2 && make) 
set(LUAJIT2_INSTALL         cd ${LUAJIT2_ROOT}/src/luajit2 && make install PREFIX=${LUAJIT2_ROOT}/install)

ExternalProject_Add(luajit2
        PREFIX              luajit2
        URL                 ${LUAJIT2_URL}
        URL_MD5             ${LUAJIT2_URL_MD5}
        #CONFIGURE_COMMAND   ${LUAJIT2_CONFIGURE}
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ${LUAJIT2_MAKE}
        INSTALL_COMMAND     ${LUAJIT2_INSTALL}
        )

ExternalProject_Get_Property(luajit2 INSTALL_DIR)
file(MAKE_DIRECTORY ${INSTALL_DIR}/include)

add_library(luajit2-static STATIC IMPORTED GLOBAL)
add_dependencies(luajit2-static luajit2)
set_property(TARGET luajit2-static PROPERTY IMPORTED_LOCATION ${INSTALL_DIR}/lib/libluajit-5.1.a)
set_property(TARGET luajit2-static PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${INSTALL_DIR}/include)



