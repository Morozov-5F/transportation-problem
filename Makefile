CC = gcc
LD = gcc
RM = rm -rf

.PHONY: clean tp

C_FILES = main.c

C_FLAGS  = -ansi -Wall -Wpedantic
LDFLAGS	 = -L/usr/local/lib
# Shared libraries to link
L_FILES  = gsl gslcblas m
# Include folders
I_PATH   = /usr/local/include

OBJS      = $(patsubst %.c, %.o, $(C_FILES))
TOREMOVE += $(patsubst %.c, %.o, $(C_FILES))
TOREMOVE += $(patsubst %.c, %.d, $(C_FILES))
TOREMOVE += $(addsuffix .dbg,    $(TARGET))

run: lp
	@chmod +x lp
	./lp

lp: $(OBJS)
	$(LD) $(LDFLAGS) $^ $(addprefix -l, $(L_FILES)) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MD -c $< -o $@

clean:
	$(RM) $(TOREMOVE)
