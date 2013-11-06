PROJDIR = .
LIBEVENT = /usr
DEBUG = 0

CC = gcc
CPP = g++
SED = sed
CFLAGS += -Wall -Werror -I$(PROJDIR)/include -I$(LIBEVENT)/include
CPPFLAGS += -Wall -Werror -I$(PROJDIR)/include -I$(LIBEVENT)/include

LD = $(CC)

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -D_DEBUG
else
CFLAGS += -g -O2 -DNDEBUG
endif

ifeq ($(PROF), 1)
CFLAGS += -pg
endif

SRC = spawner.c process.c log.c
OBJS = $(SRC:.c=.o)

LIBRARYS=

all: spawner

#.c.o:
#	$(CC) $(CFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) -c -MMD $(CFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	 $(SED) -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	 rm -f $*.d

spawner : $(OBJS)
	$(LD) $(CFLAGS) -o spawner $(OBJS) $(LIBRARYS)

clean:
	rm -f *.o *.P core core.* spawner 

cleansource:
	rm -f $(OBJS)

-include $(SRC:.c=.P)
