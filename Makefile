.PHONY: all clean

all:
	make -C src
	cp src/main .

clean:
	make -C src clean
	rm -f ./main
