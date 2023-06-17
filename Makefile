bmp_stenograhpy: bmp_stenograhpy.o info.o reveal.o hide.o
	gcc -o bmp_stenograhpy.o info.o reveal.o hide.o

bmp_stenograhpy.o: bmp_stenograhpy.c
	gcc -c bmp_stenograhpy.c

info.o: info.c
	gcc -c info.c

reveal.o: reveal.c
	gcc -c reveal.c

hide.o: hide.c
	gcc -c hide.c

clean:
	rm bmp_stenography 

