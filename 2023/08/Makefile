.PHONY: all clean test graph

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

graph: a.out
	mkdir $@ ; \
	for input in tests/*.in ; do \
	  type=svg ; \
	  echo $$input ; \
	  base=$$(basename $$input) ; \
	  dot="$@/$${base%%.*}.dot" ; \
	  cat $$input | ./a.out > $$dot ; \
	  dot -Ksfdp -T$$type $$dot -o $@/$${base%%.*}.$$type ; \
	done

clean:
	rm -vf a.out
	rm -rvf graph
