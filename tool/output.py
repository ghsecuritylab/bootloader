#!/usr/bin/env python3

import sys
import shutil  
import os  
import os.path
import binascii

path_output = "..\\output\\"

if os.path.exists(path_output) == False:
	os.mkdir(path_output)

name_bootloader_hex = "bootloader.hex"
path_bootloader_hex = "..\\bootloader\\MDK-ARM\\bootloader\\" + name_bootloader_hex
path_output_bootloader_hex = path_output + name_bootloader_hex

if os.path.exists(path_bootloader_hex) == False:
	sys.exit(0)
	
shutil.copyfile(path_bootloader_hex,path_output_bootloader_hex)
