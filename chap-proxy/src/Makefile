objects = main.o daemon.o worker.o

chap-proxy: $(objects)
	cc $(objects) -o chap-proxy

main.o: main.c daemon.h worker.h
daemon.o: daemon.c daemon.h
worker.o: daemon.c worker.h

.PHONY: clean
clean:
	rm $(objects) chap-proxy
