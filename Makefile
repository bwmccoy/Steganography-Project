bmp_stenography: bmp_stenography.o info.o reveal.o hide.o
	gcc -o bmp_stenography bmp_stenography.o info.o reveal.o hide.o 

bmp_stenography.o:
	gcc -c bmp_stenography.c

info.o:
	gcc -c info.c

reveal.o:
	gcc -c reveal.c

hide.o:
	gcc -c hide.c

clean:
	rm *.o bmp_stenography

