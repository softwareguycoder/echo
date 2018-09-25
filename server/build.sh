#!/bin/sh

echo Deleting outputs...
rm -rf server
echo Building server.c...
gcc server.c -o server
echo Build complete!
