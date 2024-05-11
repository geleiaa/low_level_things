#!/bin/bash

##############################################################################################################
# Simple script to automate installation of a toolset for rev-eng, bin-exp and mal-analysis on linux vms.
#
#
# This script install: build-essential, binutils, git, gcc, gdb and gdb-peda plugin, objdump, hexdump, 
# xxd, ltrace, strace, python3, pip, ROPgadget, pwntools, die, elfparser ...
#
#
# Create by geleiaa
# Version: 5.0
# changelog: add malware analysis tools
##############################################################################################################


tools='objdump hexdump xxd ltrace strace git python3 pip'

# check if some tools is installed
# later add more tools to check...
is_installed(){

for i in $tools
do
	if test -f /usr/bin/$i
	then
		echo "\n\e[34m $i \e[0m \e[32m IS INSTALLED \e[0m\n"
		sleep 1
	else
		echo "\n \e[34m $i \e[0m \e[31m NOT INSTALLED \e[0m\n"
	        echo "\nINSTALL $i ?? (y/n)\n"
        	read RESP
                	if test $RESP = "y"
	                then
				echo "\n================================="
				echo "INSTALLING \e[34m $i \e[0m..."
				echo "=================================\n"
				sudo apt -y install $i
                	else
                        	echo "\n ...\n"
	                        continue
        	        fi
	fi
done
}



echo "\n================================="
echo "APT UPDATE ..."
echo "=================================\n"

sudo apt update


echo "\n================================="
echo "CHECKING SOME TOOLS ..."
echo "=================================\n"

is_installed


echo "\n================================="
echo "INSTALLING \e[34m build-essential \e[0m..."
echo "=================================\n"

sudo apt -y install build-essential


echo "\n================================="
echo "INSTALLING \e[34m binutils \e[0m..."
echo "=================================\n"

sudo apt -y install binutils


echo "\n================================="
echo "INSTALLING \e[34m gdb \e[0m..."
echo "=================================\n"

sudo apt -y install gdb


echo "\n================================="
echo "INSTALLING \e[34m gdb-peda \e[0m..."
echo "=================================\n"

git clone https://github.com/longld/peda.git ~/peda

echo "source ~/peda/peda.py" >> ~/.gdbinit


echo "\n================================="
echo "DOWNLOADING \e[34m wxHexEditor \e[0m..."
echo "=================================\n"

sudo apt -y install wxhexeditor


echo "\n================================="
echo "INSTALLING \e[34m ROPgadget \e[0m..."
echo "=================================\n"

sudo -H python3 -m pip install ROPgadget


echo "\n================================="
echo "INSTALLING \e[34m PwnTools \e[0m..."
echo "=================================\n"

python3 -m pip install --upgrade pwntools


echo "\n================================="
echo "DOWNLOADING \e[34m Detect-It-Easy \e[0m..."
echo "=================================\n"

wget https://github.com/horsicq/DIE-engine/releases/download/3.08/Detect_It_Easy-3.08-x86_64.AppImage -P ~/Desktop

chmod +x ~/Desktop/Detect_It_Easy-3.08-x86_64.AppImage


echo "\n================================="
echo "DOWNLOADING \e[34m Detect-It-Easy \e[0m..."
echo "=================================\n"

wget http://elfparser.com/release/elfparser_x86_64_1.4.0.deb -P ~/Downloads

sudo dpkg -i ~/Downloads/elfparser_x86_64_1.4.0.deb


echo "\n================================="
echo "DOWNLOADING \e[34m Downloading IDA free \e[0m..."
echo "=================================\n"

wget https://out7.hex-rays.com/files/idafree84_linux.run -P ~/Downloads


echo "\n================================="
echo "\e[32m FINISHED INSTALLATION o/ \e[0m"
echo "=================================\n"
