#!/bin/bash
# Colors 
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

mkdir mountPoint
output=dfs.wofs
echo -e "${GREEN}../src/mounter.out --image=$output ./mountPoint${NC}"
read -p "Press any key to run and mount $output at mountPoint" -n1 -s
printf "\n"
../src/mounter.out --image=$output ./mountPoint
echo -n -e "${NC}"
printf "\n"