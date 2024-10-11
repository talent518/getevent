all: getevent sendevent

getevent: getevent.c | input.h-labels.h
	$(CROSS_COMPILE)gcc -static -O2 -o $@ $<

sendevent: sendevent.c | input.h-labels.h
	$(CROSS_COMPILE)gcc -static -O2 -o $@ $<

input.h-labels.h:
	./generate-input.h-labels.py > $@

clean:
	-rm -f getevent sendevent input.h-labels.h
