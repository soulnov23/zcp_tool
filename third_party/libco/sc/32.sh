g++ -m32 swapc32.S -c -o swapc32.o -O2
g++ -m32 test32.cpp swapc32.o -o test32 -O2
