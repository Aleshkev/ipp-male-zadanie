#!/usr/bin/env python3

"""
Please see python3 ./test.py --help for usage details.
"""

from argparse import ArgumentParser
from dataclasses import dataclass
from math import prod
from os import system
from pathlib import *
import re
from sys import stderr
from textwrap import indent

parser = ArgumentParser(description="Test the program.")
parser.add_argument("--prog", "-p", type=str,
                    help="The program.", default="./labyrinth")
parser.add_argument("--dir", "-d", type=str, default="tests2/",
                    help="Where to get test cases from (recursive unless \
                        --dir-flat).")
parser.add_argument("--dir-flat", "-f", action="store_true",
                    help="Don't recurse while searching for test files.")
parser.add_argument("--filter-err", "-e", type=str, default=None,
                    help="Only do cases which expect the program to raise \
                        one of the errors in the filter. '.' for no error. \
                            E.g. -e.12345 will skip cases with 'ERROR 0'")
parser.add_argument("--lim", "-n", type=int, default=999999,
                    help="Only do so many cases (after filtering).")
parser.add_argument("--abort", "-a", action="store_true",
                    help="Abort on first failed case.")
parser.add_argument("--valgrind", "-m", action="store_true",
                    help="Check memory leaks using valgrind. MAKE SURE THE \
                        PROGRAM IS COMPILED IN DEBUG MODE WITH NO SANITIZERS")
parser.add_argument("--print-all", "-t", action="store_true",
                    help="Print detailed info for each case, even if the \
                        program seems to be okay.")
parser.add_argument("--standardize", action="store_true",
                    help="Some tests disagree with the implementation. If \
                        this flag is enabled, and the detailed error messages \
                            are enabled in the implementation (error.c), the \
                                script will still consider the answers correct.")
parser.add_argument("--timeout", type=int, default=99999,
                    help="Abort the program's execution after this many \
                        seconds (for a single case). The script doesn't \
                            detect these cases, please consult the stderr \
                                and stdout to make an informed decision on \
                                    whether the program was aborted or not.")
parser.add_argument("--ignore-angles", action="store_true",
                    help="Ignore everything in the output between matching \
                        < and > characters.")

args = parser.parse_args()


@dataclass
class Case:
    input_file: Path

    # The order of the execution, depends on the filtering, the sorting etc.
    i: int = -1

    def prop_suffix(suffix):
        return property(lambda self: self.input_file.with_suffix(suffix))

    def prop_read(attr):
        return property(lambda self: attr(self).read_text())

    stdout_file = prop_suffix(".out")
    stderr_file = prop_suffix(".err")
    prog_stdout_file = prop_suffix(".prog.out")
    prog_stderr_file = prop_suffix(".prog.err")

    input = prop_read(lambda self: self.input_file)
    stdout = prop_read(lambda self: self.stdout_file)
    stderr = prop_read(lambda self: self.stderr_file)
    prog_stdout = prop_read(lambda self: self.prog_stdout_file)
    prog_stderr = prop_read(lambda self: self.prog_stderr_file)

    @property
    def size(self):
        try:
            return prod(map(int, self.input.splitlines()[0].split()))
        except:  # Probably invalid input.
            return 696969

    @property
    def stderr_code(self):
        match = re.search(r"ERROR ([0-9])", self.stderr)
        return int(match.group(1)) if match else None

    def __str__(self) -> str:
        return f"{self.input_file} (i = {self.i}, size ≈ {self.size})"


def should_process(case: Case):
    if not args.filter_err:
        return True
    code = case.stderr_code
    return (str(code) if code is not None else '.') in args.filter_err


prog = Path(args.prog)
use_valgrind = args.valgrind

test_dir = Path(args.dir)
test_files = list(test_dir.glob("*.in" if args.dir_flat else "**/*.in"))
cases = [Case(f) for f in test_files]
cases = list(filter(should_process, cases))
cases = sorted(cases, key=lambda x: x.size)
cases = cases[:args.lim]
for i, case in enumerate(cases):
    case.i = i

