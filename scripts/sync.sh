#!/bin/sh

git stage .
git commit -m "$1"
git pull origin master
git push origin master
