
from asyncio import subprocess
from audioop import mul
from dataclasses import dataclass
from itertools import accumulate
from math import prod
from os import system
from pathlib import *
import re
from textwrap import indent
from typing import List

test_dir = Path("tests2")
test_files = list(test_dir.glob("rentib/**/*.in"))[:]

prog = Path("./labyrinth")

def input_file_with_suffix(suffix):
    return property(lambda self: self.input_file.with_suffix(suffix))

def read_file(attr):
    return property(lambda self: attr(self).read_text())

@dataclass
class Case:
    input_file: Path

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

cases = [Case(f) for f in test_files]

# print(cases)
# print(cases[0].input)

cases = [case for case in cases if "ERROR" not in case.stderr]
# cases = [case for case in cases if "ERROR" in case.stderr and "ERROR 0" not in case.stderr]
cases = sorted(cases, key=lambda x: x.size)

red='\033[0;31m'
gray='\033[0;34m'
no_color='\033[0m'

print(f"{len(cases)} cases to go thru")

not_ok = []
# try:
for i, case in enumerate(cases):
    print(f"{case.input_file} ({i}, {case.size})")
    
    system(f"{prog.absolute()} <{case.input_file} >{case.prog_stdout_file} 2>{case.prog_stderr_file}")
    prog_stdout = case.prog_stdout
    prog_stderr = case.prog_stderr

    out_ok = re.sub(r"<.*>", "", prog_stdout) == case.stdout
    err_ok = re.sub(r"<.*>", "", prog_stderr) == case.stderr

    if out_ok and err_ok:
        print("  OK")
    else:
        z = []
        if not out_ok: z.append("Incorrect stdout.")
        if not err_ok: z.append("Incorrect stderr.")
        print(f"{red}  " + " ".join(z) + f"{no_color}")
        
        def print_msg(msg, block: str):
            if len(block) > 160:
                block = f"{block[:30]}...(total length {len(block)})...{block[-30:]}"
            z = f"  {gray}{msg}{no_color}: {indent(block, ' '*16)[16:]}"
            print(z)
        
        print_msg("       input", case.input)
        print_msg("expected out", case.stdout)
        print_msg("received out", prog_stdout)
        print_msg("expected err", case.stderr)
        print_msg("received err", prog_stderr)

        not_ok.append((i, case.input_file))
        # break
print(not_ok)
# finally:
#     print(f"These cases are not ok: {not_ok}")

