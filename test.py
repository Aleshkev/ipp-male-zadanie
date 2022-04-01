
from asyncio import subprocess
from dataclasses import dataclass
from os import system
from pathlib import *
from textwrap import indent
from typing import List

test_dir = Path("tests2")
test_files = list(test_dir.glob("**/*.in"))[:]

prog = Path("./labyrinth")

@dataclass
class Case:
    input_file: Path

    @property
    def stdout_file(self):
        return self.input_file.with_suffix(".out")
    
    @property
    def stderr_file(self):
        return self.input_file.with_suffix(".err")

    @property
    def prog_stdout_file(self):
        return self.input_file.with_suffix(".prog.out")
    
    @property
    def prog_stderr_file(self):
        return self.input_file.with_suffix(".prog.err")

    @property
    def input(self):
        # print(".")
        return self.input_file.read_text()

    @property
    def stdout(self):
        return self.stdout_file.read_text()

    @property
    def stderr(self):
        return self.stderr_file.read_text()

cases = [Case(f) for f in test_files]

# print(cases)
# print(cases[0].input)

cases = [case for case in cases if "ERROR" not in case.stderr]
cases = sorted(cases, key=lambda x: len(x.input))

red='\033[0;31m'
gray='\033[0;34m'
no_color='\033[0m'

print(f"{len(cases)} to go thru")

not_ok = []
try:
    for i, case in enumerate(cases):
        # print(case)
        print(f"{case.input_file} ({i})")
        
        system(f"{prog.absolute()} <{case.input_file} >{case.prog_stdout_file} 2>{case.prog_stderr_file}")
        prog_stdout = case.prog_stdout_file.read_text()
        prog_stderr = case.prog_stderr_file.read_text()

        out_ok = prog_stdout == case.stdout
        err_ok = prog_stderr == case.stderr

        if out_ok and err_ok:
            print("  All's okay.")
        else:
            z = []
            if not out_ok: z.append("Output's not okay.")
            if not err_ok: z.append("Errors are not okay.")
            print(f"{red}  " + " ".join(z) + f"{no_color}")
            
            def print_msg(msg, block: str):
                z = f"  {msg}: {gray}{block}{no_color}"

            print_msg("       input", case.input.strip())
            print_msg("expected out", case.stdout.strip())
            print_msg("received out", prog_stdout.strip())
            print_msg("expected err", case.stderr.strip())
            print_msg("received err", prog_stderr.strip())

            not_ok.append((i, case.input_file))
            break
    print(not_ok)
finally:
    print(f"These cases are not ok: {not_ok}")

