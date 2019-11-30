#!python3
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
        return any(
            pr == row or pc == col or abs(pr - row) == abs(pc - col)
            for pr, pc in prev
        )

    def accept(self, candidate):
        return len(candidate) == self.size

    def output(self, candidate):
        print(
            " ".join(
                chr(ord("a") + row) + str(col + 1)
                for row, col in candidate
            )
        )

    def iter(self, candidate):
        if len(candidate) < self.size:
            for i in range(self.size):
                yield candidate + [[len(candidate), i]]

    def rand_root(self, k):
        bounded_k = min(k, self.size)
        return [
            [i, random.randrange(self.size)] for i in range(bounded_k)
        ]

    def rand_solve(self, k):
        candidate = self.rand_root(k)
        if any(
            first_row == second_row
            or first_col == second_col
            or abs(first_row - second_row) == abs(first_col - second_col)
            for i, (first_row, first_col) in enumerate(candidate)
            for (second_row, second_col) in candidate[:i]
        ):
            return None
        return self.solve_first(candidate)


def argmin(arr):
    return min(enumerate(arr), key=lambda en: en[1])[0]


def main():
    REPEATS = 10
    NUMBER_LOOP = 10
    SIZE = 8

    q = QueenSolver(SIZE)
    times = []
    print(f"{SIZE}x{SIZE} board NQueens statistics")
    for k in range(SIZE):
        timer = timeit.Timer(
            "while q.rand_solve(k) is None: pass",
            globals={"q": q, "k": k},
        )
        times.append(min(timer.repeat(REPEATS, NUMBER_LOOP)))
        print(
            f"Random-{k} start: {times[-1]:0.3f}s "
            + f"minimum per {REPEATS} trials of size {NUMBER_LOOP}"
        )

    print(
        f"Best random start ({min(times):0.3f}s minimum per {REPEATS} trials) "
        + f"of size {NUMBER_LOOP} for k = {argmin(times)}"
    )

    for k, time in enumerate(times):
        if time <= times[0]:
            coef = times[0] / time
            print(f"approx. {coef:0.2f}x speedup for Random-{k}")
        else:
            coef = time / times[0]
            print(f"approx. {coef:0.2f}x slowdown for Random-{k}")

    print("Successful starts per 10000 trials:")
    for k in range(SIZE):
        solutions = list([q.rand_solve(k) for _ in range(10000)])
        successes = len([sol for sol in solutions if sol is not None])
        print(f"{k}: {successes}")

    solutions = list(q.solve_all([]))
    print(f"all {len(solutions)} solutions:")
    for soln in solutions:
        q.output(soln)


if __name__ == "__main__":
    main()
