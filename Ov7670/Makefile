.PHONY: all clean

OUTDIR = ../build
MY_SRCS = $(wildcard *.c)
MY_OBJS = $(MY_SRCS:.c=.o)
MY_DEPS = $(MY_SRCS:.c=.d)

all: ${MY_OBJS}

$(OUTDIR)/%.o:%.c
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	-${RM} ${MY_OBJS} ${MY_DEPS}

