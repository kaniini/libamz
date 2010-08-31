LIB = ${LIB_PREFIX}amz${LIB_SUFFIX}
LIB_MAJOR = 1
LIB_MINOR = 0

SRCS = amzfile.c amzplaylist.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -DHAVE_CONFIG_H ${LIB_CPPFLAGS} ${CFLAGS} -I.. -I../..
CFLAGS += ${LIB_CFLAGS} ${GLIB_CFLAGS} ${LIBGCRYPT_CFLAGS} ${XML_CFLAGS}

LIBS += ${GLIB_LIBS} ${LIBGCRYPT_LIBS} ${XML_LIBS}

