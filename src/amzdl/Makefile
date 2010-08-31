PROG = amzdl${PROG_SUFFIX}
SRCS = amzdl.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../libamz ${GLIB_CFLAGS} ${SOUP_CFLAGS}
LIBS += -L../libamz -lamz ${GLIB_LIBS} ${SOUP_LIBS}
