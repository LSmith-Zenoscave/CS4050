#include <iostream>
#include <random>

using namespace std;

constexpr int trials = 10000;

auto main(int /*argc*/, char * /*argv*/ []) -> int {
  srand(time(nullptr));
  int arr[1000];
  int successful = 0;
  int comparisons = 0;
  for (int t = 0; t < trials; t++) {
    for (int &i : arr) {
      i = rand();
    }
    int s_idx = rand() % 1000;
    int s = arr[s_idx];
    for (int g = 0; g < 5000; g++) {
      int guess_idx = rand() % 1000;
      comparisons++;
      if (arr[guess_idx] == s) {
        successful++;
        break;
      }
    }
  }

  cout << "Affter " << trials << " trials of 1000 numbers, " << successful
       << " were successful. searching takes "
       << comparisons / float(successful)
       << " comparisons on average when successful" << endl;
}
