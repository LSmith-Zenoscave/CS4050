#!python3
import sys
import random
from random import sample
from math import sqrt, ceil


def is_prime_miller_rabin(n, k):
    if n == 2 or n == 3:
        return True
    if (n <= 1) or (n % 2 == 0):
        return False

    d = n - 1
    rs = [d]
    while d % 2 == 0:
        d >>= 1
        rs.append(d)

    witnesses = range(2, n-1)
    for a in sample(witnesses, min(k, len(witnesses))):
        if any(pow(a, r, n) == n - 1 for r in rs) or pow(a,d,n) == 1:
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


def is_prime(n):
    if n in [2, 3, 5]:
        return True

    if n <= 1 or n % 6 in [0, 2, 3, 4]:
        return False

    bound_check = int(ceil(sqrt(n)))+6
    return all((n % (p + 1) != 0 or n == p+1) and n % (p - 1) != 0 for p in range(6, max(7, bound_check), 6))


def generate(size):
    primes = set()
    composites = set()
    for n in range(size+1):
        if is_prime(n):
            primes.add(n)
        else:
            composites.add(n)
    return primes, composites


true = int(True)
false = int(False)


def accuracy(stats):
    correct = stats[true][true] + stats[false][false]
    total_trials = sum(map(sum, stats))
    return round(correct / total_trials, 3)


def precision(stats):
    predicted_true = (stats[true][true] + stats[false][true])
    return round(stats[true][true] / predicted_true, 3)


def recall(stats):
    actually_true = sum(stats[true])
    return round(stats[true][true] / actually_true, 3)


def f1_score(stats):
    pre = precision(stats)
    rec = recall(stats)
    return round(2.0 * pre * rec / (pre + rec), 3)


def main():
    primes, composites = generate(100000)
    print("K,Method,Correct,Incorrect,Accuracy,Precision,Recall,F1")
    for p in range(1, 5):
        k = 10**p
        stats_miller = [[0, 0], [0, 0]]
        stats_rand = [[0, 0], [0, 0]]
        for v in primes | composites:
            miller_rand_predict = is_prime_miller_rabin(v, p)
            random_check_predict = is_prime_rand(v, k)
            stats_miller[int(v in primes)][int(miller_rand_predict)] += 1
            stats_rand[int(v in primes)][int(random_check_predict)] += 1

        print(f"{k},RandCheck," +
              f"{stats_rand[true][true] + stats_rand[false][false]}," +
              f"{stats_rand[true][false] + stats_rand[false][true]}," + 
              f"{accuracy(stats_rand)},{precision(stats_rand)}," + 
              f"{recall(stats_rand)},{f1_score(stats_rand)}")
        
        print(f"{p},MillerRabin," +
              f"{stats_miller[true][true] + stats_miller[false][false]}," +
              f"{stats_miller[true][false] + stats_miller[false][true]}," + 
              f"{accuracy(stats_miller)},{precision(stats_miller)}," + 
              f"{recall(stats_miller)},{f1_score(stats_miller)}")

if __name__ == '__main__':
    main()
