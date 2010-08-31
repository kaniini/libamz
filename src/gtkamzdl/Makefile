PROG = gtkamzdl${PROG_SUFFIX}
SRCS = gtkamzdl.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../libamz ${GLIB_CFLAGS} ${GTK_CFLAGS} ${SOUP_CFLAGS}
LIBS += -L../libamz -lamz ${GLIB_LIBS} ${GTK_LIBS} ${SOUP_LIBS}
