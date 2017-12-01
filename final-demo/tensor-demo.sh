#!/bin/bash
# Colors 
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

output=tensorflow.wofs
path=./test-dirs/tensorflow
echo -e "${GREEN}../src/master.out --necc --path=$path --output=$output${NC}"
read -p "Press any key to run" -n1 -s
printf "\n"
../src/master.out --necc --path=$path --output=$output

mkdir mountPoint
echo "Mounting"

../src/mounter.out --necc --image=tensorflow.wofs.necc mountPoint
