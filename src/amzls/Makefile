PROG = amzls${PROG_SUFFIX}
SRCS = amzls.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += -I../libamz ${GLIB_CFLAGS} ${SOUP_CFLAGS}
LIBS += -L../libamz -lamz ${GLIB_LIBS} ${SOUP_LIBS}
