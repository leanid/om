# install static version of standard libraries
sudo dnf install libstdc++-static
sudo dnf search glibc-static
# then g++ -static main.cxx
# should see a.out
ldd a.out
# to see no dependencies
file a.out
# to see file info

## compilation process

1. preprocess
g++ -E main.cxx > prep.cxx
2. compilation info asm
g++ -S prep.cxx
3. compilation info binarycode (platform dependent)
g++ -c prep.s
4. how you have to see prep.o (object file) and final step linking
ld prep.o
5. prefious step will fail. We need some _start - this is real start application entry point
6. To show read linking command try
g++ -v -static main.cxx
7. You should see a lot of crt(start, end, i, n).o object files and linraries to support exception
handling, IO and other stuff.
8. create minimal examle without external crt code in hack_main.cxx
g++ -c hack_main.cxx
ld hack_main.o /usr/lib64/libc.a
./a.out
9. it should print "hello world"