red = '\033[0;31m'
green = '\033[0;32m'
yellow = '\033[0;33m'
blue = '\033[0;34m'
magenta = '\033[0;35m'
cyan = '\033[36m'
white = '\033[37m'
gray = '\033[30;1m'
reset = '\033[0m'

if not args.ignore_angles:
    assert not args.standardize, "won't find any detailed error messages"

print(f"{len(cases)} cases to go thru")
not_ok = []
try:
    for case in cases:
        print(f"{case}")

        cmd = f"{prog.absolute()} <{case.input_file} >{case.prog_stdout_file} \
            2>{case.prog_stderr_file}"
        if use_valgrind:
            cmd = f"valgrind --error-exitcode=123 --leak-check=full \
                --show-leak-kinds=all --errors-for-leak-kinds=all -s {cmd}"

        cmd = f"timeout --kill-after={args.timeout} {args.timeout}  {cmd}"
        system(cmd)
        prog_stdout = case.prog_stdout
        prog_stderr = case.prog_stderr
        valgrind_out = None

        if use_valgrind:
            lines = prog_stderr.splitlines()
            real = []
            not_real = []
            for line in lines:
                match = re.fullmatch(r"==[0-9]+==(.*)", line)
                if match:
                    not_real.append(match.group(1))
                else:
                    real.append(line)
            prog_stderr = "\n".join(real)
            valgrind_out = "\n".join(not_real)

        out_reduced = prog_stdout
        if args.ignore_angles:
            out_reduced = re.sub(r"<.*>", "", out_reduced)
        out_ok = out_reduced == case.stdout

        err_reduced = prog_stderr
        if args.ignore_angles:
            err_reduced = re.sub(r"<.*>", "", prog_stderr)
        if not use_valgrind:
            err_ok = err_reduced == case.stderr
        else:
            # Whitespace is off when both valgrind and the program write to stderr.
            # So we give up with the checking.
            err_ok = err_reduced.strip() == case.stderr.strip()

        if args.standardize:
            if case.stderr_code == 4 and "ERROR 2" in prog_stderr and "bitset_get(board, start)" in prog_stdout:
                err_ok = True
            if case.stderr_code == 4 and "ERROR 3" in prog_stderr and "bitset_get(board, stop)" in prog_stdout:
                err_ok = True
            if f"ERROR {case.stderr_code - 1}" in prog_stderr and "!isnewline(peekchar())" in prog_stdout:
                err_ok = True

        valgrind_ok = not use_valgrind or (
            "in use at exit: 0 bytes" in valgrind_out)

        all_ok = out_ok and err_ok and valgrind_ok

        if all_ok:
            print(f"  {green}OK{reset}")
        if not all_ok or args.print_all:
            z = []
            if not out_ok:
                z.append("Incorrect stdout.")
            if not err_ok:
                z.append("Incorrect stderr.")
            if not valgrind_ok:
                z.append("Memory leaks???")
            print(f"  {yellow}" + " ".join(z) + f"{reset}")

            def print_block(name: str, block: str, error_color: bool = False, snip: bool = True):
                if len(block) > 160 and snip and args.lim > 1:
                    block = f"{block[:30]}...(total length {len(block)})...{block[-30:]}"
                block = block.replace("\n", f"{gray}↩{reset}\n")
                block += f"{gray}¤{reset}"
                lead = f"  {red if error_color else blue}{name}: {reset}"
                shift = len(name) + 4
                block = indent(block, ' ' * shift)[shift:]
                print(lead + block)

            print_block("       input", case.input)
            print_block("expected out", case.stdout, not out_ok)
            print_block("received out", prog_stdout, not out_ok)
            print_block("expected err", case.stderr, not err_ok)
            print_block("received err", prog_stderr, not err_ok)
            if use_valgrind:
                print_block("valgrind out", valgrind_out,
                            not valgrind_ok, valgrind_ok)

        if not all_ok:
            if args.abort:
                break
            not_ok.append(case)
    print(f"{green}Went thru 'em all.{reset}")
finally:
    if not not_ok:
        print(f"{green}All's well.{reset}")
    else:
        print(f"{yellow}These cases are not pog:{reset}")
        print("\n".join(f"  {red}{case}{reset}" for case in not_ok))
