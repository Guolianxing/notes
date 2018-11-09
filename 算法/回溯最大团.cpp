#include <iostream>
#define M 100
using namespace std;

int x[M] = {0};
bool a[M][M];
int bestn = 0;
int bestx[M] = {0};
int cn = 0;
int n = 0;

void backtrack(int t) {
    if (t > n) {
        if (cn > bestn) {
            bestn = cn;
            for (int i = 1; i <= n; i++) {
                bestx[i] = x[i];
            }
        }
        return;
    }
    int ok = 1;
    for (int i = 1; i < t; i++) {
        if (x[i] != 0 && !a[i][t]) {
            ok = 0;
            break;
        }
    }
    if (ok) {
        x[t] = 1;
        cn++;
        backtrack(t + 1);
        x[t] = 0;
        cn--;
    }
    // 当前t-1个节点中选中的 加上 后n-t个未选中的 大于 当前最优解，那么这个节点才有机会不被选中
    // 否则不选这个节点是没意义的，因为递归下去肯定会比bestn小
    if (cn + n - t > bestn) {  
        x[t] = 0;
        backtrack(t + 1);
    }
}

int main() {
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
    for (int i = 1; i <= n; i++) {
        cout << bestx[i] << " ";
    }
    cout << endl;
    cout << bestn << endl;
    return 0;
}