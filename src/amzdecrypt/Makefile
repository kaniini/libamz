PROG = amzdecrypt${PROG_SUFFIX}
SRCS = amzdecrypt.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../libamz ${GLIB_CFLAGS} ${SOUP_CFLAGS}
LIBS += -L../libamz -lamz ${GLIB_LIBS} ${SOUP_LIBS}
