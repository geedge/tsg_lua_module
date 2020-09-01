find_path(TCMALLOC_INCLUDE_DIR tcmalloc.h /usr/include /usr/include/gperftools /usr/local/include /usr/local/include/gperftools )
find_library(TCMALLOC_LIBRARY NAMES tcmalloc PATHS /usr/lib /usr/lib/gperftools /usr/local/lib /usr/local/lib/gperftools)

if (TCMALLOC_INCLUDE_DIR AND TCMALLOC_LIBRARY)
    set(tcmalloc_FOUND TRUE)
endif (TCMALLOC_INCLUDE_DIR AND TCMALLOC_LIBRARY)
