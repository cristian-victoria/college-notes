import sys
from math import isqrt

def bsgs(g, y, p, q):
    if y % p == 1:
        return 0
    m = isqrt(q) + 1

    table = {}
    current  = 1
    for i in range(m):
        if current not in table:
            table[current] = i
        current = (current * g) % p

    g_inverse = pow(g, p-2, p)
    factor = pow(g_inverse, m, p)
    
    d = y % p
    for j in range(m+1):
        if d in table:
            x = j * m + table[d]
            if x < q:
                return x
        d = (d * factor) % p
    return -1

def solution():
    data = sys.stdin.read().strip().split()
    it = iter(data)
    T = int(next(it))
    output = []
    for _ in range(T):
        p = int(next(it))
        g = int(next(it))
        q = int(next(it))
        A = int(next(it))
        B = int(next(it))

        a = bsgs(g % p, A % p, p, q)
        if a == -1:
            output.append("Bad Input")
        else:
            s = pow(B % p, a, p)
            output.append(str(s))
    print("\n".join(output))

if __name__ == "__main__":
    solution()
