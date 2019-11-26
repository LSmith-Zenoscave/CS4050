#include <chrono>
#include <iostream>

using namespace std::chrono;
constexpr int INT_RADIX_BASE = 10;

uint64_t mulmod64(uint64_t a, uint64_t b, uint64_t c) {
  uint64_t d; /* to hold the result of a*b mod c */

  /* calculates a*b mod c, stores result in d */
  asm("mov %1, %%rax;"         /* put a into rax */
      "mul %2;"                /* mul a*b -> rdx:rax */
      "div %3;"                /* (a*b)/c -> quot in rax remainder in rdx */
      "mov %%rdx, %0;"         /* store result in d */
      : "=r"(d)                /* output */
      : "r"(a), "r"(b), "r"(c) /* input */
      : "%rax", "%rdx"         /* clobbered registers */
  );
  return d;
}

uint64_t powmod64(uint64_t b, uint64_t n, uint64_t p) {
  uint64_t a = 1;
  goto tst;

mul:
  a = mulmod64(a, b, p);

sqr:
  b = mulmod64(b, b, p);
  n >>= 1;

tst:
  if (n & 1)
    goto mul;
  if (n > 0)
    goto sqr;

  return a;
}

inline bool is_prime(uint64_t n) {
  if (n == 2 || n == 3)
    return true;

  if ((n & 1) == 0 || n <= 1)
    return false;

  uint64_t d = n - 1;
  size_t s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  // These witnesses are sufficient for detirmining all primes less than 2^64.
  uint64_t witnesses[7]{2, 325, 9375, 28178, 450775, 9780504, 1795265022};

  for (int w = 0; w < 7; w++) {
    uint64_t a = witnesses[w];
    if (a == n) {
      continue;
    }

    uint64_t x = powmod64(a, d, n);
    if (x == 1 || x == n - 1) {
      continue;
    }
    bool is_witness = false;
    for (size_t r = 1; r < s; r++) {
      x = mulmod64(x, x, n);
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

uint64_t prod(const uint64_t bases[], uint64_t count) {
  size_t result = 1;
  while (count) {
    count--;
    result *= bases[count];
  }
  return result;
}

uint64_t build_pegs(uint64_t pegs[], const uint64_t pegs_size,
                    const uint64_t bases[], const uint64_t bases_count) {
  uint64_t index = 0;
  for (uint64_t n = 2; n < pegs_size; n++) {
    uint64_t b = 0;
    for (; b < bases_count; b++) {
      if (n % bases[b] == 0) {
        break;
      }
    }
    if (b == bases_count) {
      pegs[index++] = n;
    }
  }
  return index;
}

int main(int argc, char *argv[]) {
  uint64_t count = 1000;
  if (argc > 1) {
    count = strtol(argv[1], NULL, INT_RADIX_BASE);
  }

  long n = 0;
  const uint64_t bases[5]{2, 3, 5, 7, 11};
  const size_t base_size = prod(bases, 5);
  uint64_t *pegs = new uint64_t[base_size];
  const size_t peg_count = build_pegs(pegs, base_size, bases, 5);
  auto start = steady_clock::now();
  uint64_t found = 0;
  for (; found < 5; found++) {
    std::cerr << bases[found] << std::endl;
  }
  for (; found < count; n += base_size) {
    for (uint64_t i = 0; i < peg_count; i++) {
      uint64_t val = n + pegs[i];
      if (is_prime(val)) {
        found++;
        std::cerr << val << std::endl;
        if (found == count) {
          n = val;
          break;
        }
      }
    }
  }
  auto time = steady_clock::now() - start;
  std::cout << "Found the first " << count << " primes ending at " << n
            << " in " << duration_cast<milliseconds>(time).count() << "ms."
            << std::endl;
}