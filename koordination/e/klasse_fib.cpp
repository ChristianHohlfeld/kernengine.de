#include <initializer_list>
#include <cstdio>
struct R{long r=0;}; struct C{ R n24,n26; R Fib(long n){ if(n<2) return {n}; n24=Fib(n-1); n26=Fib(n-2); return {n24.r+n26.r}; } };
R fib(long n){ if(n<2) return {n}; R a=fib(n-1); R b=fib(n-2); return {a.r+b.r}; }
int main(){ for(int k:{5,10,15}){ C c; std::printf("Fib(%d): Klasse heute=%ld  C++=%ld\n",k,c.Fib(k).r,fib(k).r);} }
