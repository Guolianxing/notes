#include <iostream>
#include <cmath>
#define M 100
using namespace std;

int x[M] = {0};
int sum = 0;
int n = 0;

bool place(int i) {
    for (int j = 1; j < i; j++) {
        if (abs(i - j) == abs(x[i] - x[j]) || x[i] == x[j]) {
            return false;
        }
    }
    return true;
}

void backtrack(int t) {
    if (t > n) {
        sum++;
        for (int i = 1; i <= n; i++) {
            cout << x[i] << " ";
        }
        cout << endl ;
    } else {
        for (int i = 1; i <= n; i++) {
            x[t] = i;
            if (place(t)) {
                backtrack(t + 1);
            }
        }
    }
}

void backtrack() {
    int k = 1;
    x[k] = 0;
    while (k > 0) {
        x[k]++;
        while(x[k] <= n && !place(k)) x[k]++;
        if (x[k] <= n) {
            if (k == n) {
                sum++;
            } else {
                k++;
                x[k] = 0;
            }
        } else {
            k--;
        }
    }
}
int main() {
    n = 8;
    backtrack();
    cout << sum << endl;
    return 0;
}