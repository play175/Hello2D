#
# Hello2D Game Engine
# yoyo 2015 ShenZhen China
# repo:https://github.com/play175/Hello2D
# website:http://yoyo.play175.com
# MIT Licensed
#

APP = h2d
OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) 
OBJS += $(patsubst 3rdparty/%.c,3rdparty/%.o,$(wildcard 3rdparty/*.c)) 
OBJS += $(patsubst core/%.c,core/%.o,$(wildcard core/*.c)) 
OBJS += $(patsubst widget/%.c,widget/%.o,$(wildcard widget/*.c)) 

INCLUDE = -I.
INCLUDE += -I./3rdparty
INCLUDE += -I./core
INCLUDE += -I./widget
LIB = -L. -lgdi32

RM = rm
CC = gcc
CFLAGS = -std=gnu99
LDFLAGS = ${LIB} ${WARNS}
RC = windres

release : WARNS = -Wno-unknown-pragmas -Wl,--subsystem,windows
release : CFLAGS += -D NDEBUG
release : LDFLAGS += -O4 -s
release : ${APP}.exe
	strip $<
	upx $<

debug : WARNS = -Wall -Wl,--subsystem,console
debug : CFLAGS +=  -g
debug : LDFLAGS +=  -g
debug : ${APP}.exe

${APP}.exe : ${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}

clean :
	$(RM) *.o -f
	$(RM) core/*.o -f
	$(RM) 3rdparty/*.o -f
	$(RM) widget/*.o -f

%.o : %.c ${HEADERS}
	${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@

resource.o : resource.rc resource.h
	${RC} ${INCLUDE} -i $< -o $@
