# Valgrind may fail to find memory leak
# Use:
```sh
g++ -fsanitize=leak -g main.cxx
```
# on Fedora it require to install:
```sh
sudo dnf install liblsan
```
