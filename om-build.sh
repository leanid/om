#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

declare -a arr=("base-devel" "mingw-w64-x86_64-toolchain" "mingw-w64-x86_64-SDL2")
FREE=`df -k --output=avail "$PWD" | tail -n1`

pacman -Suy --noconfirm 

if [[ $FREE -lt 2350000 ]]; then
     echo -e ${RED}"Not enough free space at current disk. Quitting"${NC}
	 exit 2
fi;

for i in "${arr[@]}"
do
   echo -e ${YELLOW}Installing "$i" ${NC}
   if pacman -Qi "$i" > /dev/null; then 
   echo -e ${GREEN}"The package "$i" is already installed" ${NC}
   else 
   pacman -S --noconfirm --needed "$i"
   fi 
done
 
if [ ! -d ~/om ]; then
  echo -e ${RED}"Directory ~/om doesn't exist. Quitting"${NC}
  exit 3
fi
  
cd ~/om 
mkdir build 
cd build 
cmake ../tests -G "MSYS Makefiles" 
cmake --build . 
exit 0 
