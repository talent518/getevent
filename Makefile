getevent: getevent.c | input.h-labels.h
	gcc -o $@ $<

input.h-labels.h:
	./generate-input.h-labels.py > $@

clean:
	rm getevent input.h-labels.h
