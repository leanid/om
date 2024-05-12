# install static version of standard libraries g++
`sudo dnf install libstdc++-devel libstdc++-static glibc-static`
# install static version of standard libraries clang++ with `-stdlib=libc++`
`sudo dnf install libcxx-devel libcxx-static`
# then `g++ -static main.cxx`
# should see a.out
`ldd a.out`
# to see no dependencies
`file a.out`
# to see file info

## compilation process

1. preprocess
`g++ -E main.cxx > prep.cxx`
2. compilation info asm
`g++ -S prep.cxx > prep.s`
3. compilation into binary code (platform dependent)
`g++ -c prep.s > prep.o`
4. how you have to see prep.o (object file) and final step linking
`ld prep.o`
5. previous step will fail. We need `_start` - this is real start application entry point
6. To show read linking command try
`g++ -v -static main.cxx`
7. You should see a lot of CRT(start, end, i, n).o object files and libraries to support exception
handling, IO and other stuff.
8. create minimal example without external CRT code in hack_main.cxx
`g++ -c hack_main.cxx`
`ld hack_main.o /usr/lib64/libc.a`
`./a.out`
9. it should print "hello world"
10. Android example:
`~/Android/Sdk/platform-tools/adb push YOUR_BINARY /data/local/tmp`
`~/Android/Sdk/platform-tools/adb shell`
`chmod +x /data/local/tmp/YOUR_BINARY`
`/data/local/tmp/YOUR_BINARY`
11. compare c++ vs python vs rust - hello world programs using files `main.py`, `main.rs`
If you redirect default output to /dev/video0 - python and rust - show error c++ - no

12. To build `main.go` example:
```go build main.go```
13. To build `main.rs` example:
```rustc main.rs```
14. To build `Main.java` example:
```javac Main.java```
```java Main```
15. (optional) install Java JDK if needed 
```sudo dnf install java-openjdk-devel```
