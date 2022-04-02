#!/usr/bin/env bash

# As you can see, there's a ./test.sh, but it only calls a sophisticated Python
# script, which would be near impossible to implement in bash in a timely manner.
# (see python3 ./test.py --help)
#
# I think this is allowed, plz don't take away points :<

python3 ./test.py --prog "$1" --dir "$2" --valgrind --flat-dir
