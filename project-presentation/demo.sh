#!/bin/bash
# Colors 
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

#-------------------------------- Introduction --------------------------------#

echo -e "${CYAN}WRITE-ONCE FILE SYSTEM${NC}"
printf "\n \n"

#-------------------------------- Add Git Repo --------------------------------#
echo "---Get a repo to master---"
read -p "Press any key to begin..." -n1 -s

printf "\n \n \n"

echo -n -e "${GRAY}"
git clone https://github.com/andrewhao/git-poems.git
rm -rf git-poems/.git
echo -n -e "${NC}"

#--------------------------------- Mastering ----------------------------------#

printf "\n"
read -p "Press any key to begin mastering..." -n1 -s
printf "\n \n \n"
echo "---Mastering---"
echo -n "Enter key and press [ENTER]: "

read key
while [[ -z "${key// }" ]]; do
    echo "Please use a valid key (no spaces, nonempty)"
    echo -n "Enter key and press [ENTER]: "
    read key
done

path="./git-poems"
output="./image.wofs"

printf "\n"
echo -e "${GREEN}./exec/master.out --key=$key --path=$path --output=$output${NC}"
read -p "Press any key to run" -n1 -s
echo -n -e "${GRAY}"
./exec/master.out --key=$key --path=$path --output=$output
echo -n -e "${NC}"

printf "\n \n"

echo -e "${GREEN}ls${NC}"
echo -n -e "${GRAY}"
ls
echo -n -e "${NC}"

#------------------------------------ Tree ------------------------------------#

printf "\n \n"
read -p "Press any key to proceed to tree..." -n1 -s
printf "\n \n"
echo "---Tree---"
printf "\n"

echo -e "${GREEN}tree $path${NC}"
read -p "Press any key to run" -n1 -s
printf "\n"
echo -e "${GRAY}"
tree $path
echo -n -e "${NC}"

outputNECC="./image.wofs.necc"
echo -e "${GREEN}./exec/tree.out $outputNECC${NC}"
read -p "Press any key to run" -n1 -s
printf "\n"
echo -n -e "${GRAY}"
./exec/tree.out $outputNECC
echo -n -e "${NC}"

printf "\n \n"
read -p "Press any key to begin mounting..." -n1 -s

#-------------------------------- SHA Mounting --------------------------------#

printf "\n \n"
echo "---Mounting---" 

printf "\n \n"
echo -n -e "${BLUE}Making directory mountPoint...${NC}"
mkdir ./mountPoint
printf "\n"

incorrectKey=incorrect_key
echo -e "${BLUE}Attempting to mount with key: ${RED}$incorrectKey${NC}"
echo -e "${GREEN}./exec/mounter.out --image=$output --key=$incorrectKey ./mountPoint${NC}"
read -p "Press any key to run and mount $output at mountPoint" -n1 -s
printf "\n"
echo -n -e "${GRAY}"
./exec/mounter.out --image=$output --key=$incorrectKey ./mountPoint
echo -n -e "${NC}"

printf "\n \n"
echo "As expected the mount fails!"
printf "\n"

echo "Now let's mount with the correct key"
echo -e "${BLUE}Attempting to mount with key: ${GREEN}$key${NC}"
echo -e "${GREEN}./exec/mounter.out --image=$output --key=$key ./mountPoint${NC}"
read -p "Press any key to run and mount $output at mountPoint" -n1 -s
printf "\n"
echo -n -e "${GRAY}"
./exec/mounter.out --image=$output --key=$key ./mountPoint
echo -n -e "${NC}"

printf "\n \n"
echo "Mount passed!" 
echo -e "Let's run ${GREEN}ls${NC} and ${GREEN}tree${NC} on the mountPoint"
read -p "Press any key to run" -n1 -s
printf "\n"

printf "\n \n"
echo -e "${GREEN}ls mountPoint${NC}"
echo -n -e "${GRAY}"
ls mountPoint
echo -n -e "${NC}"

echo -e "${GREEN}tree mountPoint${NC}"
echo -n -e "${GRAY}"
tree mountPoint
echo -n -e "${NC}"

printf "\n \n"
read -p "Press any key to continue" -n1 -s
printf "\n"

#-------------------------------- ECC Mounting --------------------------------#

printf "\n \n"
echo "Let's remove the mountPoint and check out ECC"
read -p "Press any key to continue" -n1 -s
printf "\n"
sudo umount mountPoint 
rmdir mountPoint

echo "Current Files:"
echo -n -e "${GRAY}"
ls
echo -n -e "${NC}"

printf "\n \n"
echo "Alter $output, close the editor, and then press any key to continue"
bless $output
read -p " " -n1 -s
printf "\n \n"

mkdir ./mountPoint 

echo "Let's attempt to remount with the altered image file"
echo -e "${GREEN}./exec/mounter.out --image=$output --key=$key ./mountPoint${NC}"
read -p "Press any key to run and mount $output at mountPoint" -n1 -s
printf "\n"
echo -n -e "${GRAY}"
./exec/mounter.out --image=$output --key=$key ./mountPoint
echo -n -e "${NC}"

printf "\n \n"
read -p "Press any key to continue" -n1 -s
printf "\n"

echo "Let's look at the edited structure"
echo -e "${GREEN}./exec/tree.out ${output}.necc${NC}"
read -p "Press any key to run and mount $output at mountPoint" -n1 -s
printf "\n"
echo -n -e "${GRAY}"
./exec/tree.out ${output}.necc
echo -n -e "${NC}"


printf "\n \n"
read -p "Press any key to continue" -n1 -s
printf "\n"

echo "Now let's look at the mounted structure"
echo -e "${GREEN}tree ./mountPoint${NC}"
read -p "Press any key to run and mount $output at mountPoint" -n1 -s
printf "\n"
echo -n -e "${GRAY}"
tree ./mountPoint
echo -n -e "${NC}"


printf "\n \n"
read -p "Press any key to continue" -n1 -s
printf "\n"

printf "\n \n \n"
echo "The change has been fixed, thus ECC is successful!"
printf "\n"
echo -e "${GREEN}"
echo -n -e "${GRAY}"
ls
echo -n -e "${NC}"