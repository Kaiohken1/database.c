default: build
build:
	gcc -o db src/*.c
	./db
debug:
	gcc -o db src/*.c
	valgrind --leak-check=yes ./db