#!/bin/sh

$pwd = `pwd`
cd ~/src/repos/echo
git stage .
git commit -m "$1"
git pull origin master
git push origin master
cd $pwd
