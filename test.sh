prog="$1"
dir="$2"

red='\033[0;91m'
not_red='\033[0m'

for f in $dir/*.in; do
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

    case "$out_ok$err_ok" in
    11) echo -e "$red has incorrect both stdout and stderr";;
    10) echo -e "$red has incorrect stdout";;
    01) echo -e "$red has incorrect stderr";;
    00) echo " works correctly";;
    esac
    echo -n -e $not_red

done
