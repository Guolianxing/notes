#include <iostream>
#define N 100
using namespace std;

int n = 0;
int M[N][3] = {0};
int x[N] = {0};
int bestx[N] = {0};
int f2[N] = {0};
int f1 = 0;
int f = 0;
int bestf = 100000;


void backtrack(int i) {
    if (i > n) {
        if (f < bestf) {
            for (int j = 1; j <= n; j++) {
                bestx[i] = x[i];
            }
            bestf = f;
        }
    }
    // 全排列，当前i-1个位置都确定下来时，第i个位置的选择为从i~n
    // 将这n-i+1个任务一次放在第i个位置上，如果当前解比最优解更优，则可以继续下去
    for (int j = i; j <= n; j++) {
        f1 += M[x[j]][1];
        f2[i] = (f2[i - 1] > f1 ? f2[i - 1] : f1) + M[x[j]][2];
        f+=f2[i];
        if (f < bestf) {
            swap(x[i], x[j]);
            backtrack(i + 1);
            swap(x[i], x[j]);
        }
        f1-=M[x[j]][1];
        f-=f2[i];
    }
}

int main() {
    cin >> n;
    for (int i = 1; i <= 2; i++) {
        for (int j = 1; j <= n; j++) {
            cin >> M[j][i];
        }
    }
    for (int i = 1; i <= n; i++) {
        x[i] = i;
    }
    backtrack(1);
    cout << bestf << endl;
}