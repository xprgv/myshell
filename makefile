.PHONY: start

build:
	gcc main.c -o shell

run:
	./shell

start:
	gcc main.c -o shell && ./shell


.DEFAULT_GOAL := start