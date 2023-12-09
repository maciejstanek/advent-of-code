.PHONY: all clean test

all: a.out

a.out: solution.cxx
	g++ --std=c++23 -pedantic -Wall -Wextra -Weffc++ $<

test: a.out
	for input in tests/*.in ; do \
	  echo $$input ; \
	  cat $$input | ./a.out | head ; \
	  output="$${input%%.*}.out" ; \
	  echo $$output ; \
	  head $$output ; \
	done

clean:
	rm a.out
