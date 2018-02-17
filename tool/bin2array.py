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

name_app_bin = "app.bin"
path_app_bin = "..\\app\\MDK-ARM\\app\\" + name_app_bin
path_app_bin_temp = path_temp + name_app_bin
name_app_bin_h = "app_bin.h"
path_app_bin_h = path_lib + name_app_bin_h
path_app_bin_h_temp = path_temp + name_app_bin_h

if os.path.exists(path_app_bin) == False:
	sys.exit(0)

shutil.copyfile(path_app_bin,path_app_bin_temp)

file_app_bin = open(path_app_bin_temp,"rb")  
byte_app_bin = file_app_bin.read( )
file_app_bin.close()

str_app_bin = "#define APP_BIN "
for i in range(len(byte_app_bin)):
	str_app_bin = str_app_bin + hex(byte_app_bin[i]) + ","
str_app_bin = str_app_bin + "\n"

file_app_bin_h = open(path_app_bin_h_temp, "w")
file_app_bin_h.write(str_app_bin)
file_app_bin_h.close()

shutil.copyfile(path_app_bin_h_temp,path_app_bin_h)