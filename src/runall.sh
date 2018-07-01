#!/bin/sh


time ./run ../example/CASE1.input -o output/CASE1.route
time ./run ../example/CASE2-1.input -o output/CASE2-1.route
time ./run ../example/CASE2-2.input -o output/CASE2-2.route
time ./run ../example/CASE2-3.input -o output/CASE2-3.route
time ./run ../example/CASE3.input -o output/CASE3.route
time ./run ../example/CASE4.input -o output/CASE4.route
time ./run ../example/example_1.input -o output/example_1.route

./verifier ../example/CASE1.input output/CASE1.route
./verifier ../example/CASE2-1.input output/CASE2-1.route
./verifier ../example/CASE2-2.input output/CASE2-2.route
./verifier ../example/CASE2-3.input output/CASE2-3.route
./verifier ../example/CASE3.input output/CASE3.route
./verifier ../example/CASE4.input output/CASE4.route
