#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <vector>

/*
==========================================================================
Back tracking solutions and classes for NQueens problem
==========================================================================
*/

// Used to denote a piece not placed in the column, yet.
constexpr long UNKNOWN = -1;

using namespace std::chrono;

// For stopwatch<>::timeit()...
using process = void (*)(void *);

/**
 * @brief Abstract base class for problems solved via backtracking.
 * @tparam C representation type of a partial solution to the problem.
 */
template <typename C> class SearchProblem {
public:
  /**
   * @brief reject checks for possibility of extending `candidate`.
   * @note A search problem is brute-forced if reject only returns false.
   * @param candidate the (partial) solution to check for rejection.
   * @returns `true` only if no solutions can be extended from `candidate`
   */
  virtual bool reject(C candidate) = 0;

  /**
   * @brief accept checks for completeness and validity of `candidate`.
   * @note CSPs with a reject function only need to check completeness.
   * @param candidate the solution to check for acceptance.
   * @returns `true` only if `candidate` is completely defined and valid.
   */
  virtual bool accept(C candidate) = 0;

  /**
   * @brief extensions creates extensions of the problem from `candidate`.
   * @param candidate the solution used to extend the search space.
   */
  virtual std::vector<C> extensions(C candidate) = 0;
};

/**
 * @brief Backtracking search problem solver framework.
 * @tparam T Subclass of `SearchProblem<C>`. represents the solver.
 * @tparam C renresptantion of solutions in the search space.
 */
template <class T, typename C> class Backtrack {
private:
  T problem;

public:
  /**
   * @brief constructor only needs typecheck T against SearchProblem<C>.
   */
  Backtrack() {
    static_assert(
        std::is_base_of<SearchProblem<C>, T>::value,
        "Backtrack template argument must derive from SearchProblem");
    problem = T();
  }

  /**
   * @brief finds all solutions from a starting candidate.
   * @param candidate (partial) solution to base as search starting point.
   * @returns all solutions that extend `candidate`
   */
  std::vector<C> solutions(C candidate) {
    std::vector<C> solutions{};

    if (problem.reject(candidate)) {
      return solutions;
    }

    if (problem.accept(candidate)) {
      solutions.emplace_back(candidate);
    }

    for (auto &ext : problem.extensions(candidate)) {
      std::insert(std::end(solutions), std::begin(ext), std::end(ext));
    }

    return solutions;
  }

  /**
   * @brief finds first (possibly null) solution extension to `candidate`.
   * @note Similar to BFS or Beam-search 'eqsue searches.
   * @param solution the reference to the found solution
   * @param candidate the starting solution to for searching start point.
   * @returns true if and only if solution is found, otherwise, false.
   */
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

/**
 * @brief NQueens search problem - CSP of finding positions on a grid
 * @tparam N the square dimension (`N` x `N`) size of the grid/chessboard.
 */
template <long N> class NQueens : SearchProblem<std::array<long, N>> {
  using Candidate = std::array<long, N>;

public:
  /**
   * @brief reject placements where any positioned queen can take another.
   * @note pieces can take others diagonally only when ( |dx| == |dy| ).
   */
  bool reject(Candidate candidate) {

    /// @note can be reduced to checking only first empty index (CNF-CSP).
    for (int idx = 0; idx < N; idx++) {
      // If the current index is not filled we have validated everything.
      if (candidate.at(idx) == UNKNOWN) {
        return false;
      }

      // Check against every previous index.
      for (int comp = 0; comp < idx; comp++) {
        /// @note assumes no pieces are placed on same rank.
        // checks for same file between two pieces
        if (candidate.at(idx) == candidate.at(comp)) {
          return true;
        }

        // checks for diagonal takes.
        if (std::abs(candidate.at(idx) - candidate.at(comp)) ==
            std::abs(idx - comp)) {
          return true;
        }
      }
    }
    return false;
  }

  /**
   * @brief checks for complete solutions.
   * @note no validity checks are done. Fails if start candidate is full.
   * @param candidate the candidate to check for valid complete solution.
   * @returns true if `candidate` has all needed pieces, otherwise, false.
   */
  bool accept(Candidate candidate) {
    return (candidate.at(N - 1) != UNKNOWN);
  }

  /**
   * @brief gives all extensions by adding a queen to the next empty rank.
   * @param candidate partial candidate from which to base extensions.
   * @returns all 1-piece extensions if there is an empty rank, else, nil.
   */
  std::vector<Candidate> extensions(Candidate candidate) {
    std::vector<Candidate> extension_set{};

    // First empty rank.
    int idx = 0;
    for (; idx < N && candidate.at(idx) != UNKNOWN; idx++) {
      /* DO NOTHING... just increment idx until empty rank found. */
    }

    // No more extensions needed. Stop search.
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

/**
 * @brief Stopwatch with timer for process functions
 * @tparam Clock the system clock to use for timing resolutions. 
 */
template <typename Clock = std::chrono::steady_clock> class stopwatch {
  typename Clock::time_point last_;

public:
  stopwatch() : last_(Clock::now()) {}

  /** @brief resets the stopwatch */
  void reset() { last_ = Clock::now(); }

  /**
   * @brief gives elapsed duration since last reset or construction.
   * @returns stopwatch time elapsed as `Clock::duration`.
   */
  typename Clock::duration elapsed() const {
    return Clock::now() - last_;
  }

  /** 
   * @brief returns elapsed time and resets stopwatch.
   * @returns elapsed stopwatch time of tick.
   */
  typename Clock::duration tick() {
    auto now = Clock::now();
    auto elapsed = now - last_;
    last_ = now;
    return elapsed;
  }

  /**
   * @brief runs trials of processes. similar to python3 -m timeit module.
   * @param trials How many trials to run the repeated process
   * @param loops number of looped repitions per time trial.
   * @param proc the process/function to run in a trial
   * @param process_args the arguments to pass to the process each time.
   * @returns fastest/minimal time taken by one trial of the trials done.
   */
  typename Clock::duration timeit(long trials, long loops, process proc,
                                  void *process_args) {
    typename Clock::duration best;

    for (int trial = 0; trial < trials; trial++) {
      reset();

      // repeatedly run proc. ignore result for timing.
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

/**
 * @brief finds a solution to NQueens based off random  starting point.
 * @param start_count number of pieces to randomly place as start
 * @returns number of attempts of randomly placing pieces before success.
 */
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

/** @brief proccess wrapper of find_Random */
void solve_proc(void *args) { find_random(*(int *)args); }

int main(int /*argc*/, char * /*argv*/ []) {
  using clock = std::chrono:steady_clock
  stopwatch<clock> sw;

  clock::duration best;
  for (int start_count = 0; start_count < 8; start_count++) {
    std::cout << start_count << " random pieces placed..." << std::endl;

    best = min(best, sw.timeit(10, 10, solve_proc, (void *)&start_count));

    std::cout << find_random(start_count)
              << " attempts before successful random initialization"
              << std::endl;
  }

  std:: cout << "fastest attempt took "
             << duraction_cast<std::chrono::microseconds>(best).count()
             << " when timed."
             << std::endl;
}