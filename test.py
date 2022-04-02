#!/usr/bin/env python3

from argparse import ArgumentParser
from dataclasses import dataclass
from math import prod
from os import system
from pathlib import *
import re
from textwrap import indent

parser = ArgumentParser(description="Test the program.", epilog="Skips everything in the output between angular parentheses, e.g. '<error>'.")
parser.add_argument("--prog", "-p", type=str, help="The program.", default="./labyrinth")
parser.add_argument("--dir", "-d", type=str, default="tests2/", help="Where to get test cases from (recursive unless --dir-flat).")
parser.add_argument("--dir-flat", "-f", action="store_true", help="Don't recurse while searching for test files.")
parser.add_argument("--filter-err", "-e", type=str, default=None, 
    help="Only do cases which expect the program to raise one of the errors in the filter. '.' for no error. E.g. -e.12345 will skip cases with 'ERROR 0'")
parser.add_argument("--lim", "-n", type=int, default=999999, help="Only do so many cases (after filtering).")
parser.add_argument("--abort", "-a", action="store_true", help="Abort on first failed case.")
parser.add_argument("--valgrind", "-m", action="store_true", help="Check memory leaks using valgrind.")
parser.add_argument("--print-all", "-t", action="store_true", help="Print detailed info for each case, even if passed.")

args = parser.parse_args()

test_dir = Path(args.dir)
prog = Path(args.prog)
test_files = list(test_dir.glob("*.in" if args.dir_flat else "**/*.in"))
use_valgrind = args.valgrind

def input_file_with_suffix(suffix):
    return property(lambda self: self.input_file.with_suffix(suffix))

def read_file(attr):
    return property(lambda self: attr(self).read_text())

@dataclass
class Case:
    input_file: Path
    i: int = -1  # The order of the execution, depends on the filtering, the sorting etc.

    stdout_file = input_file_with_suffix(".out")
    stderr_file = input_file_with_suffix(".err")
    prog_stdout_file = input_file_with_suffix(".prog.out")
    prog_stderr_file = input_file_with_suffix(".prog.err")

    input = read_file(lambda self: self.input_file)
    stdout = read_file(lambda self: self.stdout_file)
    stderr = read_file(lambda self: self.stderr_file)
    prog_stdout = read_file(lambda self: self.prog_stdout_file)
    prog_stderr = read_file(lambda self: self.prog_stderr_file)

    @property
    def size(self):
        try:
            return prod(map(int, self.input.splitlines()[0].split()))
        except:  # Probably invalid input.
            return 696969

    def __str__(self) -> str:
        return f"{self.input_file} (i = {self.i}, size ≈ {self.size})"

cases = [Case(f) for f in test_files]

def include(case: Case):
    if not args.filter_err: return True
    match = re.search(r"ERROR ([0-9])", case.stderr)
    code = match.group(1) if match else '.'
    return code in args.filter_err

cases = list(filter(include, cases))
cases = sorted(cases, key=lambda x: x.size)
cases = cases[:args.lim]
for i, case in enumerate(cases): case.i = i

red = '\033[0;31m'
green = '\033[0;32m'
yellow = '\033[0;33m'
blue = '\033[0;34m'
magenta = '\033[0;35m'
cyan ='\033[36m'
white ='\033[37m'
gray = '\033[30;1m'
reset ='\033[0m'

print(f"{len(cases)} cases to go thru")

not_ok = []
try:
    for case in cases:
        print(f"{case}")
        
        cmd = f"{prog.absolute()} <{case.input_file} >{case.prog_stdout_file} 2>{case.prog_stderr_file}"
        if use_valgrind: cmd = f"valgrind --error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all -s {cmd}"
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


        out_ok = re.sub(r"<.*>", "", prog_stdout) == case.stdout

        # Whitespace is off when both valgrind and the program write to stderr.
        err_reduced = re.sub(r"<.*>", "", prog_stderr)
        err_ok = err_reduced == case.stderr if not use_valgrind else err_reduced.strip() == case.stderr.strip()
        
        valgrind_ok = not use_valgrind or ("in use at exit: 0 bytes" in valgrind_out)
        
        all_ok = out_ok and err_ok and valgrind_ok

        if all_ok:
            print(f"  {green}OK{reset}")
        if not all_ok or args.print_all:
            z = []
            if not out_ok: z.append("Incorrect stdout.")
            if not err_ok: z.append("Incorrect stderr.")
            if not valgrind_ok: z.append("Memory anomaly.")
            print(f"  {yellow}" + " ".join(z) + f"{reset}")
            
            def print_msg(msg, block: str, color: bool = False, snip: bool = True):
                if len(block) > 160 and snip and args.lim > 1:
                    block = f"{block[:30]}...(total length {len(block)})...{block[-30:]}"
                block = block.replace("\n", f"{gray}↩{reset}\n") + f"{gray}¤{reset}"
                z = f"  {red if color else blue}{msg}{reset}: {indent(block, ' '*16)[16:]}"
                print(z)
            
            print_msg("       input", case.input)
            print_msg("expected out", case.stdout, not out_ok)
            print_msg("received out", prog_stdout, not out_ok)
            print_msg("expected err", case.stderr, not err_ok)
            print_msg("received err", prog_stderr, not err_ok)
            if use_valgrind:
                print_msg("valgrind out", valgrind_out, not valgrind_ok, valgrind_ok)

        if not all_ok:
            if args.abort:
                break
            not_ok.append(case)
    print(f"{green}Went thru 'em all.{reset}")
finally:
    if not not_ok:
        print(f"{green}Everything's okay.{reset}")
    else:
        print(f"{yellow}These cases are not okay:{reset}")
        print("\n".join(f"  {red}{case}{reset}" for case in not_ok))

