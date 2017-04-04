#!/bin/bash
cd ntfsprogs
./configure --disable-debug --disable-extras --enable-really-static
make 
cd ..
rm -rf bin/*
cd bin
ln -s ../ntfsprogs/ntfsprogs/ntfsfix
ln -s ../ntfsprogs/ntfsprogs/ntfsinfo
ln -s ../ntfsprogs/src/lowntfs-3g
ln -s ../ntfsprogs/src/ntfs-3g
cd ..
nasm asm/main.asm -Iasm/
echo "Done building"
