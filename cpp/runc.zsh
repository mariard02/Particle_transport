#!/bin/zsh
g++ -std=c++11 -Iinclude main.cpp src/*.cpp -o simulation 
./simulation ../config.json 10

rm simulation