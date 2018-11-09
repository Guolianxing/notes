#include <iostream>
#define M 100
using namespace std;

int n = 0;
int m = 0;
int x[M] = {0};
bool a[M][M];
bool flag = false;

bool ok(int i) {
    for (int j = 1; j < i; j++) {
        if (a[i][j] && x[i] == x[j]) {
            return false;
        }
    }
    return true;
}

void backtrack(int t) {
    if (t > n) {
        flag = true;
        for (int i = 1; i <= n; i++) {
            cout << x[i] << " ";
        }
        cout << endl;
        return;
    }
    for (int i = 1; i <= m; i++) {
        x[t] = i;
        if (ok(t)) {
            backtrack(t + 1);
        }
    }
}

int main() {
    m = 4;
    n = 5;
    a[1][2] = true;
    a[1][4] = true;
    a[1][5] = true;
    a[2][1] = true;
    a[2][5] = true;
    a[2][3] = true;
    a[3][2] = true;
    a[3][5] = true;
    a[4][1] = true;
    a[4][5] = true;
    a[5][1] = true;
    a[5][2] = true;
    a[5][3] = true;

    backtrack(1);
    return 0;
}