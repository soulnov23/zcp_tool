g++ -m32 coctx_swap.S -c -o coctx_swap32_o2.o -O2
g++ -m32 coctx_swap_push.S -c -o coctx_swap_push_o2.o -O2
g++ -m32 test.cpp coctx_swap32_o2.o -o test32_o2 -O2
g++ -m32 test.cpp coctx_swap_push_o2.o -o test32_push_o2 -O2
g++ -m32 coctx_swap.S -c -o coctx_swap32.o 
g++ -m32 test.cpp coctx_swap32.o -o test32

g++ -m64 coctx_swap.S -c -o coctx_swap64.o 
g++ -m64 coctx_swap.S -c -o coctx_swap64_o2.o -O2
g++ -m64 test.cpp coctx_swap64.o -o test64 
g++ -m64 coctx_swap_push.S -c -o coctx_swap64.o -O2
g++ -m64 test.cpp coctx_swap64_o2.o -o test64_o2 -O2
g++ -m64 test.cpp coctx_swap64.o -o test64_push_o2 -O2

