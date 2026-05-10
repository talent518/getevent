CC = gcc

all: getevent sendevent
	@echo -n

getevent: getevent.o
	@echo LD $@
	@$(CC) -static -O2 -o $@ $^

sendevent: sendevent.o
	@echo LD $@
	@$(CC) -static -O2 -o $@ $^

getevent.o: input.h-labels.h
sendevent.o: input.h-labels.h

%.o: %.c
	@echo CC $<
	@$(CC) -c $< -o $@

input.h-labels.h:
	@echo GEN $@
	@./generate-input.h-labels.py $@

clean:
	@rm -vf getevent sendevent input.h-labels.h

