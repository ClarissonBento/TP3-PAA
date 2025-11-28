all:
	gcc src/main.c src/crypto.c -o tp3.exe

clean:
	rm -f tp3.exe

run:
	./tp3.exe