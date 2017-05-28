# variables
CC = gcc
CC_FLAGS = -Wfatal-errors -g -w -m32

project: src/*.c src/commands/*.c header/*.h header/commands/*.h
	$(CC) $(CC_FLAGS) src/*.c src/commands/*.c -o project


clean:
	rm project
