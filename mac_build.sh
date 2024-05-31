#!/bin/bash
g++ -Wall -g -Wno-deprecated -std=c++20 src/*.cpp -framework OpenGL -framework GLUT -o Maze
