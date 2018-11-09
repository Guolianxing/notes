#include <iostream>
#define N 100
using namespace std;

int c1 = 0, c2 = 0;
int n;
int w[N];
int bestw = 0;
int bestx[N] = {0};
int x[N] = {0};
int cw = 0;
int r = 0;

void backtrack(int i) {
    if (i > n) {
        if (cw > bestw) {
            for (int j = 1; j <= n; j++) {
                bestx[j] = x[j];
            }
            bestw = cw;
        }
        return;
    }
    r-=w[i];
    if (cw + w[i] <= c1) {
        x[i] = 1;
        cw += w[i];
        backtrack(i + 1);
        cw-=w[i];
    }
    // 当前载重加上剩下的还没有处理到的集装箱的重量，如果小于当前最优解的话，就不能不装，因为不装下去肯定不会是最优解
    if (cw + r > bestw) {  
        x[i] = 0;
        backtrack(i + 1);
    }
    r+=w[i];
}

int main() {
    cin >> c1 >> c2 >> n;
    r = 0;
    for (int i = 1; i <= n; i++) {
        cin >> w[i];
        r+=w[i];
    }
    backtrack(1);
    if (r - bestw <= c2) {
        cout << "yes" << endl;
    } else {
        cout << "no" << endl;
    }
    return 0;    
}