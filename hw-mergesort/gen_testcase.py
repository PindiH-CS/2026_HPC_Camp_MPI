#!/usr/bin/env python3
"""Make and check merge-sort testcases locally.

    python gen_testcase.py gen   <n> <prefix>    # writes prefix.in / .out / .txt
    python gen_testcase.py show  <file>          # dump the floats (like od -t f4)
    python gen_testcase.py check <result> <ans>  # compare against the answer

Lets you test without the camp's own testcases.
"""
import struct
import sys


def gen(n, prefix):
    import random
    data = [random.uniform(-1000.0, 1000.0) for _ in range(n)]
    data = list(struct.unpack(f"<{n}f", struct.pack(f"<{n}f", *data)))
    with open(prefix + ".in", "wb") as f:
        f.write(struct.pack(f"<{n}f", *data))
    with open(prefix + ".out", "wb") as f:
        f.write(struct.pack(f"<{n}f", *sorted(data)))
    with open(prefix + ".txt", "w") as f:
        f.write(f"n: {n}\nnodes: 1\ntime: 10\n")
    print(f"wrote {prefix}.in / {prefix}.out / {prefix}.txt  (n = {n})")


def read(path):
    with open(path, "rb") as f:
        raw = f.read()
    return list(struct.unpack(f"<{len(raw) // 4}f", raw))


def check(result, ans):
    a, b = read(result), read(ans)
    if len(a) != len(b):
        print(f"WA: length {len(a)} != {len(b)}")
        return 1
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            print(f"WA: index {i}: got {x}, expected {y}")
            return 1
    print(f"AC ({len(a)} floats)")
    return 0


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    cmd = sys.argv[1]
    if cmd == "gen":
        gen(int(sys.argv[2]), sys.argv[3])
    elif cmd == "show":
        print(read(sys.argv[2]))
    elif cmd == "check":
        sys.exit(check(sys.argv[2], sys.argv[3]))
    else:
        print(__doc__)
        sys.exit(1)
