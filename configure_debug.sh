#!/bin/bash

CC=clang CXX=clang++ cmake -S . -B build -DENABLE_ASAN=OFF -DCMAKE_BUILD_TYPE=Debug -G Ninja
