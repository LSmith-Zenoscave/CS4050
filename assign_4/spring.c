#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef __uint128_t uint128_t;
typedef __uint64_t uint64_t;
typedef __uint8_t uint8_t;

uint8_t ascii2hex(char s) {
  uint8_t r = 0xf;

  if ((s >= 48) && (s <= 57)) {
    r = s - 48;
  } else if ((s >= 65) && (s <= 70)) {
    r = s - 55;
  } else if ((s >= 97) && (s <= 102)) {
    r = s - 87;
  }
  return r;
}

uint128_t hex_strtoulll(const char *nptr, int len) {
  int i;
  uint128_t r = 0;
  uint128_t p;

  for (i = len; i >= 0; i--) {
    p = (uint128_t)ascii2hex(*(nptr + i));
    p = p << ((len - i) << 2);
    r |= p;
  }
  return r;
}

uint128_t glfsr(uint128_t mask, uint128_t *lfsr) {
  uint128_t lsb = (*lfsr) & 1U; /* Get LSB (i.e., the output bit). */
  *lfsr >>= 1;                  /* Shift register */
  if (lsb) {                    /* If the output bit is 1, */
    *lfsr ^= mask;              /*  apply toggle mask. */
  }
  return lsb;
}

uint128_t prng(uint128_t *params) {
  uint128_t codeword = 0;
  uint128_t control = glfsr(params[0], &params[1]);
  for (int pos = 0; pos < 128; pos++) {
    codeword <<= 1;
    codeword |= glfsr(params[2 | control], &params[4 | control]);
  }
  return codeword;
}

int main(int argc, char *argv[]) {
  if (argc < 7) {
    fprintf(stderr,
            "usage: %s "
            "control_poly control_start "
            "data1_poly data2_poly "
            "data1_start data2_start \n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  uint128_t params[6];
  for (int i = 0; i < 6; i++) {
    params[i] = hex_strtoulll(argv[i + 1], strlen(argv[i + 1]) - 1);
  }
  for (;;) {
    uint128_t cw = prng(params);
    for (uint128_t shift = 0; shift < 128; shift += 8) {
      printf("%c", (char)((cw >> shift) & 0xffLU));
    }
  }
}