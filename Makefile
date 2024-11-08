default: build
build:
	gcc -o db src/*.c
	./db
valgrind:
	gcc -o db src/*.c
	valgrind --leak-check=yes ./db
gdb:
	gcc -g -o db src/*.c
	gdb ./db