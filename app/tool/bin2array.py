#!/usr/bin/env python3

import shutil  
import os  
import os.path
import binascii

src="..\\MDK-ARM\\app\\app.bin"  
dst = "app.bin"

shutil.copyfile(src,dst)

f1 = open(dst,"rb")  
all_the_byte = f1.read( )
f1.close()

all_the_str = "const uint8_t app_bin[] __attribute__((at(USER_FLASH_APP_BASE))) = {"
for i in range(len(all_the_byte)):
	all_the_str = all_the_str + hex(all_the_byte[i]) + ","
all_the_str = all_the_str + "};"
	
# Open a file
f2 = open("app_bin.c", "w")
f2.write(all_the_str)

# Close opend file
f2.close() 