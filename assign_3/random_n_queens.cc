#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <vector>

constexpr long UNKNOWN = -1;
using namespace std::chrono;

using process = void (*)(void *);

template <typename C> class SearchProblem {
public:
  virtual bool reject(C candidate) = 0;
  virtual bool accept(C candidate) = 0;
  virtual std::vector<C> extensions(C candidate) = 0;
};

template <class T, typename C> class Backtrack {
private:
  T problem;

public:
  Backtrack() {
    static_assert(
        std::is_base_of<SearchProblem<C>, T>::value,
        "Backtrack template argument must derive from SearchProblem");
    problem = T();
  }

  std::vector<C> solutions(C candidate) {
    std::vector<C> solutions{};
    if (problem.reject(candidate)) {
      return solutions;
    }

    if (problem.accept(candidate)) {
      solutions.emplace_back(candidate);
    }

    for (auto &ext : problem.extensions(candidate)) {
      for (auto &soln : this->solutions(ext)) {
        solutions.emplace_back(soln);
      }
    }

    return solutions;
  }

  bool first(C &solution, C candidate) {
    if (problem.reject(candidate)) {
      return false;
    }
    if (problem.accept(candidate)) {
      solution = candidate;
      return true;
    }
    for (auto &ext : problem.extensions(candidate)) {
      if (first(solution, ext)) {
        return true;
      }
    }
    return false;
  }
};

template <long N> class NQueens : SearchProblem<std::array<long, N>> {
  using Candidate = std::array<long, N>;

public:
  bool reject(Candidate candidate) {
    for (int idx = 0; idx < N; idx++) {
      if (candidate.at(idx) == UNKNOWN) {
        return false;
      }
      for (int comp = 0; comp < idx; comp++) {
        if (candidate.at(idx) == candidate.at(comp)) {
          return true;
        }
        if (std::abs(candidate.at(idx) - candidate.at(comp)) ==
            std::abs(idx - comp)) {
          return true;
        }
      }
    }
    return false;
  }

  bool accept(Candidate candidate) {
    return (candidate.at(N - 1) != UNKNOWN);
  }

  std::vector<Candidate> extensions(Candidate candidate) {
    std::vector<Candidate> extension_set{};
    int idx = 0;
    for (; idx < N && candidate.at(idx) != UNKNOWN; idx++) {
      /* DO NOTHING... just incrementing idx. */
    }
    if (idx == N)
      return extension_set;
    for (int value = 0; value < N; value++) {
      Candidate extension{};
      std::copy(candidate.begin(), candidate.end(), extension.begin());
      extension.at(idx) = value;
      extension_set.emplace_back(extension);
    }
    return extension_set;
  }
};

template <typename Clock = std::chrono::steady_clock> class stopwatch {
  typename Clock::time_point last_;

public:
  stopwatch() : last_(Clock::now()) {}

  void reset() { last_ = Clock::now(); }

  typename Clock::duration elapsed() const {
    return Clock::now() - last_;
  }

  typename Clock::duration tick() {
    auto now = Clock::now();
    auto elapsed = now - last_;
    last_ = now;
    return elapsed;
  }
  typename Clock::duration timeit(long trials, long loops, process proc,
                                  void *process_args) {
    typename Clock::duration best;
    for (int trial = 0; trial < trials; trial++) {
      reset();
      for (int loop = 0; loop < loops; loop++)
        proc(process_args);
      best = std::min(best, tick());
    }
    std::cout << loops << " loops, best of " << trials << " trials: "
              << duration_cast<std::chrono::microseconds>(best).count()
              << " usec per trial" << std::endl;
    return best;
  }
};

int find_random(int start_count) {
  using eight_queens = std::array<long, 8>;
  Backtrack<NQueens<8>, eight_queens> b{};
  eight_queens solution;
  eight_queens start{};
  int attempts = 0;
  do {
    attempts++;
    for (int idx = 0; idx < start_count; idx++) {
      start.at(idx) = rand() % 8;
    }
    for (int idx = start_count; idx < 8; idx++) {
      start.at(idx) = UNKNOWN;
    }
  } while (!b.first(solution, start));
  return attempts;
}

void solve_proc(void *args) { find_random(*(int *)args); }

int main(int /*argc*/, char * /*argv*/ []) {
  stopwatch<> sw;
  for (int start_count = 0; start_count < 8; start_count++) {
    std::cout << start_count << " random pieces placed..." << std::endl;
    sw.timeit(10, 100, solve_proc, (void *)&start_count);
    std::cout << find_random(start_count)
              << " attempts before successful random initialization"
              << std::endl;
  }
}