.PHONY: all clean test

CC=g++
CXXFLAGS=--std=c++23 -pedantic -Wall -Wextra

all: a.out

a.out: solution.cxx
	$(CC) $(CXXFLAGS) $<

test: a.out
	for input in tests/*.in ; do \
	  echo $$input ; \
	  cat $$input | ./a.out ; \
	  output="$${input%%.*}.out" ; \
	  echo $$output ; \
	  cat $$output ; \
	done

clean:
	rm -vf a.out
