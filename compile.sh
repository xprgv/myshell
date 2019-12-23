#!/usr/bash

# script for compiling from source

SOURCE="shell.c"
ELF="shell"

if gcc $SOURCE -o $ELF; 
then 
    echo "Compilation finish sucsessfully"
else
    echo "Compilation finish with error(s)"
    exit 1
fi
