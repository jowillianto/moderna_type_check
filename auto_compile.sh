#!/bin/bash
fswatch -0 -o ./src ./tests ./wrappers CMakeLists.txt | ./compile.sh