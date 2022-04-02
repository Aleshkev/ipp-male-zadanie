#!/usr/bin/env bash

# As you can see, there's a ./test.sh, but I prefer to use a Python script,
# the functionality of which would be near impossible to implement in bash
# in a timely manner. (see python3 ./test.py --help)
#
# Jeśli tak nie wolno, proszę wykomentować te dwie linijki i ocenić tego 
# basha :C

python3 ./test.py --prog "$1" --dir "$2" --dir-flat --valgrind
exit

prog="$1"
dir="$2"

for f in $(find $dir -name '*.in'); do
    in=$f
    out=${f%in}out
    err=${f%in}err
    prog_out=${f%in}prog.out
    prog_err=${f%in}prog.err

    echo -n "Example $f"

    $prog <"$in" >"$prog_out" 2>"$prog_err"
    cmp -s "$out" "$prog_out"
    out_ok=$?
    cmp -s "$err" "$prog_err"
    err_ok=$?

    valgrind --error-exitcode=123 --leak-check=full \
        --show-leak-kinds=all --errors-for-leak-kinds=all -s \
        $prog <"$in" >"$prog_out" 2>"$prog_err"
    grep -q "ERROR SUMMARY: 0 errors" "$prog_err"
    mem_ok=$?

    if [ $out_ok -a $err_ok -a $mem_ok ]; then
        echo -ne " works correctly."
    else
        if ! $out_ok; then
            echo -ne "provides invalid stdout "
        fi
        if ! $err_ok; then
            echo -ne "provides invalid stderr "
        fi
        if ! $mem_ok; then
            echo -ne "leaks memory or something "
        fi
    fi
    echo
done
