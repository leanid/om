# install static version of standard libraries
sudo dnf install libstdc++-static
sudo dnf search glibc-static
# then g++ -static main.cxx
# should see a.out
ldd a.out
# to see no dependencies
file a.out
# to see file info
