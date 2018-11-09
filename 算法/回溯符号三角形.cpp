#include <iostream>
#define M 100
using namespace std;

int half = 0;
int n = 0;
int p[M][M] = {0};//1代表+，0代表-
int count  =0;
int sum = 0;

void backtrack(int t) {
    if (count > half || t * (t - 1) / 2 - count > half) {
        return;
    }
    if (t > n) {
        sum++;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n - i + 1; j++) {
                cout << p[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    } else {
        for (int i = 0; i < 2; i++) {
            int tmp = count;
            count+=i;
            p[1][t] = i;
            for (int j = 2; j <= t; j++) {
                p[j][t - j + 1] = (p[j - 1][t - j + 1] ^ p[j - 1][t - j + 2]);
                count+=p[t][t - j + 1];
            }
            backtrack(t + 1);
            count = tmp;
        }
    }
}

int main() {
    cin >> n;
    half = n * (n + 1) / 2;
    if (half % 2 == 1) {
        cout << 0 << endl;
        return 0;
    } 

    half /= 2;
    backtrack(1);
    cout << sum << endl;

    return 0;
}