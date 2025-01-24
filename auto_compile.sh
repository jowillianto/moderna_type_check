#!/bin/bash
fswatch -0 -o ./src ./tests ./bindings CMakeLists.txt | ./compile.sh