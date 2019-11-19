import timeit
import random

class Backtrack:
    def solve_all(self, solution):
        if self.reject(solution): 
            return
        if self.accept(solution):
            yield solution
        for extension in self.iter(solution):
            yield from self.solve_all(extension)
    
    def solve_first(self, solution):
        if self.reject(solution):
            return None
        if self.accept(solution):
            return solution
        for extension in self.iter(solution):
            possible = self.solve_first(extension)
            if possible is not None:
                return possible
        return None


class QueenSolver(Backtrack):
    def __init__(self, N):
        self.size = N
    
    def reject(self, candidate):
        if not candidate:
             return False
        *prev, (row, col) = candidate
        return any(pr == row or pc == col or abs(pr-row) == abs(pc-col) for pr,pc in prev)

    def accept(self, candidate):
        return len(candidate) == self.size

    def output(self, candidate):
        print(" ".join(chr(ord('a')+row)+str(col+1) for row,col in candidate))

    def iter(self, candidate):
        if len(candidate) < self.size:
            for i in range(self.size):
                yield candidate+[[len(candidate), i]]

    def rand_root(self, k):
        return [[i, random.randrange(self.size)] for i in range(min(k, self.size))]

    def rand_solve(self, k):
        candidate = self.rand_root(k)
        if any(f[0] == s[0] or f[1] == s[1] or abs(f[0]-s[0]) == abs(f[1]-s[1]) for i,f in enumerate(candidate) for s in candidate[:i]):
            return None
        return self.solve_first(candidate)

def argmin(arr):
    return min(enumerate(arr), key=lambda en: en[1])[0]

q8 = QueenSolver(8)
REPEATS = 10
NUMBER_LOOP = 100
times = []
for k in range(9):
    timer = timeit.Timer('while q8.rand_solve(k) is None: pass',globals={'q8':q8, 'k': k})
    times.append(sum(timer.repeat(REPEATS, NUMBER_LOOP))/float(REPEATS))
    print(f"Random-{k} start: {times[-1]} avg per {REPEATS} trials of size {NUMBER_LOOP}")

print(f"Best random start ({min(times)} avg per {REPEATS} trials) of size {NUMBER_LOOP} for k = {argmin(times)}")

for k, time in enumerate(times):
    if time <= times[0]:
        print(f"{times[0]/time:02f}x speedup for Random-{k}")
    else:
        print(f"{time/times[0]:02f}x slowdown for Random-{k}")

print("Successful starts per 1000 trials:")
for k in range(9):
    print(k, '=', len(list(filter(None, [q8.rand_solve(k) for _ in range(1000)]))))

print('all solutions:')
for soln in q8.solve_all([]):
    q8.output(soln)