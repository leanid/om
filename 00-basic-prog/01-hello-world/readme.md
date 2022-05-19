# install static version of standard libraries
sudo dnf install libstdc++-static glibc-static
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
g++ -S prep.cxx > prep.s
3. compilation info binarycode (platform dependent)
g++ -c prep.s > prep.o
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
10. Android example:
~/Android/Sdk/platform-tools/adb push YOUR_BINARY /data/local/tmp
~/Android/Sdk/platform-tools/adb shell
chmod +x /data/local/tmp/YOUR_BINARY
/data/local/tmp/YOUR_BINARY
11. compare c++ vs python vs rust - hello world programs using files **main.py**, **main.rs**
If you redirect default output to /dev/video0 - python and rust - show error c++ - not

12. To build main.go example:
```go build main.go```
13. To build main.rs example:
```rustc main.rs```
14. To build Main.java example:
```javac Main.java```
```java Main```
15. (optional) install java and javac if needed 
```sudo dnf install java-openjdk-devel```
