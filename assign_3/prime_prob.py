#!python3 
import random

def is_prime_miller_rabin(n, k):
    if n == 2 or n == 3:
        return True
    if (n <= 1) or (n % 2 == 0):
        return False

    d = n-1
    rs = [d]
    while d % 2 == 0:
        d >>= 1
        rs.append(d)

    for _ in range(k):
        a = random.randrange(2, n-1)
        if any(pow(a, r, n)in [1,n-1] for r in rs):
            continue
        return False
    return True


def is_prime_rand(n, k):
    if n == 2 or n == 3:
        return True
    if (n <= 1) or (n % 2 == 0):
        return False

    for _ in range(k):
        a = random.randrange(2, n - 1)
        if n % a == 0:
            return False
    return True

def generate(size):
    primes = {2, 3}
    composites = {4}
    p = max(primes)
    while len(primes|composites) < size-2:
        cart = set(p*q for p in primes|composites for q in primes|composites)
        primes |= {c for c in range(2, min(size, 2*p)) if c not in cart}
        composites |= {c for c in cart if c < min(size, 2*p)}
        for v in range(2, max(primes)):
            is_composite = False
            for c in primes:
                if v%c == 0 and v not in primes:
                    is_composite = True
                    break
            if is_composite:
                composites.add(v)
            else:
                primes.add(v)

        p = max(primes)
    return primes, composites


def main():
    primes, composites = generate(10000)
    l = len(composites)
    m = len(primes)

    for p in range(5):
        k = 10**p
        mc = 0
        mp = 0
        rc = 0
        rp = 0
        for v in primes|composites:
            mr = is_prime_miller_rabin(v, p+1)
            rn = is_prime_rand(v, k)
            if mr and v in primes:
                mp += 1
            if not mr and v in composites:
                mc += 1
            if rn and v in primes:
                rp += 1
            if not rn and v in composites:
                rc += 1
        print(f"{p+1},miller-rabin,{mc/l},{l},composite")
        print(f"{k},random,{rc/l},{l},composite")
        print(f"{p+1},miller-rabin,{mp/m},{m},prime")
        print(f"{k},random,{rp/m},{m},prime")

if __name__ == '__main__':
    main()
