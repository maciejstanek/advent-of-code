.PHONY: all clean test

all:
	cargo build

test:
	for input in input/*.txt ; do \
	  echo $$(tput bold)$$(tput setaf 0)$$(tput setab 2)$$input$$(tput sgr0) ; \
	  cat $$input | cargo run -q ; \
	  output="output/$$(basename -s '.txt' $$input).txt" ; \
	  echo $$(tput bold)$$(tput setaf 0)$$(tput setab 1)$$output$$(tput sgr0) ; \
	  cat $$output ; \
	done

clean:
	rm -rf target
	rm -rf debug
	rm -rf Cargo.lock
