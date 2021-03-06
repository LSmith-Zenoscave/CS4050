#include <cmath>
#include <iostream>
#include <random>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <trails>";
    exit(EXIT_FAILURE);
  }

  int inside = 0;
  int total = strtol(argv[1], nullptr, 10);

  std::random_device rd;
  std::mt19937 e(rd());
  std::uniform_real_distribution<> dist(-1, 1);
  for (int i = 0; i < total; i++) {
    double x = dist(e);
    double y = dist(e);
    if (hypot(x, y) <= 1) {
      inside++;
    }
  }
  cout << "Affter " << total << " trials, Pi is probably ~"
       << (4.0 * static_cast<double>(inside)) / total << endl;
}
