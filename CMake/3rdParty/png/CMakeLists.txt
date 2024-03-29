PROJECT(png C)

SET(PNG_SRCS
  png.h  pnglibconf.h pngconf.h  pngdebug.h  pnginfo.h  pngpriv.h  pngstruct.h
  png.c pngerror.c  pngget.c
  pngmem.c  pngpread.c  pngread.c  pngrio.c  pngrtran.c  pngrutil.c  pngset.c
  pngtrans.c  pngwio.c  pngwrite.c  pngwtran.c  pngwutil.c	   
  )

IF (WIN32)
  ADD_DEFINITIONS(-DPNG_STATIC -D_CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32)

IF(NOT WITH_LOCAL_LIBPNG)
   FIND_PACKAGE(PNG)
ENDIF(NOT WITH_LOCAL_LIBPNG)
IF(NOT PNG_FOUND)
   IF(NOT WITH_LOCAL_ZLIB)
       FIND_PACKAGE(ZLIB)
   ENDIF(NOT WITH_LOCAL_ZLIB)
   IF(NOT ZLIB_FOUND)
       SET(ZLIB_INCLUDE_DIRS ../zlib)
       SET(ZLIB_LIBRARIES zlib)
   ENDIF(NOT ZLIB_FOUND)
   ADD_LIBRARY(png ${PNG_SRCS})
   INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
   TARGET_LINK_LIBRARIES(png ${ZLIB_LIBRARIES})
ENDIF(NOT PNG_FOUND)

SET(MISC LICENSE README README.ipol)
INSTALL(FILES CMakeLists.txt ${MISC} ${PNG_SRCS}
        DESTINATION src/3rdParty/png)
