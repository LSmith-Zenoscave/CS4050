#include <cmath>
#include <random>
#include <iostream>

using namespace std;

std::random_device rd;
std::mt19937 e(rd());
std::uniform_real_distribution<> dist(0, 1);

#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)

typedef double (*unary_real_func)(double);
typedef double (*unary_real_definite_integral)(unary_real_func, double, double, int);

double randrange(double min, double max)
{
    return min + dist(e) * (max - min);
}

// f(x) = sin(1/x)
double fx(double x)
{
    return sin(1.0/x);
}

double mc_avg_integrate(unary_real_func f, double a, double b, int samples)
{
    double sum = 0.0;
    for (int i = 0; i < samples; i++)
    {
        double x = randrange(a, b);
        sum += f(x);
    }
    return (b - a) * (sum / samples);
}

double mc_bound_integrate(unary_real_func f, double a, double b, int samples)
{
    double min_y = INFINITY, max_y = -INFINITY, curr, x, y;
    int inside = 0;
    double delta = (b - a) / samples;
    for (int i = 0; i <= samples; i++)
    {
        curr = f(a+i*delta);
        max_y = MAX(max_y, curr);
        min_y = MIN(min_y, curr);
    }
    for (int i = 0; i < samples; i++)
    {
        y = randrange(min_y, max_y);
        x = randrange(a, b);
        curr = f(x);
        if ((y >= 0 && y <= curr) || (y <= 0 && y >= curr))
            inside++;
    }
    return (b - a) * (max_y - min_y) * inside / samples;
}

double trapezoid_integrate(unary_real_func f, double a, double b, int samples)
{
    double delta = (b - a) / samples;
    double area = 0, last = f(a), curr;
    for (int i = 1; i <= samples; i++)
        curr = f(a + i * delta), area += (curr + last) * delta / 2.0, last = curr;
    return area;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <trails>";
        exit(EXIT_FAILURE);
    }

    int samples = strtol(argv[1], nullptr, 10);
    cout << "Using " << samples << " samples for integration from 0 to 100 of f(x) = sin(1/x) by: " << endl;
    cout << "Expected is ~5.027962." << endl;
    cout << "Trapezoidal Rule: " << trapezoid_integrate(fx, 0.0, 100.0, samples) << endl;
    cout << "Monte-Carlo Mean values: " << mc_avg_integrate(fx, 0.0, 100.0, samples) << endl;
    cout << "Monte-Carlo Bounded samples: " << mc_bound_integrate(fx, 0.0, 100.0, samples) << endl;
}
