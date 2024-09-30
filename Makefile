default: build
build:
	gcc -o db src/*.c
debug:
	gcc -o db src/*.c
	valgrind --leak-check=yes ./db