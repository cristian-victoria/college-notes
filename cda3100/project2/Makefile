.PHONY: remove
remove:
	rm -f cache_simulator.cpp
	rm sim
	nano cache_simulator.cpp
.PHONY: build
build:
	g++ -o sim cache_simulator.cpp
	./sim lw.mc 4 2 1
.PHONY: edit
edit:
	make remove
	nano cache_simulator.cpp
.PHONY: debug
debug:
	g++ -g -o sim cache_simulator.cpp
	gdb ./sim lw.mc 4 2 1
