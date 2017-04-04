#!/bin/bash
umount crypto/
kill -9 `cat .gdbpid`
kill -9 `cat .qemupid`
echo "crypto finished"
rm -rf .gdbpid .qemupid
