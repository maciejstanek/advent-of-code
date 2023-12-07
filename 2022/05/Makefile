.PHONY: all clean test

all: a.out

a.out: solution.cxx
	clang++ -g -lboost_system --std=c++20 $<

test: a.out
	for input in tests/*.in ; do \
	  echo $$input ; \
	  cat $$input | ./a.out ; \
	  output="$${input%%.*}.out" ; \
	  echo $$output ; \
		cat $$output ; \
	done

clean:
	rm a.out
