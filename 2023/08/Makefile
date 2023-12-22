.PHONY: all clean test graph

CC=g++
CXXFLAGS=--std=c++23 -pedantic -Wall -Wextra

all: a.out

a.out: solution.cxx
	$(CC) $(CXXFLAGS) $<

test: a.out
	# for input in tests/*.in ; do \
	#   echo $$input ; \
	#   cat $$input | ./a.out ; \
	#   output="$${input%%.*}.out" ; \
	#   echo $$output ; \
	#   cat $$output ; \
	# done
	# Testing for the simplified case
	cat tests/03.in | ./a.out

graph: a.out
	# Layout: dot, sfdp, neato, fdp, circo, twopi
	mkdir $@ ; \
	for input in tests/*.in ; do \
	  layout=sfdp ; \
	  type=png ; \
	  echo $$input ; \
	  base=$$(basename $$input) ; \
	  dot="$@/$${base%%.*}.dot" ; \
	  cat $$input | ./a.out > $$dot ; \
	  dot -K$$layout -T$$type $$dot -o $@/$${base%%.*}.$$type ; \
	done

clean:
	rm -vf a.out
	rm -rvf graph
