#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include <vector>

constexpr int INT_RADIX_BASE = 10;
std::set<long> primes{};

using namespace std::chrono;

template <typename T> T modpow(T base, T exp, T modulus) {
  base %= modulus;
  T result = 1;
  while (exp > 0) {
    if (exp & 1)
      result = (result * base) % modulus;
    base = (base * base) % modulus;
    exp >>= 1;
  }
  return result;
}

template <typename T> bool miller_rabin(T n, int k) {
  if (n == 2 || n == 3)
    return true;

  if ((n & 1) == 0 || n <= 1)
    return false;

  T d = n - 1;
  size_t s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  for (int w = 0; w < k; w++) {
    T a = (T)(rand() % (n - 3) + 2);
    T x = modpow(a, d, n);
    if (x == 1 || x == n - 1) {
      continue;
    }
    bool is_witness = false;
    for (size_t r = 1; r < s; r++) {
      x = (x * x) % n;
      is_witness |= x == (n - 1);
      if (is_witness) {
        break;
      }
    }
    if (!is_witness) {
      return false;
    }
  }
  return true;
}

template <typename T> bool prime_rand(T n, int k) {
  if (n == 2 || n == 3)
    return true;

  if (n <= 1)
    return false;

  for (int w = 0; w < k; w++) {
    T check = (T)(rand() % (n - 3) + 2);
    if (n % check == 0) {
      return false;
    }
  }
  return true;
}

template <typename T> bool is_actually_prime(T n) {
  if (n == 2 || n == 3)
    return true;

  if ((n & 1) == 0 || n <= 1)
    return false;

  T d = n - 1;
  size_t s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  // Enough witnesses detirmining for all primes less than 2^64.
  std::set<T> witnesses{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

  for (T a : witnesses) {
    if (a == n) {
      continue;
    }

    T x = modpow(a, d, n);
    if (x == 1 || x == n - 1) {
      continue;
    }
    bool is_witness = false;
    for (size_t r = 1; r < s; r++) {
      x = (x * x) % n;
      is_witness |= x == (n - 1);
      if (is_witness) {
        break;
      }
    }
    if (!is_witness) {
      return false;
    }
  }
  return true;
}

std::array<double, 7> stats(size_t stats[2][2], int checks) {
  double correct = stats[0][0] + stats[1][1];
  double incorrect = stats[1][0] + stats[0][1];
  double total = correct + incorrect;
  double accuracy = correct / total;
  double precision =
      stats[1][1] / static_cast<double>(stats[1][1] + stats[0][1]);
  double recall =
      stats[1][1] / static_cast<double>(stats[1][1] + stats[1][0]);
  double f1score = 2 * precision * recall / (precision + recall);

  return std::array<double, 7>{static_cast<double>(checks),
                               correct,
                               incorrect,
                               accuracy,
                               precision,
                               recall,
                               f1score};
}

template <typename T>
std::array<double, 7> trial(bool (*checker)(T, int), T total, int k) {
  size_t count[2][2] = {{0, 0}, {0, 0}};
  for (long n = 0; n < total; n++) {
    bool n_prime = primes.find(n) != primes.end();
    bool n_predict = checker(n, k);
    count[n_prime][n_predict] += 1;
  }
  return stats(count, k);
}

int main(int argc, char *argv[]) {
  srand(time(nullptr));

  signed long total = 1000;
  if (argc > 1) {
    total = strtol(argv[1], NULL, INT_RADIX_BASE);
  }

  for (long n = 0; n < total; n++) {
    if (is_actually_prime(n))
      primes.emplace(n);
  }

  std::cerr << "Tests for numbers [0, " << total << ")" << std::endl;

  std::cout << "Method,K,Correct,Incorrect,Accuracy,Precision,Recall,F1"
            << std::endl;

  for (int size = 1; size < 5; size++) {
    std::cout << "MillerRabin";
    for (double stat : trial(miller_rabin, total, size)) {
      std::cout << "," << stat;
    }
    std::cout << std::endl;
  }

  for (int size = 1; size < 5; size++) {
    int k = modpow(10, size, std::numeric_limits<int>::max());
    std::cout << "RandomCheck";
    for (double stat : trial(prime_rand, total, k)) {
      std::cout << "," << stat;
    }
    std::cout << std::endl;
  }

  size_t count = 1000;
  if (argc > 2) {
    count = strtol(argv[2], NULL, INT_RADIX_BASE);
  }

  auto start = steady_clock::now();
  long n = 0;
  size_t found = 0;
  for (; found < count; n++) {
    if (is_actually_prime(n)) {
      found++;
    }
  }
  auto time = steady_clock::now() - start;
  std::cout << "Found the first " << count << " primes ending with "
            << n - 1 << " in "
            << duration_cast<milliseconds>(time).count() << "ms."
            << std::endl;
}