.PHONY:clean clean-complete type std all run different
all:run
run:
	./test.out
arm-run:
	./test.out --target arm
l1:
	./test.out --level 1
l2:
	./test.out --level 2
l3:
	./test.out --level 3
l4:
	./test.out --level 4
arm-l1:
	./test.out --target arm --level 1
arm-l2:
	./test.out --target arm --level 2
arm-l3:
	./test.out --target arm --level 3
arm-l4:
	./test.out --target arm --level 4
noerrlog:
	./test.out --nolog
diffcheck:
	./test.out --diffcheck
type:
	./test.out --type
std:
	for file in $(basename $(shell find test/?/*.c)); \
	do \
		touch -a $$file.in; \
	done
different:
	for file in $(basename $(shell find test/?/*.c)); \
	do \
		cp -u $$file.c $$file.sy; \
	done
clean:
	rm -rf test/*/*.out test/*/*.output
clean-complete:clean
	rm -rf test/*/*.s test/*/*.res test/*/*.sy test/*/*.output