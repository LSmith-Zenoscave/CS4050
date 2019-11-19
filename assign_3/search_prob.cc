#include <random>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int arr[1000];
    int comparisons = 0;
    for (int t = 0; t < 100; t++)
    {
        for(int i = 0; i < 1000; i++) arr[i] = rand();
        int s_idx = rand() % 1000;
        int s = arr[s_idx];
        for(int g = 0; g < 5000; g++)
        {
            int guess_idx = rand() % 1000;
            comparisons++;
            if (arr[guess_idx] == s) break;
        }
    }

    cout << "Affter 100 trials of 1000 numbers, searching takes " << comparisons / 100.0 << " comparisons" << endl;
}
