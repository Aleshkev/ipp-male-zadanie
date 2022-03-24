from dataclasses import dataclass
from operator import mul
from string import digits
import sys
from itertools import accumulate

from re import fullmatch
from typing import Tuple


@dataclass
class Tile:
    pos: Tuple[int]

    def __init__(self, pos: Tuple[int]) -> None:
        assert len(pos) == len(dim), "mismatched vector shape"
        assert all(1 <= x <= X for x, X in zip(pos, dim)), "out of bounds"
        super().__init__(pos)

    @property
    def i(self):
        return sum((self.pos[i] - 1) * dim_acc[i] for i in range(len(dim)))

    @property
    def filled(self):
        return mask & (1 << self.i) > 0


def product(l):
    return accumulate(l, mul, 1)


try:
    dim = tuple(map(int, input().split()))
    assert all(1 <= x < 2 ** 64 for x in dim)
    assert len(dim) > 0
except:
    print("ERROR 1")
    sys.exit(1)

dim_acc = [product(dim[:i]) for i in range(len(dim) + 1)]
volume = accumulate(dim, mul, 1)

try:
    start = Tile(tuple(map(int, input().split())))
except:
    print("ERROR 2")
    sys.exit(1)

try:
    stop = Tile(tuple(map(int, input().split())))
except:
    print("ERROR 3")
    sys.exit(1)

try:
    line4 = input().strip()
    if line4.startswith("R"):
        a, b, m, r, s0 = map(int, line4[1:].split())
        assert all(0 <= x < 2 ** 32 for x in [a, b, m, r, s0])
        assert m != 0

        s = [s0]
        while len(s) < r + 1:
            s.append((s[-1] * a + b) % m)
        w = [s_i % volume for s_i in s]
        w_set = set(w)
        mask = 0
        for j in range(volume):
            if j % (2 ** 32) in w_set:
                mask = mask | (1 << j)
    elif line4.startswith("0x"):
        assert line4[2] in digits, "no space between 0x and the hex number allowed"
        mask = int(line4[2:], 16)
    else:
        assert False
    assert (mask >> volume) == 0, "bit beyond the labyrinth's bounds can't be set"
except:
    print("ERROR 4")
    sys.exit(1)

if start.filled:
    print("ERROR 2")
    sys.exit(1)
if stop.filled:
    print("ERROR 3")
    sys.exit(1)

assert not stop.filled

inf = max(69 * sum(dim), 999)  # 999 looks nice, for small test cases
distance = [inf] * volume

distance[start.i] = 0


