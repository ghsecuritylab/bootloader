#!/usr/bin/env python3

import sys
import shutil  
import os  
import os.path
import binascii

path_temp = ".\\temp\\"
path_lib = "..\\lib\\"

if os.path.exists(path_temp) == False:
	os.mkdir(path_temp)
	
if os.path.exists(path_lib) == False:
	os.mkdir(path_lib)

name_bootloader_bin = "bootloader.bin"
path_bootloader_bin = "..\\bootloader\\MDK-ARM\\bootloader\\" + name_bootloader_bin
path_bootloader_bin_temp = path_temp + name_bootloader_bin
name_bootloader_bin_h = "bootloader_bin.h"
path_bootloader_bin_h = path_lib + name_bootloader_bin_h
path_bootloader_bin_h_temp = path_temp + name_bootloader_bin_h

if os.path.exists(path_bootloader_bin) == False:
	sys.exit(0)

shutil.copyfile(path_bootloader_bin,path_bootloader_bin_temp)

file_bootloader_bin = open(path_bootloader_bin_temp,"rb")  
byte_bootloader_bin = file_bootloader_bin.read( )
file_bootloader_bin.close()

str_bootloader_bin = "#ifndef __BOOTLOADER_BIN_H" + "\n"
str_bootloader_bin = str_bootloader_bin + "#define __BOOTLOADER_BIN_H" + "\n"
str_bootloader_bin = str_bootloader_bin + "\n"
str_bootloader_bin = str_bootloader_bin + "#define BOOTLOADER_BIN "
for i in range(len(byte_bootloader_bin)):
	str_bootloader_bin = str_bootloader_bin + hex(byte_bootloader_bin[i]) + ","
str_bootloader_bin = str_bootloader_bin + "\n"
str_bootloader_bin = str_bootloader_bin + "\n"
str_bootloader_bin = str_bootloader_bin + "#endif /* __BOOTLOADER_BIN_H */" + "\n"

file_bootloader_bin_h = open(path_bootloader_bin_h_temp, "w")
file_bootloader_bin_h.write(str_bootloader_bin)
file_bootloader_bin_h.close()

shutil.copyfile(path_bootloader_bin_h_temp,path_bootloader_bin_h)
