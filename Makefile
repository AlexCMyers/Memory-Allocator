best: bettermem.c
	gcc -O2 -DNDEBUG -Wall -fPIC -shared bettermem.c -o libmyalloc.so -lm

mylib: mymem.c
	gcc -O2 -DNDEBUG -Wall -fPIC -shared mymem.c -o libmyalloc.so -lm

test: mymem.c
	gcc -g -Wall -fPIC -shared mymem.c -o libmyalloc.so -lm

better: bettermem.c
	gcc -g -Wall -fPIC -shared bettermem.c -o libmyalloc.so -lm



clean:
	rm *.so
