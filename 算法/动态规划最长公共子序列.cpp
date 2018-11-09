#include <iostream>
#define M 100
using namespace std;

int m = 6;
int n = 6;
char x[M] = {' ','a', 'd', 'c', 'm', 'h', 'n'};
char y[M] = {' ','a', 'j', 'c', 'h', 'w', 'n'};
int c[M][M];
int b[M][M] = {0};

void LCSLength(int m, int n, char x[], char y[], int c[M][M], int b[M][M]) {
    int i, j;
    for (i = 0; i <= m; i++) {
        c[i][0] = 0;
    }
    for (i = 0; i <= n; i++) {
        c[0][i] = 0;
    }
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= n; j++) {
            if (x[i] == y[j]) {
                b[i][j] = 1;
                c[i][j] = c[i - 1][j - 1] + 1;
            } else if (c[i - 1][j] > c[i][j - 1]) {
                c[i][j] = c[i - 1][j];
                b[i][j] = 2;
            } else {
                c[i][j] = c[i][j - 1];
                b[i][j] = 3;
            }
        }
    }
}

void LCS(int i, int j, char x[], int b[M][M]) {
    if (i == 0 || j == 0) {
        return;
    }
    if (b[i][j] == 1) {
        LCS(i - 1, j - 1, x, b);
        cout << x[i] << " ";
    } else if (b[i][j] == 2) {
        LCS(i - 1, j, x, b);
    } else {
        LCS(i, j - 1, x, b);
    }
}


int main() {
    LCSLength(m, n, x, y, c, b);
    LCS(m, n, x, b);
    return 0;
}