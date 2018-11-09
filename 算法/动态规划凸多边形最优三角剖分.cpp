#include <iostream>
#define N 6
using namespace std;

int weight[][N] = {{0,2,2,3,1,4},{2,0,1,5,2,3},{2,1,0,2,1,4},{3,5,2,0,6,2},{1,2,1,6,0,1},{4,3,4,2,1, 0}}; 
int get_weight(const int a, const int b, const int c) {    return weight[a][b] + weight[b][c] + weight[c][a]; }

int t[N][N];
int s[N][N];

void minest() {
    for (int i = 1; i < N; i++) {
        t[i][i] = 0;
    }
    for (int r = 2; r < N; r++) {
        for (int i = 1; i < N - r + 1; i++) {
            int j = i + r - 1;
            int min = 100000;
            for (int k = i; k < j; k++) {
                int tmp = t[i][k] + t[k + 1][j] + get_weight(i - 1, k, j);
                if (tmp < min) {
                    min = tmp;
                    s[i][j] = k;
                }
            }
            t[i][j] = min;
        }
    }
}

void back_track(int a, int b) {
    if (a == b) {
        return;
    }
    back_track(a, s[a][b]);
    back_track(s[a][b] + 1, b);
    cout << "最优三角形V" << a - 1 << " V" << s[a][b] << " V" << "b" << endl;
}

int main() {
    minest();
    cout << "res: " << t[1][N - 1] << endl;
    back_track(1, 5);
    return 0;
}