
ETHERLAB_SOURCE_DIR = ../ethercat-hg
CFLAGS = -g

all:
	gcc $(CFLAGS) led.c -o led -I/opt/etherlab/include -L/opt/etherlab/lib -lethercat -Wl,-rpath -Wl,/opt/etherlab/lib

clean:
	rm -f led led.o

etags:
	etags *.[ch] /opt/etherlab/include/*.h ${ETHERLAB_SOURCE_DIR}/master/*.[ch]

