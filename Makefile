CC = gcc
LD = gcc
RM = rm -rf

.PHONY: clean tp

C_FILES = main.c

C_FLAGS  = -ansi -Wall -Wpedantic
# Shared libraries to link
L_FILES  = gsl gslcblas m
# Include folders

OBJS      = $(patsubst %.c, %.o, $(C_FILES))
TOREMOVE += $(patsubst %.c, %.o, $(C_FILES))
TOREMOVE += $(patsubst %.c, %.d, $(C_FILES))
TOREMOVE += $(addsuffix .dbg,    $(TARGET))

lp: $(OBJS)
	$(LD) $(LDFLAGS) $^ $(addprefix -l, $(L_FILES)) -o $@

run: lp
	@chmod +x lp
	./lp

%.o: %.c
	$(CC) $(CFLAGS) -MD -c $(addprefix -I, $(I_PATH)) $< -o $@

clean:
	$(RM) $(TOREMOVE)
