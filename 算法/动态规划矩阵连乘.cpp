#include <iostream>
#define M 100
using namespace std;

    int p[M] = {20, 30, 40, 50};
    int n = 3;
    int m[M][M] = {0};
    int s[M][M] = {0};

// 直接动态规划
void MatrixChain(int *p, int n, int m[M][M], int s[M][M]) {
    for (int i = 1; i <= n; i++) {
        m[i][i] = 0;
    }
    for (int r = 2; r <= n; r++) {
        for (int i = 1; i <= n - r + 1; i++) {
            int j = i + r - 1;
            m[i][j] = m[i + 1][j] + p[i - 1] * p[i] * p[j];
            s[i][j] = i;
            for (int k = i + 1; k < j; k++) {
                int t = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];
                if (t < m[i][j]) {
                    m[i][j]  = t;
                    s[i][j] = k;
                }
            }
        }
    }
}

// 备忘录方法，子问题有重复，将已经计算过的子问题记录下来，避免重复求解
int LookupChain(int i, int j) {
    if (m[i][j] > 0) { // 已经计算过的子问题
        return m[i][j];
    }
    if (i == j) {
        return 0;
    }
    int u = LookupChain(i, i) + LookupChain(i + 1, j) + p[i - 1] * p[i] * p[j];
    s[i][j] = i;
    for (int k = i + 1; k < j; k++) {
        int t = LookupChain(i, k) + LookupChain(k + 1, j) + p[i - 1] * p[k] * p[j];
        if (t < u) {
            u = t;
            s[i][j] = k;
        }
    }
    m[i][j] = u;
    return u;
}

void traceback(int s[M][M], int i, int j) {
    if (i == j) {
        return;
    }
    traceback(s, i, s[i][j]);
    traceback(s, s[i][j] + 1, j);
    cout << "Multiply A ["  << i << ": " << s[i][j] << "] and A [" << (s[i][j] + 1) << ": " << j << "]" << endl;
}
int main() {

    //MatrixChain(p, n, m, s);
    LookupChain(1, n);
    cout << m[1][n] << endl;;
    traceback(s, 1, n);
    return 0;
}
