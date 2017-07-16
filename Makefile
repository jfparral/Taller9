all: Servidor Cliente

Servidor: Servidor.c 
	gcc -Wall -g $^ -o $@

Cliente: Cliente.c 
	gcc -Wall $^ -o $@

clean:
	rm -rf cliente servidor
