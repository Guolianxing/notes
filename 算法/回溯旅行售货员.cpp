#include <iostream>
#define MAX_VALUE 100000
#define M 100
using namespace std;

int cc = 0;
int n = 0;
float a[M][M];
float bestc = MAX_VALUE;
float cc;
int x[M] = {0};
int bestx[M] = {0};

void backtrack(int i) {
    if (i == n) {
        if (a[x[n]][x[n - 1]] < MAX_VALUE && a[x[n]][1] < MAX_VALUE && (cc + a[x[n - 1]][x[n]] + a[x[n]][1] < bestc || bestc == MAX_VALUE)) {
            bestc = cc + a[x[n - 1]][x[n]] + a[x[n]][1];
            for (int j = 1; j <= n; j++) {
                bestx[j] = x[j];
            }
        }
        return;
    }
    for (int j = i; j <= n; j++) {
        if (a[x[i - 1]][x[j]] < MAX_VALUE && (cc + a[x[i - 1]][x[j]] < bestc || bestc == MAX_VALUE)) {
            cc += a[x[i - 1]][x[j]];
            swap(x[i], x[j]);
            backtrack(i + 1);
            swap(x[i], x[j]);
            cc -= a[x[i - 1]][x[j]];
        }
    }
}

int main() {
    for (int i = 1; i <= n; i++) {
        x[i] = i;
    }
    return 0;
}