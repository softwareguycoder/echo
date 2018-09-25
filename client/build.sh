#!/bin/sh

echo Deleting outputs...
rm -rf client
echo Building client.c...
gcc client.c -o client
echo Build complete!
