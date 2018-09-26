#!/bin/sh

cd ~/src/repos/echo
git pull origin master
git stage .
git commit -m "$1"
git push origin master

