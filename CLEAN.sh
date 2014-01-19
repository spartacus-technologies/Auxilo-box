#!/bin/bash

find . -name \*.o -type f -delete

rm bin/*
rm lib/*

rm CMakeCache.txt

exit 0